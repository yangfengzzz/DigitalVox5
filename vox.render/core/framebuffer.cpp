//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/framebuffer.h"

#include "vox.render/core/device.h"

namespace vox {
VkFramebuffer Framebuffer::GetHandle() const { return handle_; }

const VkExtent2D &Framebuffer::GetExtent() const { return extent_; }

Framebuffer::Framebuffer(Device &device, const RenderTarget &render_target, const RenderPass &render_pass)
    : device_{device}, extent_{render_target.GetExtent()} {
    std::vector<VkImageView> attachments;

    for (auto &view : render_target.GetViews()) {
        attachments.emplace_back(view.GetHandle());
    }

    VkFramebufferCreateInfo create_info{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};

    create_info.renderPass = render_pass.GetHandle();
    create_info.attachmentCount = utility::ToU32(attachments.size());
    create_info.pAttachments = attachments.data();
    create_info.width = extent_.width;
    create_info.height = extent_.height;
    create_info.layers = 1;

    auto result = vkCreateFramebuffer(device.GetHandle(), &create_info, nullptr, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create Framebuffer"};
    }
}

Framebuffer::Framebuffer(Framebuffer &&other) noexcept
    : device_{other.device_}, handle_{other.handle_}, extent_{other.extent_} {
    other.handle_ = VK_NULL_HANDLE;
}

Framebuffer::~Framebuffer() {
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(device_.GetHandle(), handle_, nullptr);
    }
}

}  // namespace vox
