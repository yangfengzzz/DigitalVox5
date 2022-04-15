//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "buffer.h"

#include "device.h"

namespace vox::core {
Buffer::Buffer(Device const &device, VkDeviceSize size, VkBufferUsageFlags buffer_usage,
               VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags,
               const std::vector<uint32_t> &queue_family_indices) :
VulkanResource{VK_NULL_HANDLE, &device},
size_{size} {
#ifdef VK_USE_PLATFORM_METAL_EXT
    // Workaround for Mac (MoltenVK requires unmapping https://github.com/KhronosGroup/MoltenVK/issues/175)
    // Force clears the flag VMA_ALLOCATION_CREATE_MAPPED_BIT
    flags &= ~VMA_ALLOCATION_CREATE_MAPPED_BIT;
#endif
    
    persistent_ = (flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;
    
    VkBufferCreateInfo buffer_info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.usage = buffer_usage;
    buffer_info.size = size;
    if (queue_family_indices.size() >= 2) {
        buffer_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        buffer_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size());
        buffer_info.pQueueFamilyIndices = queue_family_indices.data();
    }
    
    VmaAllocationCreateInfo memory_info{};
    memory_info.flags = flags;
    memory_info.usage = memory_usage;
    
    VmaAllocationInfo allocation_info{};
    auto result = vmaCreateBuffer(device.get_memory_allocator(),
                                  &buffer_info, &memory_info,
                                  &handle_, &allocation_,
                                  &allocation_info);
    
    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create Buffer"};
    }
    
    memory_ = allocation_info.deviceMemory;
    
    if (persistent_) {
        mapped_data_ = static_cast<uint8_t *>(allocation_info.pMappedData);
    }
}

Buffer::Buffer(Buffer &&other) noexcept:
VulkanResource{other.handle_, other.device_},
allocation_{other.allocation_},
memory_{other.memory_},
size_{other.size_},
mapped_data_{other.mapped_data_},
mapped_{other.mapped_} {
    // Reset other handles to avoid releasing on destruction
    other.allocation_ = VK_NULL_HANDLE;
    other.memory_ = VK_NULL_HANDLE;
    other.mapped_data_ = nullptr;
    other.mapped_ = false;
}

Buffer::~Buffer() {
    if (handle_ != VK_NULL_HANDLE && allocation_ != VK_NULL_HANDLE) {
        unmap();
        vmaDestroyBuffer(device_->get_memory_allocator(), handle_, allocation_);
    }
}

const VkBuffer *Buffer::get() const {
    return &handle_;
}

VmaAllocation Buffer::get_allocation() const {
    return allocation_;
}

VkDeviceMemory Buffer::get_memory() const {
    return memory_;
}

VkDeviceSize Buffer::get_size() const {
    return size_;
}

uint8_t *Buffer::map() {
    if (!mapped_ && !mapped_data_) {
        VK_CHECK(vmaMapMemory(device_->get_memory_allocator(), allocation_,
                              reinterpret_cast<void **>(&mapped_data_)));
        mapped_ = true;
    }
    return mapped_data_;
}

void Buffer::unmap() {
    if (mapped_) {
        vmaUnmapMemory(device_->get_memory_allocator(), allocation_);
        mapped_data_ = nullptr;
        mapped_ = false;
    }
}

void Buffer::flush() const {
    vmaFlushAllocation(device_->get_memory_allocator(), allocation_, 0, size_);
}

void Buffer::update(const std::vector<uint8_t> &data, size_t offset) {
    update(data.data(), data.size(), offset);
}

uint64_t Buffer::get_device_address() {
    VkBufferDeviceAddressInfoKHR buffer_device_address_info{};
    buffer_device_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    buffer_device_address_info.buffer = handle_;
    return vkGetBufferDeviceAddressKHR(device_->get_handle(), &buffer_device_address_info);
}

void Buffer::update(void *data, size_t size, size_t offset) {
    update(reinterpret_cast<const uint8_t *>(data), size, offset);
}

void Buffer::update(const uint8_t *data, const size_t size, const size_t offset) {
    if (persistent_) {
        std::copy(data, data + size, mapped_data_ + offset);
        flush();
    } else {
        map();
        std::copy(data, data + size, mapped_data_ + offset);
        flush();
        unmap();
    }
}

}        // namespace vox
