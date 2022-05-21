//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_base_material.h"
#include "shader/internal_variant_name.h"

namespace vox {
const Color &PbrBaseMaterial::base_color() const {
    return pbr_base_data_.base_color;
}

void PbrBaseMaterial::set_base_color(const Color &new_value) {
    pbr_base_data_.base_color = new_value;
    shader_data_.SetData(pbr_base_prop_, pbr_base_data_);
}

std::shared_ptr<Image> PbrBaseMaterial::base_texture() const {
    return base_texture_;
}

void PbrBaseMaterial::set_base_texture(const std::shared_ptr<Image> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        set_base_texture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void PbrBaseMaterial::set_base_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    base_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(base_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_BASE_COLORMAP);
    } else {
        shader_data_.RemoveDefine(HAS_BASE_COLORMAP);
    }
}

std::shared_ptr<Image> PbrBaseMaterial::normal_texture() const {
    return normal_texture_;
}

void PbrBaseMaterial::set_normal_texture(const std::shared_ptr<Image> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        set_normal_texture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void PbrBaseMaterial::set_normal_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    normal_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(normal_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_NORMAL_TEXTURE);
    } else {
        shader_data_.RemoveDefine(HAS_NORMAL_TEXTURE);
    }
}

float PbrBaseMaterial::normal_texture_intensity() const {
    return pbr_base_data_.normal_texture_intensity;
}

void PbrBaseMaterial::set_normal_texture_intensity(float new_value) {
    pbr_base_data_.normal_texture_intensity = new_value;
    shader_data_.SetData(pbr_base_prop_, pbr_base_data_);
}

const Color &PbrBaseMaterial::emissive_color() const {
    return pbr_base_data_.emissive_color;
}

void PbrBaseMaterial::set_emissive_color(const Color &new_value) {
    pbr_base_data_.emissive_color = new_value;
    shader_data_.SetData(pbr_base_prop_, pbr_base_data_);
}

std::shared_ptr<Image> PbrBaseMaterial::emissive_texture() const {
    return emissive_texture_;
}

void PbrBaseMaterial::set_emissive_texture(const std::shared_ptr<Image> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        set_emissive_texture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void PbrBaseMaterial::set_emissive_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    emissive_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(emissive_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_EMISSIVEMAP);
    } else {
        shader_data_.RemoveDefine(HAS_EMISSIVEMAP);
    }
}

std::shared_ptr<Image> PbrBaseMaterial::occlusion_texture() const {
    return occlusion_texture_;
}

void PbrBaseMaterial::set_occlusion_texture(const std::shared_ptr<Image> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        set_occlusion_texture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void PbrBaseMaterial::set_occlusion_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info) {
    occlusion_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(occlusion_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_OCCLUSIONMAP);
    } else {
        shader_data_.RemoveDefine(HAS_OCCLUSIONMAP);
    }
}

float PbrBaseMaterial::occlusion_texture_intensity() const {
    return pbr_base_data_.occlusion_texture_intensity;
}

void PbrBaseMaterial::set_occlusion_texture_intensity(float new_value) {
    pbr_base_data_.occlusion_texture_intensity = new_value;
    shader_data_.SetData(pbr_base_prop_, pbr_base_data_);
}

PbrBaseMaterial::PbrBaseMaterial(Device &device, const std::string &name) :
BaseMaterial(device, name),
pbr_base_prop_("pbrBaseData"),
base_texture_prop_("baseColorTexture"),
normal_texture_prop_("normalTexture"),
emissive_texture_prop_("emissiveTexture"),
occlusion_texture_prop_("occlusionTexture") {
    shader_data_.AddDefine(NEED_WORLDPOS);
    shader_data_.SetData(pbr_base_prop_, pbr_base_data_);
}

}
