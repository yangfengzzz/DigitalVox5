//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/shader_binding_table.h"

#include "vox.render/core/device.h"

namespace vox::core {
ShaderBindingTable::ShaderBindingTable(Device &device,
                                       uint32_t handle_count,
                                       VkDeviceSize handle_size_aligned,
                                       VmaMemoryUsage memory_usage)
    : device_{device} {
    VkBufferCreateInfo buffer_info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.usage = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    buffer_info.size = handle_count * handle_size_aligned;

    VmaAllocationCreateInfo memory_info{};
    memory_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    memory_info.usage = memory_usage;

    VmaAllocationInfo allocation_info{};
    auto result = vmaCreateBuffer(device.GetMemoryAllocator(), &buffer_info, &memory_info, &handle_, &allocation_,
                                  &allocation_info);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Could not create ShaderBindingTable"};
    }

    memory_ = allocation_info.deviceMemory;

    VkBufferDeviceAddressInfoKHR buffer_device_address_info{};
    buffer_device_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    buffer_device_address_info.buffer = handle_;
    strided_device_address_region_.deviceAddress =
            vkGetBufferDeviceAddressKHR(device.GetHandle(), &buffer_device_address_info);
    strided_device_address_region_.stride = handle_size_aligned;
    strided_device_address_region_.size = handle_count * handle_size_aligned;

    mapped_data_ = static_cast<uint8_t *>(allocation_info.pMappedData);
}

ShaderBindingTable::~ShaderBindingTable() {
    if (handle_ != VK_NULL_HANDLE && allocation_ != VK_NULL_HANDLE) {
        vmaDestroyBuffer(device_.GetMemoryAllocator(), handle_, allocation_);
    }
}

const VkStridedDeviceAddressRegionKHR *vox::core::ShaderBindingTable::GetStridedDeviceAddressRegion() const {
    return &strided_device_address_region_;
}

uint8_t *ShaderBindingTable::GetData() const { return mapped_data_; }

}  // namespace vox::core
