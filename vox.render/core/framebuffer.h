//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/render_pass.h"
#include "vox.render/rendering/render_target.h"
#include "vox.render/vk_common.h"

namespace vox {
class Device;

class Framebuffer {
public:
    Framebuffer(Device &device, const RenderTarget &render_target, const RenderPass &render_pass);

    Framebuffer(const Framebuffer &) = delete;

    Framebuffer(Framebuffer &&other) noexcept;

    ~Framebuffer();

    Framebuffer &operator=(const Framebuffer &) = delete;

    Framebuffer &operator=(Framebuffer &&) = delete;

    [[nodiscard]] VkFramebuffer GetHandle() const;

    [[nodiscard]] const VkExtent2D &GetExtent() const;

private:
    Device &device_;

    VkFramebuffer handle_{VK_NULL_HANDLE};

    VkExtent2D extent_{};
};

}  // namespace vox
