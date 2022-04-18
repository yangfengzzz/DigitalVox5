//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_base_material.h"

namespace vox {
const Color &PbrBaseMaterial::base_color() const {
    return base_color_;
}

void PbrBaseMaterial::set_base_color(const Color &new_value) {
    base_color_ = new_value;
    shader_data_.set_data(PbrBaseMaterial::base_color_prop_, new_value);
}

std::shared_ptr<Image> PbrBaseMaterial::base_texture() const {
    return base_texture_;
}

void PbrBaseMaterial::set_base_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_base_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void PbrBaseMaterial::set_base_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    base_texture_ = new_value;
    shader_data_.set_texture(PbrBaseMaterial::base_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_BASE_COLORMAP");
    } else {
        shader_data_.add_undefine("HAS_BASE_COLORMAP");
    }
}

std::shared_ptr<Image> PbrBaseMaterial::normal_texture() const {
    return normal_texture_;
}

void PbrBaseMaterial::set_normal_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_normal_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void PbrBaseMaterial::set_normal_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    normal_texture_ = new_value;
    shader_data_.set_texture(PbrBaseMaterial::normal_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_NORMAL_TEXTURE");
    } else {
        shader_data_.add_undefine("HAS_NORMAL_TEXTURE");
    }
}

float PbrBaseMaterial::normal_texture_intensity() const {
    return normal_texture_intensity_;
}

void PbrBaseMaterial::set_normal_texture_intensity(float new_value) {
    normal_texture_intensity_ = new_value;
    shader_data_.set_data(PbrBaseMaterial::normal_texture_intensity_prop_, new_value);
}

const Color &PbrBaseMaterial::emissive_color() const {
    return emissive_color_;
}

void PbrBaseMaterial::set_emissive_color(const Color &new_value) {
    emissive_color_ = new_value;
    shader_data_.set_data(PbrBaseMaterial::emissive_color_prop_, new_value);
}

std::shared_ptr<Image> PbrBaseMaterial::emissive_texture() const {
    return emissive_texture_;
}

void PbrBaseMaterial::set_emissive_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_emissive_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void PbrBaseMaterial::set_emissive_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    emissive_texture_ = new_value;
    shader_data_.set_texture(PbrBaseMaterial::emissive_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_EMISSIVEMAP");
    } else {
        shader_data_.add_undefine("HAS_EMISSIVEMAP");
    }
}

std::shared_ptr<Image> PbrBaseMaterial::occlusion_texture() const {
    return occlusion_texture_;
}

void PbrBaseMaterial::set_occlusion_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_occlusion_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void PbrBaseMaterial::set_occlusion_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    occlusion_texture_ = new_value;
    shader_data_.set_texture(PbrBaseMaterial::occlusion_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_OCCLUSIONMAP");
    } else {
        shader_data_.add_undefine("HAS_OCCLUSIONMAP");
    }
}

float PbrBaseMaterial::occlusion_texture_intensity() const {
    return occlusion_texture_intensity_;
}

void PbrBaseMaterial::set_occlusion_texture_intensity(float new_value) {
    occlusion_texture_intensity_ = new_value;
    shader_data_.set_data(PbrBaseMaterial::occlusion_texture_intensity_prop_, new_value);
}

const Vector4F &PbrBaseMaterial::tiling_offset() const {
    return tiling_offset_;
}

void PbrBaseMaterial::set_tiling_offset(const Vector4F &new_value) {
    tiling_offset_ = new_value;
    shader_data_.set_data(PbrBaseMaterial::tiling_offset_prop_, new_value);
}

PbrBaseMaterial::PbrBaseMaterial(Device &device, const std::string &name) :
BaseMaterial(device, name),
tiling_offset_prop_("u_tilingOffset"),
normal_texture_intensity_prop_("u_normalIntensity"),
occlusion_texture_intensity_prop_("u_occlusionStrength"),
base_color_prop_("u_baseColor"),
emissive_color_prop_("u_emissiveColor"),
base_texture_prop_("u_baseColorTexture"),
normal_texture_prop_("u_normalTexture"),
emissive_texture_prop_("u_emissiveTexture"),
occlusion_texture_prop_("u_occlusionTexture") {
    shader_data_.add_define("NEED_WORLDPOS");
    shader_data_.add_define("NEED_TILINGOFFSET");
    
    shader_data_.set_data(PbrBaseMaterial::base_color_prop_, Color(1, 1, 1, 1));
    shader_data_.set_data(PbrBaseMaterial::emissive_color_prop_, Color(0, 0, 0, 1));
    shader_data_.set_data(PbrBaseMaterial::tiling_offset_prop_, Vector4F(1, 1, 0, 0));
    
    shader_data_.set_data(PbrBaseMaterial::normal_texture_intensity_prop_, 1.f);
    shader_data_.set_data(PbrBaseMaterial::occlusion_texture_intensity_prop_, 1.f);
}

}
