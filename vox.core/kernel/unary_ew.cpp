//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.core/kernel/unary_ew.h"

#include "vox.base/logging.h"
#include "vox.core/shape_util.h"
#include "vox.core/tensor.h"

namespace vox {
namespace core {
namespace kernel {

void UnaryEW(const Tensor& src, Tensor& dst, UnaryEWOpCode op_code) {
    // Check shape
    if (!shape_util::CanBeBrocastedToShape(src.GetShape(), dst.GetShape())) {
        LOGE("Shape {} can not be broadcasted to {}.", src.GetShape().ToString(), dst.GetShape().ToString());
    }

    // Dispatch to device
    Device src_device = src.GetDevice();
    Device dst_device = dst.GetDevice();
    if (src_device != dst_device) {
        LOGE("Source device {} != destination device {}.", src_device.ToString(), dst_device.ToString());
    }

    if (src_device.GetType() == Device::DeviceType::CPU) {
        UnaryEWCPU(src, dst, op_code);
    } else if (src_device.GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        UnaryEWCUDA(src, dst, op_code);
#else
        LOGE("Not compiled with CUDA, but CUDA device is used.");
#endif
    } else {
        LOGE("UnaryEW Unimplemented device");
    }
}

void Copy(const Tensor& src, Tensor& dst) {
    // Check shape
    if (!shape_util::CanBeBrocastedToShape(src.GetShape(), dst.GetShape())) {
        LOGE("Shape {} can not be broadcasted to {}.", src.GetShape().ToString(), dst.GetShape().ToString());
    }

    // Disbatch to device
    Device::DeviceType src_device_type = src.GetDevice().GetType();
    Device::DeviceType dst_device_type = dst.GetDevice().GetType();
    if ((src_device_type != Device::DeviceType::CPU && src_device_type != Device::DeviceType::CUDA) ||
        (dst_device_type != Device::DeviceType::CPU && dst_device_type != Device::DeviceType::CUDA)) {
        LOGE("Copy: Unimplemented device");
    }
    if (src_device_type == Device::DeviceType::CPU && dst_device_type == Device::DeviceType::CPU) {
        CopyCPU(src, dst);
    } else {
#ifdef BUILD_CUDA_MODULE
        CopyCUDA(src, dst);
#else
        LOGE("Not compiled with CUDA, but CUDA device is used.");
#endif
    }
}

}  // namespace kernel
}  // namespace core
}  // namespace vox
