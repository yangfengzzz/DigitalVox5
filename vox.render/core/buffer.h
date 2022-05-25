//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/vulkan_resource.h"
#include "vox.render/vk_common.h"

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

    template <typename T>
    static std::vector<T> Copy(std::unordered_map<std::string, vox::core::Buffer> &buffers, const char *buffer_name) {
        auto iter = buffers.find(buffer_name);
        if (iter == buffers.cend()) {
            return {};
        }
        auto &buffer = iter->second;
        std::vector<T> out;

        const size_t kSz = buffer.GetSize();
        out.resize(kSz / sizeof(T));
        const bool kAlreadyMapped = buffer.GetData() != nullptr;
        if (!kAlreadyMapped) {
            buffer.Map();
        }
        memcpy(&out[0], buffer.GetData(), kSz);
        if (!kAlreadyMapped) {
            buffer.Unmap();
        }
        return out;
    }

    [[nodiscard]] const VkBuffer *Get() const;

    [[nodiscard]] VmaAllocation GetAllocation() const;

    [[nodiscard]] VkDeviceMemory GetMemory() const;

    /**
     * @brief Flushes memory if it is HOST_VISIBLE and not HOST_COHERENT
     */
    void Flush() const;

    /**
     * @brief Maps vulkan memory if it isn't already mapped to an host visible address
     * @return Pointer to host visible memory
     */
    uint8_t *Map();

    /**
     * @brief Unmaps vulkan memory from the host visible address
     */
    void Unmap();

    /**
     * @return The size of the buffer
     */
    [[nodiscard]] VkDeviceSize GetSize() const;

    [[nodiscard]] const uint8_t *GetData() const { return mapped_data_; }

    /**
     * @brief Copies byte data into the buffer
     * @param data The data to copy from
     * @param size The amount of bytes to copy
     * @param offset The offset to start the copying into the mapped data
     */
    void Update(const uint8_t *data, size_t size, size_t offset = 0);

    /**
     * @brief Converts any non byte data into bytes and then updates the buffer
     * @param data The data to copy from
     * @param size The amount of bytes to copy
     * @param offset The offset to start the copying into the mapped data
     */
    void Update(void *data, size_t size, size_t offset = 0);

    /**
     * @brief Copies a vector of bytes into the buffer
     * @param data The data vector to upload
     * @param offset The offset to start the copying into the mapped data
     */
    void Update(const std::vector<uint8_t> &data, size_t offset = 0);

    /**
     * @brief Copies an object as byte data into the buffer
     * @param object The object to convert into byte data
     * @param offset The offset to start the copying into the mapped data
     */
    template <class T>
    void ConvertAndUpdate(const T &object, size_t offset = 0) {
        Update(reinterpret_cast<const uint8_t *>(&object), sizeof(T), offset);
    }

    /**
     * @return Return the buffer's device address (note: requires that the buffer has been created
     * with the VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT usage fla)
     */
    uint64_t GetDeviceAddress();

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

}  // namespace core
}  // namespace vox
