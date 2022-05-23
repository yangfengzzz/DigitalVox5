//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/pipeline_state.h"

bool operator==(const VkVertexInputAttributeDescription &lhs, const VkVertexInputAttributeDescription &rhs) {
    return std::tie(lhs.binding, lhs.format, lhs.location, lhs.offset) ==
           std::tie(rhs.binding, rhs.format, rhs.location, rhs.offset);
}

bool operator==(const VkVertexInputBindingDescription &lhs, const VkVertexInputBindingDescription &rhs) {
    return std::tie(lhs.binding, lhs.inputRate, lhs.stride) == std::tie(rhs.binding, rhs.inputRate, rhs.stride);
}

bool operator==(const vox::ColorBlendAttachmentState &lhs, const vox::ColorBlendAttachmentState &rhs) {
    return std::tie(lhs.alpha_blend_op, lhs.blend_enable, lhs.color_blend_op, lhs.color_write_mask,
                    lhs.dst_alpha_blend_factor, lhs.dst_color_blend_factor, lhs.src_alpha_blend_factor,
                    lhs.src_color_blend_factor) == std::tie(rhs.alpha_blend_op, rhs.blend_enable, rhs.color_blend_op,
                                                            rhs.color_write_mask, rhs.dst_alpha_blend_factor,
                                                            rhs.dst_color_blend_factor, rhs.src_alpha_blend_factor,
                                                            rhs.src_color_blend_factor);
}

bool operator!=(const vox::StencilOpState &lhs, const vox::StencilOpState &rhs) {
    return std::tie(lhs.compare_op, lhs.depth_fail_op, lhs.fail_op, lhs.pass_op) !=
           std::tie(rhs.compare_op, rhs.depth_fail_op, rhs.fail_op, rhs.pass_op);
}

bool operator!=(const vox::VertexInputState &lhs, const vox::VertexInputState &rhs) {
    return lhs.attributes != rhs.attributes || lhs.bindings != rhs.bindings;
}

bool operator!=(const vox::InputAssemblyState &lhs, const vox::InputAssemblyState &rhs) {
    return std::tie(lhs.primitive_restart_enable, lhs.topology) != std::tie(rhs.primitive_restart_enable, rhs.topology);
}

bool operator!=(const vox::RasterizationState &lhs, const vox::RasterizationState &rhs) {
    return std::tie(lhs.cull_mode, lhs.depth_bias_enable, lhs.depth_clamp_enable, lhs.front_face, lhs.front_face,
                    lhs.polygon_mode, lhs.rasterizer_discard_enable) !=
           std::tie(rhs.cull_mode, rhs.depth_bias_enable, rhs.depth_clamp_enable, rhs.front_face, rhs.front_face,
                    rhs.polygon_mode, rhs.rasterizer_discard_enable);
}

bool operator!=(const vox::ViewportState &lhs, const vox::ViewportState &rhs) {
    return lhs.viewport_count != rhs.viewport_count || lhs.scissor_count != rhs.scissor_count;
}

bool operator!=(const vox::MultisampleState &lhs, const vox::MultisampleState &rhs) {
    return std::tie(lhs.alpha_to_coverage_enable, lhs.alpha_to_one_enable, lhs.min_sample_shading,
                    lhs.rasterization_samples, lhs.sample_mask, lhs.sample_shading_enable) !=
           std::tie(rhs.alpha_to_coverage_enable, rhs.alpha_to_one_enable, rhs.min_sample_shading,
                    rhs.rasterization_samples, rhs.sample_mask, rhs.sample_shading_enable);
}

bool operator!=(const vox::DepthStencilState &lhs, const vox::DepthStencilState &rhs) {
    return std::tie(lhs.depth_bounds_test_enable, lhs.depth_compare_op, lhs.depth_test_enable, lhs.depth_write_enable,
                    lhs.stencil_test_enable) != std::tie(rhs.depth_bounds_test_enable, rhs.depth_compare_op,
                                                         rhs.depth_test_enable, rhs.depth_write_enable,
                                                         rhs.stencil_test_enable) ||
           lhs.back != rhs.back || lhs.front != rhs.front;
}

bool operator!=(const vox::ColorBlendState &lhs, const vox::ColorBlendState &rhs) {
    return std::tie(lhs.logic_op, lhs.logic_op_enable) != std::tie(rhs.logic_op, rhs.logic_op_enable) ||
           lhs.attachments.size() != rhs.attachments.size() ||
           !std::equal(lhs.attachments.begin(), lhs.attachments.end(), rhs.attachments.begin(),
                       [](const vox::ColorBlendAttachmentState &lhs, const vox::ColorBlendAttachmentState &rhs) {
                           return lhs == rhs;
                       });
}

