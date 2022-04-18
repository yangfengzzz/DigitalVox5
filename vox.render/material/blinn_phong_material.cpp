//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "blinn_phong_material.h"

namespace vox {
const Color &BlinnPhongMaterial::base_color() const {
    return base_color_;
}

void BlinnPhongMaterial::set_base_color(const Color &new_value) {
    base_color_ = new_value;
    shader_data_.set_data(BlinnPhongMaterial::diffuse_color_prop_, new_value);
}

std::shared_ptr<Image> BlinnPhongMaterial::base_texture() const {
    return base_texture_;
}

void BlinnPhongMaterial::set_base_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_base_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void BlinnPhongMaterial::set_base_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    base_texture_ = new_value;
    shader_data_.set_texture(BlinnPhongMaterial::base_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_DIFFUSE_TEXTURE");
    } else {
        shader_data_.add_undefine("HAS_DIFFUSE_TEXTURE");
    }
}

const Color &BlinnPhongMaterial::specular_color() const {
    return specular_color_;
}

void BlinnPhongMaterial::set_specular_color(const Color &new_value) {
    specular_color_ = new_value;
    shader_data_.set_data(BlinnPhongMaterial::specular_color_prop_, new_value);
}

std::shared_ptr<Image> BlinnPhongMaterial::specular_texture() const {
    return specular_texture_;
}

void BlinnPhongMaterial::set_specular_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_specular_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void BlinnPhongMaterial::set_specular_texture(const std::shared_ptr<Image> &new_value,
                                              const VkSamplerCreateInfo &info) {
    specular_texture_ = new_value;
    shader_data_.set_texture(BlinnPhongMaterial::specular_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_SPECULAR_TEXTURE");
    } else {
        shader_data_.add_undefine("HAS_SPECULAR_TEXTURE");
    }
}

const Color &BlinnPhongMaterial::emissive_color() const {
    return emissive_color_;
}

void BlinnPhongMaterial::set_emissive_color(const Color &new_value) {
    emissive_color_ = new_value;
    shader_data_.set_data(BlinnPhongMaterial::emissive_color_prop_, new_value);
}

std::shared_ptr<Image> BlinnPhongMaterial::emissive_texture() const {
    return emissive_texture_;
}

void BlinnPhongMaterial::BlinnPhongMaterial::set_emissive_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_emissive_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void BlinnPhongMaterial::set_emissive_texture(const std::shared_ptr<Image> &new_value,
                                              const VkSamplerCreateInfo &info) {
    emissive_texture_ = new_value;
    shader_data_.set_texture(BlinnPhongMaterial::emissive_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_EMISSIVE_TEXTURE");
    } else {
        shader_data_.add_undefine("HAS_EMISSIVE_TEXTURE");
    }
}

std::shared_ptr<Image> BlinnPhongMaterial::normal_texture() const {
    return normal_texture_;
}

void BlinnPhongMaterial::set_normal_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_normal_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void BlinnPhongMaterial::set_normal_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    normal_texture_ = new_value;
    shader_data_.set_texture(BlinnPhongMaterial::normal_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_NORMAL_TEXTURE");
    } else {
        shader_data_.add_undefine("HAS_NORMAL_TEXTURE");
    }
}

float BlinnPhongMaterial::normal_intensity() const {
    return normal_intensity_;
}

void BlinnPhongMaterial::set_normal_intensity(float new_value) {
    normal_intensity_ = new_value;
    shader_data_.set_data(BlinnPhongMaterial::normal_intensity_prop_, new_value);
}

float BlinnPhongMaterial::shininess() const {
    return shininess_;
}

void BlinnPhongMaterial::set_shininess(float new_value) {
    shininess_ = new_value;
    shader_data_.set_data(BlinnPhongMaterial::shininess_prop_, new_value);
}

const Vector4F &BlinnPhongMaterial::tiling_offset() const {
    return tiling_offset_;
}

void BlinnPhongMaterial::set_tiling_offset(const Vector4F &new_value) {
    tiling_offset_ = new_value;
    shader_data_.set_data(BlinnPhongMaterial::tiling_offset_prop_, new_value);
}

BlinnPhongMaterial::BlinnPhongMaterial(Device &device, const std::string &name) :
BaseMaterial(device, name),
diffuse_color_prop_("u_diffuseColor"),
specular_color_prop_("u_specularColor"),
emissive_color_prop_("u_emissiveColor"),
tiling_offset_prop_("u_tilingOffset"),
shininess_prop_("u_shininess"),
normal_intensity_prop_("u_normalIntensity"),
base_texture_prop_("u_diffuseTexture"),
specular_texture_prop_("u_specularTexture"),
emissive_texture_prop_("u_emissiveTexture"),
normal_texture_prop_("u_normalTexture") {
    shader_data_.add_define("NEED_WORLDPOS");
    shader_data_.add_define("NEED_TILINGOFFSET");
    
    shader_data_.set_data(diffuse_color_prop_, Color(1, 1, 1, 1));
    shader_data_.set_data(specular_color_prop_, Color(1, 1, 1, 1));
    shader_data_.set_data(emissive_color_prop_, Color(0, 0, 0, 1));
    shader_data_.set_data(tiling_offset_prop_, Vector4F(1, 1, 0, 0));
    shader_data_.set_data(shininess_prop_, 16.f);
    shader_data_.set_data(normal_intensity_prop_, 1.f);
}

}
