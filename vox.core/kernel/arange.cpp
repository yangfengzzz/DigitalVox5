//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "kernel/arange.h"

#include "tensor.h"
#include "tensor_check.h"

namespace vox {
namespace core {
namespace kernel {

Tensor Arange(const Tensor& start, const Tensor& stop, const Tensor& step) {
    AssertTensorShape(start, {});
    AssertTensorShape(stop, {});
    AssertTensorShape(step, {});

    Device device = start.GetDevice();
    Device::DeviceType device_type = device.GetType();
    AssertTensorDevice(stop, device);
    AssertTensorDevice(step, device);

    int64_t num_elements = 0;
    bool is_arange_valid = true;

    Dtype dtype = start.GetDtype();
    DISPATCH_DTYPE_TO_TEMPLATE(dtype, [&]() {
        scalar_t sstart = start.Item<scalar_t>();
        scalar_t sstop = stop.Item<scalar_t>();
        scalar_t sstep = step.Item<scalar_t>();

        if (sstep == 0) {
            LOGE("Step cannot be 0");
        }
        if (sstart == sstop) {
            is_arange_valid = false;
        }

        num_elements =
                static_cast<int64_t>(std::ceil(static_cast<double>(sstop - sstart) / static_cast<double>(sstep)));
        if (num_elements <= 0) {
            is_arange_valid = false;
        }
    });

    // Special case.
    if (!is_arange_valid) {
        return Tensor({0}, dtype, device);
    }

    // Input parameters.
    std::unordered_map<std::string, core::Tensor> srcs = {
            {"start", start},
            {"step", step},
    };

    // Output.
    Tensor dst = Tensor({num_elements}, dtype, device);

    if (device_type == Device::DeviceType::CPU) {
        ArangeCPU(start, stop, step, dst);
    } else if (device_type == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        ArangeCUDA(start, stop, step, dst);
#else
        LOGE("Not compiled with CUDA, but CUDA device is used.");
#endif
    } else {
        LOGE("Arange: Unimplemented device.");
    }

    return dst;
}

}  // namespace kernel
}  // namespace core
}  // namespace vox
