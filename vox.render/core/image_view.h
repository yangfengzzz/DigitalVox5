//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/image.h"
#include "vox.render/core/vulkan_resource.h"
#include "vox.render/vk_common.h"

namespace vox::core {
class ImageView : public VulkanResource<VkImageView, VK_OBJECT_TYPE_IMAGE_VIEW, const Device> {
public:
    ImageView(Image &image,
              VkImageViewType view_type,
              VkFormat format = VK_FORMAT_UNDEFINED,
              uint32_t base_mip_level = 0,
              uint32_t base_array_layer = 0,
              uint32_t n_mip_levels = 0,
              uint32_t n_array_layers = 0);

    ImageView(ImageView &) = delete;

    ImageView(ImageView &&other) noexcept;

    ~ImageView() override;

    ImageView &operator=(const ImageView &) = delete;

    ImageView &operator=(ImageView &&) = delete;

    [[nodiscard]] const Image &GetImage() const;

    /**
     * @brief Update the image this view is referring to
     *        Used on image move
     */
    void SetImage(Image &image);

    [[nodiscard]] VkFormat GetFormat() const;

    [[nodiscard]] VkImageSubresourceRange GetSubresourceRange() const;

    [[nodiscard]] VkImageSubresourceLayers GetSubresourceLayers() const;

private:
    Image *image_{};

    VkFormat format_{};

    VkImageSubresourceRange subresource_range_{};
};

}  // namespace vox::core
