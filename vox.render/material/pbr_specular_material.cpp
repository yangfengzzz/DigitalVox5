//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#include "pbr_specular_material.h"

namespace vox {
const Color &PbrSpecularMaterial::specular_color() const {
    return specular_color_;
}

void PbrSpecularMaterial::set_specular_color(const Color &new_value) {
    specular_color_ = new_value;
    shader_data_.set_data(PbrSpecularMaterial::specular_color_prop_, new_value);
}

float PbrSpecularMaterial::glossiness() const {
    return glossiness_;
}

void PbrSpecularMaterial::set_glossiness(float new_value) {
    glossiness_ = new_value;
    shader_data_.set_data(PbrSpecularMaterial::glossiness_prop_, new_value);
}

std::shared_ptr<Image> PbrSpecularMaterial::specular_glossiness_texture() const {
    return specular_glossiness_texture_;
}

void PbrSpecularMaterial::set_specular_glossiness_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_specular_glossiness_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void PbrSpecularMaterial::set_specular_glossiness_texture(const std::shared_ptr<Image> &new_value,
                                                          const VkSamplerCreateInfo &info) {
    specular_glossiness_texture_ = new_value;
    shader_data_.set_texture(PbrSpecularMaterial::specular_glossiness_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_SPECULARGLOSSINESSMAP");
    } else {
        shader_data_.add_undefine("HAS_SPECULARGLOSSINESSMAP");
    }
}

PbrSpecularMaterial::PbrSpecularMaterial(Device &device, const std::string &name) :
PbrBaseMaterial(device, name),
glossiness_prop_(ShaderProperty::create("u_glossiness", ShaderDataGroup::MATERIAL)),
specular_color_prop_(ShaderProperty::create("u_specularColor", ShaderDataGroup::MATERIAL)),
specular_glossiness_texture_prop_(ShaderProperty::create("_specularGlossinessTexture", ShaderDataGroup::MATERIAL)) {
    shader_data_.set_data(PbrSpecularMaterial::specular_color_prop_, Color(1, 1, 1, 1));
    shader_data_.set_data(PbrSpecularMaterial::glossiness_prop_, 1.f);
}

}
