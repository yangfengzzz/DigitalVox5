//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_set>

#include "vox.base/helper.h"
#include "vox.render/core/vulkan_resource.h"
#include "vox.render/vk_common.h"

namespace vox {
class Device;

namespace core {
class ImageView;

class Image : public VulkanResource<VkImage, VK_OBJECT_TYPE_IMAGE, const Device> {
public:
    Image(Device const &device,
          VkImage handle,
          const VkExtent3D &extent,
          VkFormat format,
          VkImageUsageFlags image_usage,
          VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT);

    Image(Device const &device,
          const VkExtent3D &extent,
          VkFormat format,
          VkImageUsageFlags image_usage,
          VmaMemoryUsage memory_usage,
          VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT,
          uint32_t mip_levels = 1,
          uint32_t array_layers = 1,
          VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
          VkImageCreateFlags flags = 0,
          uint32_t num_queue_families = 0,
          const uint32_t *queue_families = nullptr);

    Image(const Image &) = delete;

    Image(Image &&other) noexcept;

    ~Image() override;

    Image &operator=(const Image &) = delete;

    Image &operator=(Image &&) = delete;

    [[nodiscard]] VmaAllocation GetMemory() const;

    /**
     * @brief Maps vulkan memory to an host visible address
     * @return Pointer to host visible memory
     */
    uint8_t *Map();

    /**
     * @brief Unmaps vulkan memory from the host visible address
     */
    void Unmap();

    [[nodiscard]] VkImageType GetType() const;

    [[nodiscard]] const VkExtent3D &GetExtent() const;

    [[nodiscard]] VkFormat GetFormat() const;

    [[nodiscard]] VkSampleCountFlagBits GetSampleCount() const;

    [[nodiscard]] VkImageUsageFlags GetUsage() const;

    [[nodiscard]] VkImageTiling GetTiling() const;

    [[nodiscard]] VkImageSubresource GetSubresource() const;

    [[nodiscard]] uint32_t GetArrayLayerCount() const;

    std::unordered_set<ImageView *> &GetViews();

private:
    VmaAllocation memory_{VK_NULL_HANDLE};

    VkImageType type_{};

    VkExtent3D extent_{};

    VkFormat format_{};

    VkImageUsageFlags usage_{};

    VkSampleCountFlagBits sample_count_{};

    VkImageTiling tiling_{};

    VkImageSubresource subresource_{};

    uint32_t array_layer_count_{0};

    /// Image views referring to this image
    std::unordered_set<ImageView *> views_;

    uint8_t *mapped_data_{nullptr};

    /// Whether it was mapped with vmaMapMemory
    bool mapped_{false};
};

}  // namespace core
}  // namespace vox
