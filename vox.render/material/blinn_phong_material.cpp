//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "blinn_phong_material.h"
#include "shader/internal_variant_name.h"
#include "shader/shader_manager.h"

namespace vox {
const Color &BlinnPhongMaterial::base_color() const {
    return blinn_phong_data_.base_color;
}

void BlinnPhongMaterial::set_base_color(const Color &new_value) {
    blinn_phong_data_.base_color = new_value;
    shader_data_.set_data(blinn_phong_prop_, blinn_phong_data_);
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
    shader_data_.set_texture(base_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define(HAS_DIFFUSE_TEXTURE);
    } else {
        shader_data_.remove_define(HAS_DIFFUSE_TEXTURE);
    }
}

const Color &BlinnPhongMaterial::specular_color() const {
    return blinn_phong_data_.specular_color;
}

void BlinnPhongMaterial::set_specular_color(const Color &new_value) {
    blinn_phong_data_.specular_color = new_value;
    shader_data_.set_data(blinn_phong_prop_, blinn_phong_data_);
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
    shader_data_.set_texture(specular_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define(HAS_SPECULAR_TEXTURE);
    } else {
        shader_data_.remove_define(HAS_SPECULAR_TEXTURE);
    }
}

const Color &BlinnPhongMaterial::emissive_color() const {
    return blinn_phong_data_.emissive_color;
}

void BlinnPhongMaterial::set_emissive_color(const Color &new_value) {
    blinn_phong_data_.emissive_color = new_value;
    shader_data_.set_data(blinn_phong_prop_, blinn_phong_data_);
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
    shader_data_.set_texture(emissive_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define(HAS_EMISSIVE_TEXTURE);
    } else {
        shader_data_.remove_define(HAS_EMISSIVE_TEXTURE);
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
    shader_data_.set_texture(normal_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define(HAS_NORMAL_TEXTURE);
    } else {
        shader_data_.remove_define(HAS_NORMAL_TEXTURE);
    }
}

float BlinnPhongMaterial::normal_intensity() const {
    return blinn_phong_data_.normal_intensity;
}

void BlinnPhongMaterial::set_normal_intensity(float new_value) {
    blinn_phong_data_.normal_intensity = new_value;
    shader_data_.set_data(blinn_phong_prop_, blinn_phong_data_);
}

float BlinnPhongMaterial::shininess() const {
    return blinn_phong_data_.shininess;
}

void BlinnPhongMaterial::set_shininess(float new_value) {
    blinn_phong_data_.shininess = new_value;
    shader_data_.set_data(blinn_phong_prop_, blinn_phong_data_);
}

BlinnPhongMaterial::BlinnPhongMaterial(Device &device, const std::string &name) :
BaseMaterial(device, name),
blinn_phong_prop_("blinnPhongData"),
base_texture_prop_("diffuseTexture"),
specular_texture_prop_("specularTexture"),
emissive_texture_prop_("emissiveTexture"),
normal_texture_prop_("normalTexture") {
    vertex_source_ = ShaderManager::get_singleton().load_shader("base/blinn-phong.vert");
    fragment_source_ = ShaderManager::get_singleton().load_shader("base/blinn-phong.frag");
    
    shader_data_.add_define(NEED_WORLDPOS);
    shader_data_.set_data(blinn_phong_prop_, blinn_phong_data_);
}

}
