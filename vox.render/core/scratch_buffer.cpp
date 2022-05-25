//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/scratch_buffer.h"

#include "vox.render/core/device.h"
#include "vox.render/error.h"

namespace vox::core {
ScratchBuffer::ScratchBuffer(Device &device, VkDeviceSize size) : device_{device}, size_{size} {
    VkBufferCreateInfo buffer_info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    buffer_info.size = size;

    VmaAllocationCreateInfo memory_info{};
    memory_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VmaAllocationInfo allocation_info{};
    auto result = vmaCreateBuffer(device.GetMemoryAllocator(), &buffer_info, &memory_info, &handle_, &allocation_,
                                  &allocation_info);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Could not create Scratchbuffer"};
    }

    memory_ = allocation_info.deviceMemory;

    VkBufferDeviceAddressInfoKHR buffer_device_address_info{};
    buffer_device_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    buffer_device_address_info.buffer = handle_;
    device_address_ = vkGetBufferDeviceAddressKHR(device.GetHandle(), &buffer_device_address_info);
}

ScratchBuffer::~ScratchBuffer() {
    if (handle_ != VK_NULL_HANDLE && allocation_ != VK_NULL_HANDLE) {
        vmaDestroyBuffer(device_.GetMemoryAllocator(), handle_, allocation_);
    }
}

VkBuffer ScratchBuffer::GetHandle() const { return handle_; }

uint64_t ScratchBuffer::GetDeviceAddress() const { return device_address_; }

}  // namespace vox::core
