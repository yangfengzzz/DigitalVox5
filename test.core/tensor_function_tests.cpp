//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "core_test.h"
#include "tests.h"
#include "vox.base/helper.h"
#include "vox.core/tensor_function.h"

namespace vox::tests {

class TensorFunctionPermuteDevices : public PermuteDevices {};
INSTANTIATE_TEST_SUITE_P(Tensor, TensorFunctionPermuteDevices, testing::ValuesIn(PermuteDevices::TestCases()));

TEST_P(TensorFunctionPermuteDevices, Concatenate) {
    core::Device device = GetParam();

    core::Tensor a, b, c, output_tensor;

    // 0-D cannot be concatenated.
    a = core::Tensor::Init<float>(0, device);
    b = core::Tensor::Init<float>(1, device);
    c = core::Tensor::Init<float>(1, device);
    EXPECT_ANY_THROW(core::Concatenate({a, b, c}, 0));
    EXPECT_ANY_THROW(core::Concatenate({a, b, c}, -1));

    // Same Shape.
    // Concatenating 1-D tensors.
    a = core::Tensor::Init<float>({0, 1, 2}, device);
    b = core::Tensor::Init<float>({3, 4}, device);
    c = core::Tensor::Init<float>({5, 6, 7}, device);

    // 1-D can be concatenated along axis = 0, -1.
    // Default axis is 0.
    output_tensor = core::Concatenate({a, b, c});
    EXPECT_TRUE(output_tensor.AllClose(core::Tensor::Init<float>({0, 1, 2, 3, 4, 5, 6, 7}, device)));

    output_tensor = core::Concatenate({a, b, c}, -1);
    EXPECT_TRUE(output_tensor.AllClose(core::Tensor::Init<float>({0, 1, 2, 3, 4, 5, 6, 7}, device)));

    // 1-D can be concatenated along axis = 1, -2.
    EXPECT_ANY_THROW(core::Concatenate({a, b, c}, 1));
    EXPECT_ANY_THROW(core::Concatenate({a, b, c}, -2));

    // Concatenating 2-D tensors.
    a = core::Tensor::Init<float>({{0, 1}, {2, 3}}, device);
    b = core::Tensor::Init<float>({{4, 5}}, device);
    c = core::Tensor::Init<float>({{6, 7}}, device);

    // Above tensors can be concatenated along axis = 0, -2.
    output_tensor = core::Concatenate({a, b, c}, 0);
    EXPECT_TRUE(output_tensor.AllClose(core::Tensor::Init<float>({{0, 1}, {2, 3}, {4, 5}, {6, 7}}, device)));
    output_tensor = core::Concatenate({a, b, c}, -2);
    EXPECT_TRUE(output_tensor.AllClose(core::Tensor::Init<float>({{0, 1}, {2, 3}, {4, 5}, {6, 7}}, device)));

    // Above 2-D tensors cannot be appended to 2-D along axis = 1, -1.
    EXPECT_ANY_THROW(core::Concatenate({a, b, c}, 1));
    EXPECT_ANY_THROW(core::Concatenate({a, b, c}, -1));

    // Concatenating 2-D tensors of shape {3, 1}.
    a = core::Tensor::Init<float>({{0}, {1}, {2}}, device);
    b = core::Tensor::Init<float>({{3}, {4}, {5}}, device);
    c = core::Tensor::Init<float>({{6}, {7}, {8}}, device);

    // Above tensors can be concatenated along axis = 0, 1, -1, -2.
    output_tensor = core::Concatenate({a, b, c}, 0);
    EXPECT_TRUE(
            output_tensor.AllClose(core::Tensor::Init<float>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}}, device)));
    output_tensor = core::Concatenate({a, b, c}, -2);
    EXPECT_TRUE(
            output_tensor.AllClose(core::Tensor::Init<float>({{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}}, device)));

    output_tensor = core::Concatenate({a, b, c}, 1);

    EXPECT_TRUE(output_tensor.AllClose(core::Tensor::Init<float>({{0, 3, 6}, {1, 4, 7}, {2, 5, 8}}, device)));
    output_tensor = core::Concatenate({a, b, c}, -1);
    EXPECT_TRUE(output_tensor.AllClose(core::Tensor::Init<float>({{0, 3, 6}, {1, 4, 7}, {2, 5, 8}}, device)));

    // 2-D can not be concatenated along axis = 2, -3.
    EXPECT_ANY_THROW(core::Concatenate({a, b, c}, 2));
    EXPECT_ANY_THROW(core::Concatenate({a, b, c}, -3));

    // Using Concatenate for a single tensor. The tensor is split along its
    // first dimension, and concatenated along the axis.
    a = core::Tensor::Init<float>({{{0, 1}, {2, 3}}, {{4, 5}, {6, 7}}, {{8, 9}, {10, 11}}}, device);
    EXPECT_TRUE(core::Concatenate({a}, 1).AllClose(
            core::Tensor::Init<float>({{0, 1, 4, 5, 8, 9}, {2, 3, 6, 7, 10, 11}}, device)));

    // Dtype and Device of both the tensors must be same.
    // Taking the above case of [1, 2] to [2, 2] with different dtype and
    // device.
    EXPECT_ANY_THROW(core::Concatenate({a, b.To(core::Float64), c}));
    if (device.GetType() == core::Device::DeviceType::CUDA) {
        EXPECT_ANY_THROW(core::Concatenate({a, b.To(core::Device("CPU:0")), c}));
    }
}

