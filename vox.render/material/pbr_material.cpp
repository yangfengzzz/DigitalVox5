//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/pbr_material.h"

#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
float PbrMaterial::Metallic() const { return pbr_data_.metallic; }

void PbrMaterial::SetMetallic(float new_value) {
    pbr_data_.metallic = new_value;
    shader_data_.SetData(pbr_prop_, pbr_data_);
}

float PbrMaterial::Roughness() const { return pbr_data_.roughness; }

void PbrMaterial::SetRoughness(float new_value) {
    pbr_data_.roughness = new_value;
    shader_data_.SetData(pbr_prop_, pbr_data_);
}

std::shared_ptr<Texture> PbrMaterial::MetallicRoughnessTexture() { return metallic_roughness_texture_; }

void PbrMaterial::SetMetallicRoughnessTexture(const std::shared_ptr<Texture> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        SetMetallicRoughnessTexture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void PbrMaterial::SetMetallicRoughnessTexture(const std::shared_ptr<Texture> &new_value,
                                              const VkSamplerCreateInfo &info) {
    metallic_roughness_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(metallic_roughness_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_METALROUGHNESSMAP);
    } else {
        shader_data_.RemoveDefine(HAS_METALROUGHNESSMAP);
    }
}

PbrMaterial::PbrMaterial(Device &device, const std::string &name)
    : PbrBaseMaterial(device, name),
      pbr_prop_("pbrData"),
      metallic_roughness_texture_prop_("metallicRoughnessTexture") {
    shader_data_.AddDefine("IS_METALLIC_WORKFLOW");
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/pbr.frag");

    shader_data_.SetData(pbr_prop_, pbr_data_);
}

}  // namespace vox
