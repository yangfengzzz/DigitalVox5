//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "subpass.h"

#include <utility>

#include "render_context.h"

namespace vox {

Subpass::Subpass(RenderContext &render_context, ShaderSource &&vertex_source, ShaderSource &&fragment_source) :
render_context_{render_context},
vertex_shader_{std::move(vertex_source)},
fragment_shader_{std::move(fragment_source)} {
}

void Subpass::update_render_target_attachments(RenderTarget &render_target) {
    render_target.set_input_attachments(input_attachments_);
    render_target.set_output_attachments(output_attachments_);
}

RenderContext &Subpass::get_render_context() {
    return render_context_;
}

const ShaderSource &Subpass::get_vertex_shader() const {
    return vertex_shader_;
}

const ShaderSource &Subpass::get_fragment_shader() const {
    return fragment_shader_;
}

DepthStencilState &Subpass::get_depth_stencil_state() {
    return depth_stencil_state_;
}

const std::vector<uint32_t> &Subpass::get_input_attachments() const {
    return input_attachments_;
}

void Subpass::set_input_attachments(std::vector<uint32_t> input) {
    input_attachments_ = std::move(input);
}

const std::vector<uint32_t> &Subpass::get_output_attachments() const {
    return output_attachments_;
}

void Subpass::set_output_attachments(std::vector<uint32_t> output) {
    output_attachments_ = std::move(output);
}

const std::vector<uint32_t> &Subpass::get_color_resolve_attachments() const {
    return color_resolve_attachments_;
}

void Subpass::set_color_resolve_attachments(std::vector<uint32_t> color_resolve) {
    color_resolve_attachments_ = std::move(color_resolve);
}

const bool &Subpass::get_disable_depth_stencil_attachment() const {
    return disable_depth_stencil_attachment_;
}

void Subpass::set_disable_depth_stencil_attachment(bool disable_depth_stencil) {
    disable_depth_stencil_attachment_ = disable_depth_stencil;
}

const uint32_t &Subpass::get_depth_stencil_resolve_attachment() const {
    return depth_stencil_resolve_attachment_;
}

void Subpass::set_depth_stencil_resolve_attachment(uint32_t depth_stencil_resolve) {
    depth_stencil_resolve_attachment_ = depth_stencil_resolve;
}

VkResolveModeFlagBits Subpass::get_depth_stencil_resolve_mode() const {
    return depth_stencil_resolve_mode_;
}

void Subpass::set_depth_stencil_resolve_mode(VkResolveModeFlagBits mode) {
    depth_stencil_resolve_mode_ = mode;
}

void Subpass::set_sample_count(VkSampleCountFlagBits sample_count) {
    sample_count_ = sample_count;
}

const std::string &Subpass::get_debug_name() const {
    return debug_name_;
}

void Subpass::set_debug_name(const std::string &name) {
    debug_name_ = name;
}

}        // namespace vox
