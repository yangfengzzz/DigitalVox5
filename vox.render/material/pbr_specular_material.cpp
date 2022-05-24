//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/pbr_specular_material.h"

#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
const Color &PbrSpecularMaterial::SpecularColor() const { return pbr_specular_data_.specular_color; }

void PbrSpecularMaterial::SetSpecularColor(const Color &new_value) {
    pbr_specular_data_.specular_color = new_value;
    shader_data_.SetData(pbr_specular_prop_, pbr_specular_data_);
}

float PbrSpecularMaterial::Glossiness() const { return pbr_specular_data_.glossiness; }

void PbrSpecularMaterial::SetGlossiness(float new_value) {
    pbr_specular_data_.glossiness = new_value;
    shader_data_.SetData(pbr_specular_prop_, pbr_specular_data_);
}

std::shared_ptr<Texture> PbrSpecularMaterial::SpecularGlossinessTexture() const { return specular_glossiness_texture_; }

void PbrSpecularMaterial::SetSpecularGlossinessTexture(const std::shared_ptr<Texture> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        SetSpecularGlossinessTexture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void PbrSpecularMaterial::SetSpecularGlossinessTexture(const std::shared_ptr<Texture> &new_value,
                                                       const VkSamplerCreateInfo &info) {
    specular_glossiness_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(specular_glossiness_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_SPECULARGLOSSINESSMAP);
    } else {
        shader_data_.RemoveDefine(HAS_SPECULARGLOSSINESSMAP);
    }
}

PbrSpecularMaterial::PbrSpecularMaterial(Device &device, const std::string &name)
    : PbrBaseMaterial(device, name),
      pbr_specular_prop_("pbrSpecularData"),
      specular_glossiness_texture_prop_("specularGlossinessTexture") {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/pbr.frag");

    shader_data_.SetData(pbr_specular_prop_, pbr_specular_data_);
}

}  // namespace vox
