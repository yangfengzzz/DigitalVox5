//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "linalg/solve.h"

#ifdef BUILD_CUDA_MODULE
#include <cuda_runtime_api.h>
#endif

#include <unordered_map>

#include "linalg/linalg_headers_cpu.h"

namespace vox {
namespace core {

void Solve(const Tensor &A, const Tensor &B, Tensor &X) {
    AssertTensorDtypes(A, {Float32, Float64});
    const Device device = A.GetDevice();
    const Dtype dtype = A.GetDtype();

    AssertTensorDtype(B, dtype);
    AssertTensorDevice(B, device);

    // Check dimensions
    SizeVector A_shape = A.GetShape();
    SizeVector B_shape = B.GetShape();
    if (A_shape.size() != 2) {
        LOGE("Tensor A must be 2D, but got {}D", A_shape.size());
    }
    if (A_shape[0] != A_shape[1]) {
        LOGE("Tensor A must be square, but got {} x {}.", A_shape[0], A_shape[1]);
    }
    if (B_shape.size() != 1 && B_shape.size() != 2) {
        LOGE("Tensor B must be 1D (vector) or 2D (matrix), but got {}D", B_shape.size());
    }
    if (B_shape[0] != A_shape[0]) {
        LOGE("Tensor A and B's first dimension mismatch.");
    }

    int64_t n = A_shape[0];
    int64_t k = B_shape.size() == 2 ? B_shape[1] : 1;
    if (n == 0 || k == 0) {
        LOGE("Tensor shapes should not contain dimensions with zero.");
    }

    // A and B are modified in-place
    Tensor A_copy = A.T().Clone();
    void *A_data = A_copy.GetDataPtr();

    X = B.T().Clone();
    void *B_data = X.GetDataPtr();

    if (device.GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        Tensor ipiv = Tensor::Empty({n}, core::Int32, device);
        void *ipiv_data = ipiv.GetDataPtr();

        SolveCUDA(A_data, B_data, ipiv_data, n, k, dtype, device);
#else
        LOGE("Unimplemented device.");
#endif
    } else {
        Dtype ipiv_dtype;
        if (sizeof(OPEN3D_CPU_LINALG_INT) == 4) {
            ipiv_dtype = core::Int32;
        } else if (sizeof(OPEN3D_CPU_LINALG_INT) == 8) {
            ipiv_dtype = core::Int64;
        } else {
            LOGE("Unsupported OPEN3D_CPU_LINALG_INT type.");
        }
        Tensor ipiv = Tensor::Empty({n}, ipiv_dtype, device);
        void *ipiv_data = ipiv.GetDataPtr();

        SolveCPU(A_data, B_data, ipiv_data, n, k, dtype, device);
    }
    X = X.T();
}
}  // namespace core
}  // namespace vox
