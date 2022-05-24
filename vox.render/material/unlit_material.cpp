//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/unlit_material.h"

#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
const Color &UnlitMaterial::BaseColor() const { return base_color_; }

void UnlitMaterial::SetBaseColor(const Color &new_value) {
    base_color_ = new_value;
    shader_data_.SetData(UnlitMaterial::base_color_prop_, base_color_);
}

std::shared_ptr<Texture> UnlitMaterial::BaseTexture() const { return base_texture_; }

void UnlitMaterial::SetBaseTexture(const std::shared_ptr<Texture> &new_value) {
    if (new_value) {
        BaseMaterial::last_sampler_create_info_.maxLod = static_cast<float>(new_value->GetMipmaps().size());
        SetBaseTexture(new_value, BaseMaterial::last_sampler_create_info_);
    }
}

void UnlitMaterial::SetBaseTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info) {
    base_texture_ = new_value;
    if (new_value) {
        shader_data_.SetSampledTexture(base_texture_prop_, new_value->GetVkImageView(),
                                       &device_.GetResourceCache().RequestSampler(info));
        shader_data_.AddDefine(HAS_BASE_TEXTURE);
    } else {
        shader_data_.RemoveDefine(HAS_BASE_TEXTURE);
    }
}

UnlitMaterial::UnlitMaterial(Device &device, const std::string &name)
    : BaseMaterial(device, name), base_color_prop_("baseColor"), base_texture_prop_("baseTexture") {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/unlit.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/unlit.frag");

    shader_data_.AddDefine(OMIT_NORMAL);

    shader_data_.SetData(base_color_prop_, base_color_);
}

}  // namespace vox
