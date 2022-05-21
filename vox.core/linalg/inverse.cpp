//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "linalg/inverse.h"

#include <unordered_map>

#include "linalg/linalg_headers_cpu.h"

namespace vox {
namespace core {

void Inverse(const Tensor &A, Tensor &output) {
    AssertTensorDtypes(A, {Float32, Float64});

    const Device device = A.GetDevice();
    const Dtype dtype = A.GetDtype();

    // Check dimensions
    SizeVector A_shape = A.GetShape();
    if (A_shape.size() != 2) {
        LOGE("Tensor must be 2D, but got {}D.", A_shape.size());
    }
    if (A_shape[0] != A_shape[1]) {
        LOGE("Tensor must be square, but got {} x {}.", A_shape[0], A_shape[1]);
    }

    int64_t n = A_shape[0];
    if (n == 0) {
        LOGE("Tensor shapes should not contain dimensions with zero.");
    }

    if (device.GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        Tensor ipiv = Tensor::Zeros({n}, core::Int32, device);
        void *ipiv_data = ipiv.GetDataPtr();

        // cuSolver does not support getri, so we have to provide an identity
        // matrix. This matrix is modified in-place as output.
        Tensor A_T = A.T().Contiguous();
        void *A_data = A_T.GetDataPtr();

        output = Tensor::Eye(n, dtype, device);
        void *output_data = output.GetDataPtr();

        InverseCUDA(A_data, ipiv_data, output_data, n, dtype, device);
        output = output.T();
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

        // LAPACKE supports getri, A is in-place modified as output.
        Tensor A_T = A.T().To(device, /*copy=*/true);
        void *A_data = A_T.GetDataPtr();

        InverseCPU(A_data, ipiv_data, nullptr, n, dtype, device);
        output = A_T.T();
    }
}
}  // namespace core
}  // namespace vox
