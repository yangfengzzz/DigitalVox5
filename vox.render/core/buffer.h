//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "helpers.h"
#include "vk_common.h"
#include "core/vulkan_resource.h"

namespace vox {
class Device;

namespace core {
class Buffer : public VulkanResource<VkBuffer, VK_OBJECT_TYPE_BUFFER, const Device> {
public:
    /**
     * @brief Creates a buffer using VMA
     * @param device A valid Vulkan device
     * @param size The size in bytes of the buffer
     * @param buffer_usage The usage flags for the VkBuffer
     * @param memory_usage The memory usage of the buffer
     * @param flags The allocation create flags
     * @param queue_family_indices optional queue family indices
     */
    Buffer(Device const &device,
           VkDeviceSize size,
           VkBufferUsageFlags buffer_usage,
           VmaMemoryUsage memory_usage,
           VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
           const std::vector<uint32_t> &queue_family_indices = {});
    
    Buffer(const Buffer &) = delete;
    
    Buffer(Buffer &&other) noexcept;
    
    ~Buffer() override;
    
    Buffer &operator=(const Buffer &) = delete;
    
    Buffer &operator=(Buffer &&) = delete;
    
    template<typename T>
    static std::vector<T>
    copy(std::unordered_map<std::string, vox::core::Buffer> &buffers, const char *buffer_name) {
        auto iter = buffers.find(buffer_name);
        if (iter == buffers.cend()) {
            return {};
        }
        auto &buffer = iter->second;
        std::vector<T> out;
        
        const size_t kSz = buffer.get_size();
        out.resize(kSz / sizeof(T));
        const bool kAlreadyMapped = buffer.get_data() != nullptr;
        if (!kAlreadyMapped) {
            buffer.map();
        }
        memcpy(&out[0], buffer.get_data(), kSz);
        if (!kAlreadyMapped) {
            buffer.unmap();
        }
        return out;
    }
    
    [[nodiscard]] const VkBuffer *get() const;
    
    [[nodiscard]] VmaAllocation get_allocation() const;
    
    [[nodiscard]] VkDeviceMemory get_memory() const;
    
    /**
     * @brief Flushes memory if it is HOST_VISIBLE and not HOST_COHERENT
     */
    void flush() const;
    
    /**
     * @brief Maps vulkan memory if it isn't already mapped to an host visible address
     * @return Pointer to host visible memory
     */
    uint8_t *map();
    
    /**
     * @brief Unmaps vulkan memory from the host visible address
     */
    void unmap();
    
    /**
     * @return The size of the buffer
     */
    [[nodiscard]] VkDeviceSize get_size() const;
    
    [[nodiscard]] const uint8_t *get_data() const {
        return mapped_data_;
    }
    
    /**
     * @brief Copies byte data into the buffer
     * @param data The data to copy from
     * @param size The amount of bytes to copy
     * @param offset The offset to start the copying into the mapped data
     */
    void update(const uint8_t *data, size_t size, size_t offset = 0);
    
    /**
     * @brief Converts any non byte data into bytes and then updates the buffer
     * @param data The data to copy from
     * @param size The amount of bytes to copy
     * @param offset The offset to start the copying into the mapped data
     */
    void update(void *data, size_t size, size_t offset = 0);
    
    /**
     * @brief Copies a vector of bytes into the buffer
     * @param data The data vector to upload
     * @param offset The offset to start the copying into the mapped data
     */
    void update(const std::vector<uint8_t> &data, size_t offset = 0);
    
    /**
     * @brief Copies an object as byte data into the buffer
     * @param object The object to convert into byte data
     * @param offset The offset to start the copying into the mapped data
     */
    template<class T>
    void convert_and_update(const T &object, size_t offset = 0) {
        update(reinterpret_cast<const uint8_t *>(&object), sizeof(T), offset);
    }
    
    /**
     * @return Return the buffer's device address (note: requires that the buffer has been created
     * with the VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT usage fla)
     */
    uint64_t get_device_address();
    
private:
    VmaAllocation allocation_{VK_NULL_HANDLE};
    
    VkDeviceMemory memory_{VK_NULL_HANDLE};
    
    VkDeviceSize size_{0};
    
    uint8_t *mapped_data_{nullptr};
    
    /// Whether the buffer is persistently mapped or not
    bool persistent_{false};
    
    /// Whether the buffer has been mapped with vmaMapMemory
    bool mapped_{false};
};

}        // namespace core
}        // namespace vox
