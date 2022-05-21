//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "linalg/tri.h"

#include "linalg/tri_impl.h"
#include "tensor.h"

namespace vox {
namespace core {

static void CheckInput(const Tensor& A, const int diagonal) {
    // Check dimensions.
    SizeVector A_shape = A.GetShape();
    if (A_shape.size() != 2) {
        LOGE("Tensor must be 2D, but got {}D.", A_shape.size());
    }
    if (A_shape[0] == 0 || A_shape[1] == 0) {
        LOGE("Tensor shapes should not contain dimensions with zero.");
    }
    if (diagonal <= -1 * A_shape[0] || diagonal >= A_shape[1]) {
        LOGE("Diagonal parameter must be between [-{}, {}] for a matrix "
             "with shape {}, but got {}.",
             A_shape[0], A_shape[1], A.GetShape().ToString(), diagonal);
    }
}

void Triu(const Tensor& A, Tensor& output, const int diagonal) {
    CheckInput(A, diagonal);
    core::Device device = A.GetDevice();
    output = core::Tensor::Zeros(A.GetShape(), A.GetDtype(), device);
    if (device.GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        TriuCUDA(A.Contiguous(), output, diagonal);
#else
        LOGE("Unimplemented device.");
#endif
    } else {
        TriuCPU(A.Contiguous(), output, diagonal);
    }
}

void Tril(const Tensor& A, Tensor& output, const int diagonal) {
    CheckInput(A, diagonal);
    core::Device device = A.GetDevice();
    output = core::Tensor::Zeros(A.GetShape(), A.GetDtype(), device);
    if (device.GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        TrilCUDA(A.Contiguous(), output, diagonal);
#else
        LOGE("Unimplemented device.");
#endif
    } else {
        TrilCPU(A.Contiguous(), output, diagonal);
    }
}

void Triul(const Tensor& A, Tensor& upper, Tensor& lower, const int diagonal) {
    CheckInput(A, diagonal);
    core::Device device = A.GetDevice();
    upper = core::Tensor::Zeros(A.GetShape(), A.GetDtype(), device);
    lower = core::Tensor::Zeros(A.GetShape(), A.GetDtype(), device);
    if (device.GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        TriulCUDA(A.Contiguous(), upper, lower, diagonal);
#else
        LOGE("Unimplemented device.");
#endif
    } else {
        TriulCPU(A.Contiguous(), upper, lower, diagonal);
    }
}

}  // namespace core
}  // namespace vox
