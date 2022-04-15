//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image_view.h"

#include "core/image.h"
#include "device.h"

namespace vox::core {
ImageView::ImageView(Image &img, VkImageViewType view_type, VkFormat format,
                     uint32_t mip_level, uint32_t array_layer,
                     uint32_t n_mip_levels, uint32_t n_array_layers) :
VulkanResource{VK_NULL_HANDLE, &img.get_device()},
image_{&img},
format_{format} {
    if (format == VK_FORMAT_UNDEFINED) {
        format_ = format = image_->get_format();
    }
    
    subresource_range_.baseMipLevel = mip_level;
    subresource_range_.baseArrayLayer = array_layer;
    subresource_range_.levelCount = n_mip_levels == 0 ? image_->get_subresource().mipLevel : n_mip_levels;
    subresource_range_.layerCount = n_array_layers == 0 ? image_->get_subresource().arrayLayer : n_array_layers;
    
    if (is_depth_stencil_format(format)) {
        subresource_range_.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    } else {
        subresource_range_.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    
    VkImageViewCreateInfo view_info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_info.image = image_->get_handle();
    view_info.viewType = view_type;
    view_info.format = format;
    view_info.subresourceRange = subresource_range_;
    
    auto result = vkCreateImageView(device_->get_handle(), &view_info, nullptr, &handle_);
    
    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create ImageView"};
    }
    
    // Register this image view to its image
    // in order to be notified when it gets moved
    image_->get_views().emplace(this);
}

ImageView::ImageView(ImageView &&other) noexcept:
VulkanResource{std::move(other)},
image_{other.image_},
format_{other.format_},
subresource_range_{other.subresource_range_} {
    // Remove old view from image set and add this new one
    auto &views = image_->get_views();
    views.erase(&other);
    views.emplace(this);
    
    other.handle_ = VK_NULL_HANDLE;
}

ImageView::~ImageView() {
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyImageView(device_->get_handle(), handle_, nullptr);
    }
}

const Image &ImageView::get_image() const {
    assert(image_ && "Image view is referring an invalid image");
    return *image_;
}

void ImageView::set_image(Image &img) {
    image_ = &img;
}

VkFormat ImageView::get_format() const {
    return format_;
}

VkImageSubresourceRange ImageView::get_subresource_range() const {
    return subresource_range_;
}

VkImageSubresourceLayers ImageView::get_subresource_layers() const {
    VkImageSubresourceLayers subresource{};
    subresource.aspectMask = subresource_range_.aspectMask;
    subresource.baseArrayLayer = subresource_range_.baseArrayLayer;
    subresource.layerCount = subresource_range_.layerCount;
    subresource.mipLevel = subresource_range_.baseMipLevel;
    return subresource;
}

}        // namespace vox
