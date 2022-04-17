//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "base_material.h"

namespace vox {
bool BaseMaterial::is_transparent() const {
    return is_transparent_;
}

void BaseMaterial::set_is_transparent(bool new_value) {
    if (new_value == is_transparent_) {
        return;
    }
    is_transparent_ = new_value;
    
    if (new_value) {
        depth_stencil_state_.depth_write_enable = false;
        render_queue_ = RenderQueueType::TRANSPARENT;
    } else {
        depth_stencil_state_.depth_write_enable = true;
        render_queue_ = alpha_cutoff_ > 0 ? RenderQueueType::ALPHA_TEST : RenderQueueType::OPAQUE;
    }
}

float BaseMaterial::alpha_cutoff() const {
    return alpha_cutoff_;
}

void BaseMaterial::set_alpha_cutoff(float new_value) {
    alpha_cutoff_ = new_value;
    shader_data_.set_data(BaseMaterial::alpha_cutoff_prop_, new_value);
    
    if (new_value > 0) {
        shader_data_.add_define("NEED_ALPHA_CUTOFF");
        render_queue_ = is_transparent_ ? RenderQueueType::TRANSPARENT : RenderQueueType::ALPHA_TEST;
    } else {
        shader_data_.add_undefine("NEED_ALPHA_CUTOFF");
        render_queue_ = is_transparent_ ? RenderQueueType::TRANSPARENT : RenderQueueType::OPAQUE;
    }
}

const RenderFace &BaseMaterial::render_face() const {
    return render_face_;
}

void BaseMaterial::set_render_face(const RenderFace &new_value) {
    render_face_ = new_value;
    
    switch (new_value) {
        case RenderFace::FRONT:rasterization_state_.cull_mode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
            break;
        case RenderFace::BACK:rasterization_state_.cull_mode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
            break;
        case RenderFace::DOUBLE:rasterization_state_.cull_mode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
            break;
    }
}

const BlendMode &BaseMaterial::blend_mode() const {
    return blend_mode_;
}

void BaseMaterial::set_blend_mode(const BlendMode &new_value) {
    blend_mode_ = new_value;
    
    switch (new_value) {
        case BlendMode::NORMAL:
            color_blend_state_.attachments[0].src_color_blend_factor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
            color_blend_state_.attachments[0].dst_color_blend_factor =
            VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            color_blend_state_.attachments[0].src_alpha_blend_factor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
            color_blend_state_.attachments[0].dst_alpha_blend_factor =
            VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            color_blend_state_.attachments[0].alpha_blend_op = VkBlendOp::VK_BLEND_OP_ADD;
            color_blend_state_.attachments[0].color_blend_op = VkBlendOp::VK_BLEND_OP_ADD;
            break;
        case BlendMode::ADDITIVE:
            color_blend_state_.attachments[0].src_color_blend_factor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
            color_blend_state_.attachments[0].dst_color_blend_factor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
            color_blend_state_.attachments[0].src_alpha_blend_factor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
            color_blend_state_.attachments[0].dst_alpha_blend_factor =
            VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            color_blend_state_.attachments[0].alpha_blend_op = VkBlendOp::VK_BLEND_OP_ADD;
            color_blend_state_.attachments[0].color_blend_op = VkBlendOp::VK_BLEND_OP_ADD;
            break;
    }
}

BaseMaterial::BaseMaterial(Device &device, const std::string &name) :
Material(device, name),
alpha_cutoff_prop_(ShaderProperty::create("u_alphaCutoff", ShaderDataGroup::MATERIAL)) {
    set_blend_mode(BlendMode::NORMAL);
    shader_data_.set_data(alpha_cutoff_prop_, 0.0f);
}

}
