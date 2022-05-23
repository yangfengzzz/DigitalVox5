//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/subpass.h"

#include <utility>

#include "vox.render/material/material.h"
#include "vox.render/renderer.h"
#include "vox.render/rendering/render_context.h"

namespace vox {

Subpass::Subpass(RenderContext &render_context, Scene *scene, Camera *camera)
    : render_context_{render_context}, scene_{scene}, camera_{camera} {}

void Subpass::UpdateRenderTargetAttachments(RenderTarget &render_target) {
    render_target.SetInputAttachments(input_attachments_);
    render_target.SetOutputAttachments(output_attachments_);
}

RenderContext &Subpass::GetRenderContext() { return render_context_; }

const std::vector<uint32_t> &Subpass::GetInputAttachments() const { return input_attachments_; }

void Subpass::SetInputAttachments(std::vector<uint32_t> input) { input_attachments_ = std::move(input); }

const std::vector<uint32_t> &Subpass::GetOutputAttachments() const { return output_attachments_; }

void Subpass::SetOutputAttachments(std::vector<uint32_t> output) { output_attachments_ = std::move(output); }

const std::vector<uint32_t> &Subpass::GetColorResolveAttachments() const { return color_resolve_attachments_; }

void Subpass::SetColorResolveAttachments(std::vector<uint32_t> color_resolve) {
    color_resolve_attachments_ = std::move(color_resolve);
}

const bool &Subpass::GetDisableDepthStencilAttachment() const { return disable_depth_stencil_attachment_; }

void Subpass::SetDisableDepthStencilAttachment(bool disable_depth_stencil) {
    disable_depth_stencil_attachment_ = disable_depth_stencil;
}

const uint32_t &Subpass::GetDepthStencilResolveAttachment() const { return depth_stencil_resolve_attachment_; }

void Subpass::SetDepthStencilResolveAttachment(uint32_t depth_stencil_resolve) {
    depth_stencil_resolve_attachment_ = depth_stencil_resolve;
}

VkResolveModeFlagBits Subpass::GetDepthStencilResolveMode() const { return depth_stencil_resolve_mode_; }

void Subpass::SetDepthStencilResolveMode(VkResolveModeFlagBits mode) { depth_stencil_resolve_mode_ = mode; }

void Subpass::SetSampleCount(VkSampleCountFlagBits sample_count) { sample_count_ = sample_count; }

const std::string &Subpass::GetDebugName() const { return debug_name_; }

void Subpass::SetDebugName(const std::string &name) { debug_name_ = name; }

bool Subpass::CompareFromNearToFar(const RenderElement &a, const RenderElement &b) {
    return (a.material->render_queue_ < b.material->render_queue_) ||
           (a.renderer->DistanceForSort() < b.renderer->DistanceForSort());
}

bool Subpass::CompareFromFarToNear(const RenderElement &a, const RenderElement &b) {
    return (a.material->render_queue_ < b.material->render_queue_) ||
           (b.renderer->DistanceForSort() < a.renderer->DistanceForSort());
}

PipelineLayout &Subpass::PreparePipelineLayout(CommandBuffer &command_buffer,
                                               const std::vector<ShaderModule *> &shader_modules) {
    // Sets any specified resource modes
    for (auto &shader_module : shader_modules) {
        for (auto &resource_mode : resource_mode_map_) {
            shader_module->SetResourceMode(resource_mode.first, resource_mode.second);
        }
    }

    return command_buffer.GetDevice().GetResourceCache().RequestPipelineLayout(shader_modules);
}

}  // namespace vox
