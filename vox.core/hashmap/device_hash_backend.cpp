//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "hashmap/device_hash_backend.h"

#include "hashmap/hash_map.h"
#include "helper.h"
#include "logging.h"

namespace vox {
namespace core {

std::shared_ptr<DeviceHashBackend> CreateDeviceHashBackend(int64_t init_capacity,
                                                           const Dtype& key_dtype,
                                                           const SizeVector& key_element_shape,
                                                           const std::vector<Dtype>& value_dtypes,
                                                           const std::vector<SizeVector>& value_element_shapes,
                                                           const Device& device,
                                                           const HashBackendType& backend) {
    if (device.GetType() == Device::DeviceType::CPU) {
        return CreateCPUHashBackend(init_capacity, key_dtype, key_element_shape, value_dtypes, value_element_shapes,
                                    device, backend);
    }
#if defined(BUILD_CUDA_MODULE)
    else if (device.GetType() == Device::DeviceType::CUDA) {
        return CreateCUDAHashBackend(init_capacity, key_dtype, key_element_shape, value_dtypes, value_element_shapes,
                                     device, backend);
    }
#endif
    else {
        throw std::runtime_error("Unimplemented device");
    }
}

}  // namespace core
}  // namespace vox