namespace vox {
void SpecializationConstantState::Reset() {
    if (dirty_) {
        specialization_constant_state_.clear();
    }

    dirty_ = false;
}

bool SpecializationConstantState::IsDirty() const { return dirty_; }

void SpecializationConstantState::ClearDirty() { dirty_ = false; }

void SpecializationConstantState::SetConstant(uint32_t constant_id, const std::vector<uint8_t> &value) {
    auto data = specialization_constant_state_.find(constant_id);

    if (data != specialization_constant_state_.end() && data->second == value) {
        return;
    }

    dirty_ = true;

    specialization_constant_state_[constant_id] = value;
}

void SpecializationConstantState::SetSpecializationConstantState(
        const std::map<uint32_t, std::vector<uint8_t>> &state) {
    specialization_constant_state_ = state;
}

const std::map<uint32_t, std::vector<uint8_t>> &SpecializationConstantState::GetSpecializationConstantState() const {
    return specialization_constant_state_;
}

void PipelineState::Reset() {
    ClearDirty();

    pipeline_layout_ = nullptr;

    render_pass_ = nullptr;

    specialization_constant_state_.Reset();

    vertex_input_state_ = {};

    input_assembly_state_ = {};

    rasterization_state_ = {};

    multisample_state_ = {};

    depth_stencil_state_ = {};

    color_blend_state_ = {};

    subpass_index_ = {0U};
}

void PipelineState::SetPipelineLayout(PipelineLayout &pipeline_layout) {
    if (pipeline_layout_) {
        if (pipeline_layout_->GetHandle() != pipeline_layout.GetHandle()) {
            pipeline_layout_ = &pipeline_layout;

            dirty_ = true;
        }
    } else {
        pipeline_layout_ = &pipeline_layout;

        dirty_ = true;
    }
}

void PipelineState::SetRenderPass(const RenderPass &render_pass) {
    if (render_pass_) {
        if (render_pass_->GetHandle() != render_pass.GetHandle()) {
            render_pass_ = &render_pass;

            dirty_ = true;
        }
    } else {
        render_pass_ = &render_pass;

        dirty_ = true;
    }
}

void PipelineState::SetSpecializationConstant(uint32_t constant_id, const std::vector<uint8_t> &data) {
    specialization_constant_state_.SetConstant(constant_id, data);

    if (specialization_constant_state_.IsDirty()) {
        dirty_ = true;
    }
}

void PipelineState::SetVertexInputState(const VertexInputState &vertex_input_state) {
    if (vertex_input_state_ != vertex_input_state) {
        vertex_input_state_ = vertex_input_state;

        dirty_ = true;
    }
}

void PipelineState::SetInputAssemblyState(const InputAssemblyState &input_assembly_state) {
    if (input_assembly_state_ != input_assembly_state) {
        input_assembly_state_ = input_assembly_state;

        dirty_ = true;
    }
}

void PipelineState::SetRasterizationState(const RasterizationState &rasterization_state) {
    if (rasterization_state_ != rasterization_state) {
        rasterization_state_ = rasterization_state;

        dirty_ = true;
    }
}

void PipelineState::SetViewportState(const ViewportState &viewport_state) {
    if (viewport_state_ != viewport_state) {
        viewport_state_ = viewport_state;

        dirty_ = true;
    }
}

void PipelineState::SetMultisampleState(const MultisampleState &multisample_state) {
    if (multisample_state_ != multisample_state) {
        multisample_state_ = multisample_state;

        dirty_ = true;
    }
}

void PipelineState::SetDepthStencilState(const DepthStencilState &depth_stencil_state) {
    if (depth_stencil_state_ != depth_stencil_state) {
        depth_stencil_state_ = depth_stencil_state;

        dirty_ = true;
    }
}

void PipelineState::SetColorBlendState(const ColorBlendState &color_blend_state) {
    if (color_blend_state_ != color_blend_state) {
        color_blend_state_ = color_blend_state;

        dirty_ = true;
    }
}

void PipelineState::SetSubpassIndex(uint32_t subpass_index) {
    if (subpass_index_ != subpass_index) {
        subpass_index_ = subpass_index;

        dirty_ = true;
    }
}

const PipelineLayout &PipelineState::GetPipelineLayout() const {
    assert(pipeline_layout_ && "Graphics state Pipeline layout is not set");
    return *pipeline_layout_;
}

const RenderPass *PipelineState::GetRenderPass() const { return render_pass_; }

const SpecializationConstantState &PipelineState::GetSpecializationConstantState() const {
    return specialization_constant_state_;
}

const VertexInputState &PipelineState::GetVertexInputState() const { return vertex_input_state_; }

const InputAssemblyState &PipelineState::GetInputAssemblyState() const { return input_assembly_state_; }

const RasterizationState &PipelineState::GetRasterizationState() const { return rasterization_state_; }

const ViewportState &PipelineState::GetViewportState() const { return viewport_state_; }

const MultisampleState &PipelineState::GetMultisampleState() const { return multisample_state_; }

const DepthStencilState &PipelineState::GetDepthStencilState() const { return depth_stencil_state_; }

const ColorBlendState &PipelineState::GetColorBlendState() const { return color_blend_state_; }

uint32_t PipelineState::GetSubpassIndex() const { return subpass_index_; }

bool PipelineState::IsDirty() const { return dirty_ || specialization_constant_state_.IsDirty(); }

void PipelineState::ClearDirty() {
    dirty_ = false;
    specialization_constant_state_.ClearDirty();
}

}  // namespace vox
