//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_set>

#include "helpers.h"
#include "vk_common.h"
#include "core/vulkan_resource.h"

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
    
    [[nodiscard]] VmaAllocation get_memory() const;
    
    /**
     * @brief Maps vulkan memory to an host visible address
     * @return Pointer to host visible memory
     */
    uint8_t *map();
    
    /**
     * @brief Unmaps vulkan memory from the host visible address
     */
    void unmap();
    
    [[nodiscard]] VkImageType get_type() const;
    
    [[nodiscard]] const VkExtent3D &get_extent() const;
    
    [[nodiscard]] VkFormat get_format() const;
    
    [[nodiscard]] VkSampleCountFlagBits get_sample_count() const;
    
    [[nodiscard]] VkImageUsageFlags get_usage() const;
    
    [[nodiscard]] VkImageTiling get_tiling() const;
    
    [[nodiscard]] VkImageSubresource get_subresource() const;
    
    [[nodiscard]] uint32_t get_array_layer_count() const;
    
    std::unordered_set<ImageView *> &get_views();
    
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

}        // namespace core
}        // namespace vox
