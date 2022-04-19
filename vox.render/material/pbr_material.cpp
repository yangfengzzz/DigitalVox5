//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_material.h"

namespace vox {
float PbrMaterial::metallic() const {
    return metallic_;
}

void PbrMaterial::set_metallic(float new_value) {
    metallic_ = new_value;
    shader_data_.set_data(PbrMaterial::metallic_prop_, new_value);
}

float PbrMaterial::roughness() const {
    return roughness_;
}

void PbrMaterial::set_roughness(float new_value) {
    roughness_ = new_value;
    shader_data_.set_data(PbrMaterial::roughness_prop_, new_value);
}

std::shared_ptr<Image> PbrMaterial::metallic_roughness_texture() {
    return metallic_roughness_texture_;
}

void PbrMaterial::set_metallic_roughness_texture(const std::shared_ptr<Image> &new_value) {
    BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->get_mipmaps().size());
    set_metallic_roughness_texture(new_value, BaseMaterial::last_sampler_create_info_);
}

void PbrMaterial::set_metallic_roughness_texture(const std::shared_ptr<Image> &new_value,
                                                 const VkSamplerCreateInfo &info) {
    metallic_roughness_texture_ = new_value;
    shader_data_.set_texture(PbrMaterial::metallic_roughness_texture_prop_, new_value, get_sampler_(info));
    if (new_value) {
        shader_data_.add_define("HAS_METALROUGHNESSMAP");
    } else {
        shader_data_.remove_define("HAS_METALROUGHNESSMAP");
    }
}

PbrMaterial::PbrMaterial(Device &device, const std::string &name) :
PbrBaseMaterial(device, name),
metallic_prop_("u_metal"),
roughness_prop_("u_roughness"),
metallic_roughness_texture_prop_("u_metallicRoughnessTexture") {
    shader_data_.add_define("IS_METALLIC_WORKFLOW");
    shader_data_.set_data(PbrMaterial::metallic_prop_, 1.f);
    shader_data_.set_data(PbrMaterial::roughness_prop_, 1.f);
}

}
