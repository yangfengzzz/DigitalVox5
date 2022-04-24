//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_material.h"
#include "shader/internal_variant_name.h"
#include "shader/shader_manager.h"

namespace vox {
float PbrMaterial::metallic() const {
    return pbr_data_.metallic;
}

void PbrMaterial::set_metallic(float new_value) {
    pbr_data_.metallic = new_value;
    shader_data_.set_data(pbr_prop_, pbr_data_);
}

float PbrMaterial::roughness() const {
    return pbr_data_.roughness;
}

void PbrMaterial::set_roughness(float new_value) {
    pbr_data_.roughness = new_value;
    shader_data_.set_data(pbr_prop_, pbr_data_);
}

std::shared_ptr<Image> PbrMaterial::metallic_roughness_texture() {
    return metallic_roughness_texture_;
}

void PbrMaterial::set_metallic_roughness_texture(const std::shared_ptr<Image> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
        set_metallic_roughness_texture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void PbrMaterial::set_metallic_roughness_texture(const std::shared_ptr<Image> &new_value,
                                                 const VkSamplerCreateInfo &info) {
    metallic_roughness_texture_ = new_value;
    if (new_value) {
        shader_data_.set_sampled_texture(metallic_roughness_texture_prop_, new_value, get_sampler_(info));
        shader_data_.add_define(HAS_METALROUGHNESSMAP);
    } else {
        shader_data_.remove_define(HAS_METALROUGHNESSMAP);
    }
}

PbrMaterial::PbrMaterial(Device &device, const std::string &name) :
PbrBaseMaterial(device, name),
pbr_prop_("pbrData"),
metallic_roughness_texture_prop_("metallicRoughnessTexture") {
    shader_data_.add_define("IS_METALLIC_WORKFLOW");
    vertex_source_ = ShaderManager::get_singleton().load_shader("base/blinn-phong.vert");
    fragment_source_ = ShaderManager::get_singleton().load_shader("base/pbr.frag");
    
    shader_data_.set_data(pbr_prop_, pbr_data_);
}

}
