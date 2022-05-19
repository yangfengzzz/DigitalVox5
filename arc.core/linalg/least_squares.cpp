// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "linalg/least_squares.h"

#include <unordered_map>

namespace arc {
namespace core {

void LeastSquares(const Tensor &A, const Tensor &B, Tensor &X) {
    AssertTensorDtypes(A, {Float32, Float64});
    AssertTensorDevice(B, A.GetDevice());
    AssertTensorDtype(B, A.GetDtype());

    const Device device = A.GetDevice();
    const Dtype dtype = A.GetDtype();

    // Check dimensions
    SizeVector A_shape = A.GetShape();
    SizeVector B_shape = B.GetShape();
    if (A_shape.size() != 2) {
        LOGE("Tensor A must be 2D, but got {}D", A_shape.size());
    }
    if (B_shape.size() != 1 && B_shape.size() != 2) {
        LOGE("Tensor B must be 1D (vector) or 2D (matrix), but got {}D.", B_shape.size());
    }
    if (B_shape[0] != A_shape[0]) {
        LOGE("Tensor A and B's first dimension mismatch.");
    }

    int64_t m = A_shape[0];
    int64_t n = A_shape[1];
    int64_t k = B_shape.size() == 2 ? B_shape[1] : 1;
    if (m == 0 || n == 0 || k == 0) {
        LOGE("Tensor shapes should not contain dimensions with zero.");
    }

    if (m < n) {
        LOGE("Tensor A shape must satisfy rows({}) > cols({}).", m, n);
    }

    // A and B are modified in-place
    Tensor A_copy = A.T().Clone();
    Tensor B_copy = B.T().Clone();

    void *A_data = A_copy.GetDataPtr();
    void *B_data = B_copy.GetDataPtr();

    if (device.GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        LeastSquaresCUDA(A_data, B_data, m, n, k, dtype, device);
#else
        LOGE("Unimplemented device.");
#endif
    } else {
        LeastSquaresCPU(A_data, B_data, m, n, k, dtype, device);
    }

    X = B_copy.T().Slice(0, 0, n);
}
}  // namespace core
}  // namespace arc