//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/blinn_phong_material.h"

#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
const Color &BlinnPhongMaterial::BaseColor() const { return blinn_phong_data_.base_color; }

void BlinnPhongMaterial::SetBaseColor(const Color &new_value) {
    blinn_phong_data_.base_color = new_value;
    shader_data_.SetData(blinn_phong_prop_, blinn_phong_data_);
}

std::shared_ptr<Texture> BlinnPhongMaterial::BaseTexture() const { return base_texture_; }

void BlinnPhongMaterial::SetBaseTexture(const std::shared_ptr<Texture> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        SetBaseTexture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void BlinnPhongMaterial::SetBaseTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info) {
    base_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(base_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_DIFFUSE_TEXTURE);
    } else {
        shader_data_.RemoveDefine(HAS_DIFFUSE_TEXTURE);
    }
}

const Color &BlinnPhongMaterial::SpecularColor() const { return blinn_phong_data_.specular_color; }

void BlinnPhongMaterial::SetSpecularColor(const Color &new_value) {
    blinn_phong_data_.specular_color = new_value;
    shader_data_.SetData(blinn_phong_prop_, blinn_phong_data_);
}

std::shared_ptr<Texture> BlinnPhongMaterial::SpecularTexture() const { return specular_texture_; }

void BlinnPhongMaterial::SetSpecularTexture(const std::shared_ptr<Texture> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        SetSpecularTexture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void BlinnPhongMaterial::SetSpecularTexture(const std::shared_ptr<Texture> &new_value,
                                            const VkSamplerCreateInfo &info) {
    specular_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(specular_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_SPECULAR_TEXTURE);
    } else {
        shader_data_.RemoveDefine(HAS_SPECULAR_TEXTURE);
    }
}

const Color &BlinnPhongMaterial::EmissiveColor() const { return blinn_phong_data_.emissive_color; }

void BlinnPhongMaterial::SetEmissiveColor(const Color &new_value) {
    blinn_phong_data_.emissive_color = new_value;
    shader_data_.SetData(blinn_phong_prop_, blinn_phong_data_);
}

std::shared_ptr<Texture> BlinnPhongMaterial::EmissiveTexture() const { return emissive_texture_; }

void BlinnPhongMaterial::BlinnPhongMaterial::SetEmissiveTexture(const std::shared_ptr<Texture> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        SetEmissiveTexture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void BlinnPhongMaterial::SetEmissiveTexture(const std::shared_ptr<Texture> &new_value,
                                            const VkSamplerCreateInfo &info) {
    emissive_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(emissive_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_EMISSIVE_TEXTURE);
    } else {
        shader_data_.RemoveDefine(HAS_EMISSIVE_TEXTURE);
    }
}

std::shared_ptr<Texture> BlinnPhongMaterial::NormalTexture() const { return normal_texture_; }

void BlinnPhongMaterial::SetNormalTexture(const std::shared_ptr<Texture> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        SetNormalTexture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void BlinnPhongMaterial::SetNormalTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info) {
    normal_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(normal_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_NORMAL_TEXTURE);
    } else {
        shader_data_.RemoveDefine(HAS_NORMAL_TEXTURE);
    }
}

float BlinnPhongMaterial::NormalIntensity() const { return blinn_phong_data_.normal_intensity; }

void BlinnPhongMaterial::SetNormalIntensity(float new_value) {
    blinn_phong_data_.normal_intensity = new_value;
    shader_data_.SetData(blinn_phong_prop_, blinn_phong_data_);
}

float BlinnPhongMaterial::Shininess() const { return blinn_phong_data_.shininess; }

void BlinnPhongMaterial::SetShininess(float new_value) {
    blinn_phong_data_.shininess = new_value;
    shader_data_.SetData(blinn_phong_prop_, blinn_phong_data_);
}

BlinnPhongMaterial::BlinnPhongMaterial(Device &device, const std::string &name)
    : BaseMaterial(device, name),
      blinn_phong_prop_("blinnPhongData"),
      base_texture_prop_("diffuseTexture"),
      specular_texture_prop_("specularTexture"),
      emissive_texture_prop_("emissiveTexture"),
      normal_texture_prop_("normalTexture") {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/blinn-phong.frag");

    shader_data_.AddDefine(NEED_WORLDPOS);
    shader_data_.SetData(blinn_phong_prop_, blinn_phong_data_);
}

}  // namespace vox
