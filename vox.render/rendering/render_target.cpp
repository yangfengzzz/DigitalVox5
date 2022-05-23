//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/render_target.h"

#include "vox.render/core/device.h"

namespace vox {
namespace {
struct CompareExtent2D {
    bool operator()(const VkExtent2D &lhs, const VkExtent2D &rhs) const {
        return !(lhs.width == rhs.width && lhs.height == rhs.height) &&
               (lhs.width < rhs.width && lhs.height < rhs.height);
    }
};
}  // namespace

Attachment::Attachment(VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage)
    : format{format}, samples{samples}, usage{usage} {}

const RenderTarget::CreateFunc RenderTarget::default_create_func_ =
        [](core::Image &&swapchain_image) -> std::unique_ptr<RenderTarget> {
    VkFormat depth_format = GetSuitableDepthFormat(swapchain_image.GetDevice().GetGpu().GetHandle());

    core::Image depth_image{swapchain_image.GetDevice(), swapchain_image.GetExtent(), depth_format,
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                            VMA_MEMORY_USAGE_GPU_ONLY};

    std::vector<core::Image> images;
    images.push_back(std::move(swapchain_image));
    images.push_back(std::move(depth_image));

    return std::make_unique<RenderTarget>(std::move(images));
};

vox::RenderTarget::RenderTarget(std::vector<core::Image> &&images)
    : device_{images.back().GetDevice()}, images_{std::move(images)} {
    assert(!images_.empty() && "Should specify at least 1 image");

    std::set<VkExtent2D, CompareExtent2D> unique_extent;

    // Returns the image extent as a VkExtent2D structure from a VkExtent3D
    auto get_image_extent = [](const core::Image &image) {
        return VkExtent2D{image.GetExtent().width, image.GetExtent().height};
    };

    // Constructs a set of unique image extents given a vector of images
    std::transform(images_.begin(), images_.end(), std::inserter(unique_extent, unique_extent.end()), get_image_extent);

    // Allow only one extent size for a render target
    if (unique_extent.size() != 1) {
        throw VulkanException{VK_ERROR_INITIALIZATION_FAILED, "Extent size is not unique"};
    }

    extent_ = *unique_extent.begin();

    for (auto &image : images_) {
        if (image.GetType() != VK_IMAGE_TYPE_2D) {
            throw VulkanException{VK_ERROR_INITIALIZATION_FAILED, "Image type is not 2D"};
        }

        views_.emplace_back(image, VK_IMAGE_VIEW_TYPE_2D);

        attachments_.emplace_back(Attachment{image.GetFormat(), image.GetSampleCount(), image.GetUsage()});
    }
}

vox::RenderTarget::RenderTarget(std::vector<core::ImageView> &&image_views)
    : device_{const_cast<core::Image &>(image_views.back().GetImage()).GetDevice()},
      images_{},
      views_{std::move(image_views)} {
    assert(!views_.empty() && "Should specify at least 1 image view");

    std::set<VkExtent2D, CompareExtent2D> unique_extent;

    // Returns the extent of the base mip level pointed at by a view
    auto get_view_extent = [](const core::ImageView &view) {
        const VkExtent3D kMip0Extent = view.GetImage().GetExtent();
        const uint32_t kMipLevel = view.GetSubresourceRange().baseMipLevel;
        return VkExtent2D{kMip0Extent.width >> kMipLevel, kMip0Extent.height >> kMipLevel};
    };

    // Constructs a set of unique image extents given a vector of image views;
    // allow only one extent size for a render target
    std::transform(views_.begin(), views_.end(), std::inserter(unique_extent, unique_extent.end()), get_view_extent);
    if (unique_extent.size() != 1) {
        throw VulkanException{VK_ERROR_INITIALIZATION_FAILED, "Extent size is not unique"};
    }
    extent_ = *unique_extent.begin();

    for (auto &view : views_) {
        const auto &image = view.GetImage();
        attachments_.emplace_back(Attachment{image.GetFormat(), image.GetSampleCount(), image.GetUsage()});
    }
}

const VkExtent2D &RenderTarget::GetExtent() const { return extent_; }

const std::vector<core::ImageView> &RenderTarget::GetViews() const { return views_; }

const std::vector<Attachment> &RenderTarget::GetAttachments() const { return attachments_; }

void RenderTarget::SetInputAttachments(std::vector<uint32_t> &input) { input_attachments_ = input; }

const std::vector<uint32_t> &RenderTarget::GetInputAttachments() const { return input_attachments_; }

void RenderTarget::SetOutputAttachments(std::vector<uint32_t> &output) { output_attachments_ = output; }

const std::vector<uint32_t> &RenderTarget::GetOutputAttachments() const { return output_attachments_; }

void RenderTarget::SetLayout(uint32_t attachment, VkImageLayout layout) {
    attachments_[attachment].initial_layout = layout;
}

VkImageLayout RenderTarget::GetLayout(uint32_t attachment) const { return attachments_[attachment].initial_layout; }

}  // namespace vox