TEST_P(TensorFunctionPermuteDevices, Append) {
    core::Device device = GetParam();

    core::Tensor self, other, output;

    // Appending 0-D to 0-D.
    self = core::Tensor::Init<float>(0, device);
    other = core::Tensor::Init<float>(1, device);

    // 0-D can be appended to 0-D along axis = null.
    output = core::Append(self, other);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({0, 1}, device)));

    // 0-D can not be appended to 0-D along axis = 0, -1.
    EXPECT_ANY_THROW(core::Append(self, other, 0));
    EXPECT_ANY_THROW(core::Append(self, other, -1));

    // Same Shape.
    // Appending 1-D [3,] self to 1-D [4,].
    self = core::Tensor::Init<float>({0, 1, 2, 3}, device);
    other = core::Tensor::Init<float>({4, 5, 6}, device);

    // 1-D can be appended to 1-D along axis = null, 0, -1.
    output = core::Append(self, other);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({0, 1, 2, 3, 4, 5, 6}, device)));

    output = core::Append(self, other, 0);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({0, 1, 2, 3, 4, 5, 6}, device)));

    output = core::Append(self, other, -1);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({0, 1, 2, 3, 4, 5, 6}, device)));

    // 1-D can not be appended to 1-D along axis = 1, -2.
    EXPECT_ANY_THROW(core::Append(self, other, 1));
    EXPECT_ANY_THROW(core::Append(self, other, -2));

    // Appending 2-D [2, 2] self to 2-D [2, 2].
    self = core::Tensor::Init<float>({{0, 1}, {2, 3}}, device);
    other = core::Tensor::Init<float>({{4, 5}, {6, 7}}, device);

    // 2-D self can be appended to 2-D self along axis = null, 0, 1, -1, -2.
    output = core::Append(self, other);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({0, 1, 2, 3, 4, 5, 6, 7}, device)));

    output = core::Append(self, other, 0);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({{0, 1}, {2, 3}, {4, 5}, {6, 7}}, device)));

    output = core::Append(self, other, -2);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({{0, 1}, {2, 3}, {4, 5}, {6, 7}}, device)));

    output = core::Append(self, other, 1);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({{0, 1, 4, 5}, {2, 3, 6, 7}}, device)));

    output = core::Append(self, other, -1);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({{0, 1, 4, 5}, {2, 3, 6, 7}}, device)));

    // 2-D can not be appended to 2-D along axis = 2, -3.
    EXPECT_ANY_THROW(core::Append(self, other, 2));
    EXPECT_ANY_THROW(core::Append(self, other, -3));

    // Appending 2-D [1, 2] self to 2-D [2, 2].
    self = core::Tensor::Init<float>({{0, 1}, {2, 3}}, device);
    other = core::Tensor::Init<float>({{4, 5}}, device);

    // Only the dimension along the axis can be different, so self of shape
    // [1, 2] can be appended to [2, 2] along axis = null, 0, -2.
    output = core::Append(self, other);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({0, 1, 2, 3, 4, 5}, device)));

    output = core::Append(self, other, 0);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({{0, 1}, {2, 3}, {4, 5}}, device)));

    output = core::Append(self, other, -2);
    EXPECT_TRUE(output.AllClose(core::Tensor::Init<float>({{0, 1}, {2, 3}, {4, 5}}, device)));

    // [1, 2] can not be appended to [2, 2] along axis = 1, -1.
    EXPECT_ANY_THROW(core::Append(self, other, 1));
    EXPECT_ANY_THROW(core::Append(self, other, -1));

    // Dtype and Device of both the tensors must be same.
    // Taking the above case of [1, 2] to [2, 2] with different dtype and
    // device.
    EXPECT_ANY_THROW(core::Append(self, other.To(core::Float64)));
    if (device.GetType() == core::Device::DeviceType::CUDA) {
        EXPECT_ANY_THROW(core::Append(self, other.To(core::Device("CPU:0"))));
    }

    // output = core::Append(self, other);
    // is same as:
    // output = self.Append(other);
    EXPECT_TRUE(core::Append(self, other).AllClose(self.Append(other)));
}

}  // namespace vox::tests
