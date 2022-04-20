//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "base_material.h"
#include "core/device.h"
#include "shader/internal_variant_name.h"

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
        shader_data_.add_define(NEED_ALPHA_CUTOFF);
        render_queue_ = is_transparent_ ? RenderQueueType::TRANSPARENT : RenderQueueType::ALPHA_TEST;
    } else {
        shader_data_.remove_define(NEED_ALPHA_CUTOFF);
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

VkSamplerCreateInfo BaseMaterial::last_sampler_create_info_;

BaseMaterial::BaseMaterial(Device &device, const std::string &name) :
Material(device, name),
alpha_cutoff_prop_("u_alphaCutoff") {
    color_blend_state_.attachments.resize(1);
    set_blend_mode(BlendMode::NORMAL);
    shader_data_.set_data(alpha_cutoff_prop_, 0.0f);
    
    if (!get_sampler_) {
        get_sampler_ = [&](const VkSamplerCreateInfo& info)->core::Sampler* {
            auto iter = sampler_pool_.find(info);
            if (iter != sampler_pool_.end()) {
                return &iter->second;
            } else {
                auto pair = std::make_pair(info, core::Sampler(device, info));
                auto sampler = &pair.second;
                sampler_pool_.insert(std::move(pair));
                return sampler;
            }
        };
    }
    
    if (last_sampler_create_info_.sType != VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO) {
        // Create a default sampler
        last_sampler_create_info_.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        last_sampler_create_info_.magFilter = VK_FILTER_LINEAR;
        last_sampler_create_info_.minFilter = VK_FILTER_LINEAR;
        last_sampler_create_info_.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        last_sampler_create_info_.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        last_sampler_create_info_.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        last_sampler_create_info_.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        last_sampler_create_info_.mipLodBias = 0.0f;
        last_sampler_create_info_.compareOp = VK_COMPARE_OP_NEVER;
        last_sampler_create_info_.minLod = 0.0f;
        // Max level-of-detail should match mip level count
        last_sampler_create_info_.maxLod = 0.0f;
        // Only enable anisotropic filtering if enabled on the device
        // Note that for simplicity, we will always be using max. available anisotropy level for the current device
        // This may have an impact on performance, esp. on lower-specced devices
        // In a real-world scenario the level of anisotropy should be a user setting or e.g. lowered for mobile devices by default
        last_sampler_create_info_.maxAnisotropy = device.get_gpu().get_features().samplerAnisotropy
        ? (device.get_gpu().get_properties().limits.maxSamplerAnisotropy)
        : 1.0f;
        last_sampler_create_info_.anisotropyEnable = device.get_gpu().get_features().samplerAnisotropy;
        last_sampler_create_info_.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    }
}

}
