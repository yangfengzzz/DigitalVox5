//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/image.h"
#include "vox.render/core/image_view.h"
#include "vox.render/vk_common.h"

namespace vox {
class Device;

/**
 * @brief Description of render pass attachments.
 * Attachment descriptions can be used to automatically create render target images.
 */
struct Attachment {
    VkFormat format{VK_FORMAT_UNDEFINED};

    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};

    VkImageUsageFlags usage{VK_IMAGE_USAGE_SAMPLED_BIT};

    VkImageLayout initial_layout{VK_IMAGE_LAYOUT_UNDEFINED};

    Attachment() = default;

    Attachment(VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage);
};

/**
 * @brief RenderTarget contains three vectors for: core::Image, core::ImageView and Attachment.
 * The first two are Vulkan images and corresponding image views respectively.
 * Attachment (s) contain a description of the images, which has two main purposes:
 * - RenderPass creation only needs a list of Attachment (s), not the actual images, so we keep
 *   the minimum amount of information necessary
 * - Creation of a RenderTarget becomes simpler, because the caller can just ask for some
 *   Attachment (s) without having to create the images
 */
class RenderTarget {
public:
    using CreateFunc = std::function<std::unique_ptr<RenderTarget>(core::Image &&)>;

    static const CreateFunc default_create_func_;

    explicit RenderTarget(std::vector<core::Image> &&images);

    explicit RenderTarget(std::vector<core::ImageView> &&image_views);

    RenderTarget(const RenderTarget &) = delete;

    RenderTarget(RenderTarget &&) = delete;

    RenderTarget &operator=(const RenderTarget &other) noexcept = delete;

    RenderTarget &operator=(RenderTarget &&other) noexcept = delete;

    [[nodiscard]] const VkExtent2D &GetExtent() const;

    [[nodiscard]] const std::vector<core::ImageView> &GetViews() const;

    [[nodiscard]] const std::vector<Attachment> &GetAttachments() const;

    /**
     * @brief Sets the current input attachments overwriting the current ones
     *        Should be set before beginning the render pass and before starting a new subpass
     * @param input Set of attachment reference number to use as input
     */
    void SetInputAttachments(std::vector<uint32_t> &input);

    [[nodiscard]] const std::vector<uint32_t> &GetInputAttachments() const;

    /**
     * @brief Sets the current output attachments overwriting the current ones
     *        Should be set before beginning the render pass and before starting a new subpass
     * @param output Set of attachment reference number to use as output
     */
    void SetOutputAttachments(std::vector<uint32_t> &output);

    [[nodiscard]] const std::vector<uint32_t> &GetOutputAttachments() const;

    void SetLayout(uint32_t attachment, VkImageLayout layout);

    [[nodiscard]] VkImageLayout GetLayout(uint32_t attachment) const;

private:
    Device const &device_;

    VkExtent2D extent_{};

    std::vector<core::Image> images_;

    std::vector<core::ImageView> views_;

    std::vector<Attachment> attachments_;

    /// By default there are no input attachments
    std::vector<uint32_t> input_attachments_ = {};

    /// By default the output attachments is attachment 0
    std::vector<uint32_t> output_attachments_ = {0};
};

}  // namespace vox
