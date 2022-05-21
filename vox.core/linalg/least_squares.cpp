//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "linalg/least_squares.h"

#include <unordered_map>

namespace vox {
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
}  // namespace vox
