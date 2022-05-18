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

#include "kernel/unary_ew.h"

#include "logging.h"
#include "shape_util.h"
#include "tensor.h"

namespace arc {
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
}  // namespace arc
