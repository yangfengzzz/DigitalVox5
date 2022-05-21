//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "kernel/non_zero.h"

#include "device.h"
#include "logging.h"
#include "tensor.h"

namespace vox {
namespace core {
namespace kernel {

Tensor NonZero(const Tensor& src) {
    Device::DeviceType device_type = src.GetDevice().GetType();
    if (device_type == Device::DeviceType::CPU) {
        return NonZeroCPU(src);
    } else if (device_type == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        return NonZeroCUDA(src);
#else
        throw std::runtime_error("Not compiled with CUDA, but CUDA device is used.");
#endif
    } else {
        throw std::runtime_error("NonZero: Unimplemented device");
    }
}

}  // namespace kernel
}  // namespace core
}  // namespace vox
