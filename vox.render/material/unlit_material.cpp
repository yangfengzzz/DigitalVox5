//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unlit_material.h"
#include "shader/internal_variant_name.h"

namespace vox {
const Color &UnlitMaterial::base_color() const {
    return base_color_;
}

void UnlitMaterial::set_base_color(const Color &new_value) {
    base_color_ = new_value;
    shader_data_.set_data(UnlitMaterial::base_color_prop_, new_value);
}

std::shared_ptr<Image> UnlitMaterial::base_texture() const {
    return base_texture_;
}

void UnlitMaterial::set_base_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_base_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void UnlitMaterial::set_base_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    base_texture_ = new_value;
    shader_data_.set_texture(base_texture_prop_, new_value, get_sampler_(info));
    
    if (new_value) {
        shader_data_.add_define(HAS_BASE_TEXTURE);
    } else {
        shader_data_.remove_define(HAS_BASE_TEXTURE);
    }
}

const Vector4F &UnlitMaterial::tiling_offset() const {
    return tiling_offset_;
}

void UnlitMaterial::set_tiling_offset(const Vector4F &new_value) {
    tiling_offset_ = new_value;
    shader_data_.set_data(tiling_offset_prop_, new_value);
}

UnlitMaterial::UnlitMaterial(Device &device, const std::string &name) :
BaseMaterial(device, name),
base_color_prop_("u_baseColor"),
base_texture_prop_("u_baseTexture"),
tiling_offset_prop_("u_tilingOffset") {
    shader_data_.add_define(OMIT_NORMAL);
    shader_data_.add_define(NEED_TILINGOFFSET);
    
    shader_data_.set_data(base_color_prop_, Color(1, 1, 1, 1));
    shader_data_.set_data(tiling_offset_prop_, Vector4F(1, 1, 0, 0));
}

}
