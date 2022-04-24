//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_specular_material.h"
#include "shader/internal_variant_name.h"
#include "shader/shader_manager.h"

namespace vox {
const Color &PbrSpecularMaterial::specular_color() const {
    return pbr_specular_data_.specular_color;
}

void PbrSpecularMaterial::set_specular_color(const Color &new_value) {
    pbr_specular_data_.specular_color = new_value;
    shader_data_.set_data(pbr_specular_prop_, pbr_specular_data_);
}

float PbrSpecularMaterial::glossiness() const {
    return pbr_specular_data_.glossiness;
}

void PbrSpecularMaterial::set_glossiness(float new_value) {
    pbr_specular_data_.glossiness = new_value;
    shader_data_.set_data(pbr_specular_prop_, pbr_specular_data_);
}

std::shared_ptr<Image> PbrSpecularMaterial::specular_glossiness_texture() const {
    return specular_glossiness_texture_;
}

void PbrSpecularMaterial::set_specular_glossiness_texture(const std::shared_ptr<Image> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
        set_specular_glossiness_texture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void PbrSpecularMaterial::set_specular_glossiness_texture(const std::shared_ptr<Image> &new_value,
                                                          const VkSamplerCreateInfo &info) {
    specular_glossiness_texture_ = new_value;
    if (new_value) {
        shader_data_.set_sampled_texture(specular_glossiness_texture_prop_, new_value, get_sampler_(info));
        shader_data_.add_define(HAS_SPECULARGLOSSINESSMAP);
    } else {
        shader_data_.remove_define(HAS_SPECULARGLOSSINESSMAP);
    }
}

PbrSpecularMaterial::PbrSpecularMaterial(Device &device, const std::string &name) :
PbrBaseMaterial(device, name),
pbr_specular_prop_("pbrSpecularData"),
specular_glossiness_texture_prop_("specularGlossinessTexture") {
    vertex_source_ = ShaderManager::get_singleton().load_shader("base/blinn-phong.vert");
    fragment_source_ = ShaderManager::get_singleton().load_shader("base/pbr.frag");
    
    shader_data_.set_data(pbr_specular_prop_, pbr_specular_data_);
}

}
