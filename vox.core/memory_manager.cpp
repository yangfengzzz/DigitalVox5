//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "memory_manager.h"

#include <numeric>
#include <unordered_map>

#include "device.h"
#include "helper.h"
#include "logging.h"
#include "memory_manager_statistic.h"

namespace vox::core {

void* MemoryManager::Malloc(size_t byte_size, const Device& device) {
    void* ptr = GetDeviceMemoryManager(device)->Malloc(byte_size, device);
    MemoryManagerStatistic::GetInstance().CountMalloc(ptr, byte_size, device);
    return ptr;
}

void MemoryManager::Free(void* ptr, const Device& device) {
    // Update statistics before freeing the memory. This ensures a consistent
    // order in case a subsequent Malloc requires the currently freed memory.
    MemoryManagerStatistic::GetInstance().CountFree(ptr, device);
    GetDeviceMemoryManager(device)->Free(ptr, device);
}

void MemoryManager::Memcpy(
        void* dst_ptr, const Device& dst_device, const void* src_ptr, const Device& src_device, size_t num_bytes) {
    // 0-element Tensor's data_ptr_ is nullptr
    if (num_bytes == 0) {
        return;
    } else if (src_ptr == nullptr || dst_ptr == nullptr) {
        LOGE("src_ptr and dst_ptr cannot be nullptr.")
    }

    if ((dst_device.GetType() != Device::DeviceType::CPU && dst_device.GetType() != Device::DeviceType::CUDA) ||
        (src_device.GetType() != Device::DeviceType::CPU && src_device.GetType() != Device::DeviceType::CUDA)) {
        LOGE("MemoryManager::Memcpy: Unimplemented device.")
    }

    std::shared_ptr<DeviceMemoryManager> device_mm;
    if (dst_device.GetType() == Device::DeviceType::CPU && src_device.GetType() == Device::DeviceType::CPU) {
        device_mm = GetDeviceMemoryManager(src_device);
    } else if (src_device.GetType() == Device::DeviceType::CUDA) {
        device_mm = GetDeviceMemoryManager(src_device);
    } else {
        device_mm = GetDeviceMemoryManager(dst_device);
    }

    device_mm->Memcpy(dst_ptr, dst_device, src_ptr, src_device, num_bytes);
}

void MemoryManager::MemcpyFromHost(void* dst_ptr, const Device& dst_device, const void* host_ptr, size_t num_bytes) {
    // Currently default host is CPU:0
    Memcpy(dst_ptr, dst_device, host_ptr, Device("CPU:0"), num_bytes);
}

void MemoryManager::MemcpyToHost(void* host_ptr, const void* src_ptr, const Device& src_device, size_t num_bytes) {
    // Currently default host is CPU:0
    Memcpy(host_ptr, Device("CPU:0"), src_ptr, src_device, num_bytes);
}

std::shared_ptr<DeviceMemoryManager> MemoryManager::GetDeviceMemoryManager(const Device& device) {
    static std::unordered_map<Device::DeviceType, std::shared_ptr<DeviceMemoryManager>, utility::hash_enum_class>
            map_device_type_to_memory_manager = {
                    {Device::DeviceType::CPU, std::make_shared<CPUMemoryManager>()},
#ifdef BUILD_CUDA_MODULE
#ifdef BUILD_CACHED_CUDA_MANAGER
                    {Device::DeviceType::CUDA,
                     std::make_shared<CachedMemoryManager>(std::make_shared<CUDAMemoryManager>())},
#else
                    {Device::DeviceType::CUDA, std::make_shared<CUDAMemoryManager>()},
#endif  // BUILD_CACHED_CUDA_MANAGER
#endif  // BUILD_CUDA_MODULE
            };
    if (map_device_type_to_memory_manager.find(device.GetType()) == map_device_type_to_memory_manager.end()) {
        LOGE("Unimplemented device '{}'.", device.ToString())
    }
    return map_device_type_to_memory_manager.at(device.GetType());
}

}  // namespace vox::core
