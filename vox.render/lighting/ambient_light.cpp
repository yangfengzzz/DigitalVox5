//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/ambient_light.h"

#include "vox.render/scene.h"
#include "vox.render/shader/internal_variant_name.h"

namespace vox {
AmbientLight::AmbientLight()
    : env_map_property_("envMapLight"),
      diffuse_sh_property_("envSH"),
      specular_texture_property_("env_specularTexture"),
      sampler_create_info_{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO} {}

void AmbientLight::SetScene(Scene *value) {
    scene_ = value;
    if (!value) return;

    // Create a default sampler
    sampler_create_info_.magFilter = VK_FILTER_LINEAR;
    sampler_create_info_.minFilter = VK_FILTER_LINEAR;
    sampler_create_info_.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info_.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.mipLodBias = 0.0f;
    sampler_create_info_.compareOp = VK_COMPARE_OP_NEVER;
    sampler_create_info_.minLod = 0.0f;
    // Max level-of-detail should match mip level count
    sampler_create_info_.maxLod = std::numeric_limits<float>::max();
    sampler_create_info_.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    sampler_create_info_.maxAnisotropy = 1;
    sampler_create_info_.unnormalizedCoordinates = false;
    sampler_ = std::make_unique<core::Sampler>(scene_->Device(), sampler_create_info_);

    env_map_light_.diffuse = Vector3F(0.212, 0.227, 0.259);
    env_map_light_.diffuse_intensity = 1.0;
    env_map_light_.specular_intensity = 1.0;
    scene_->shader_data.SetData(env_map_property_, env_map_light_);
}

DiffuseMode AmbientLight::GetDiffuseMode() { return diffuse_mode_; }

void AmbientLight::SetDiffuseMode(DiffuseMode value) {
    diffuse_mode_ = value;
    if (!scene_) return;

    switch (value) {
        case DiffuseMode::SPHERICAL_HARMONICS:
            scene_->shader_data.RemoveDefine(HAS_DIFFUSE_ENV);
            scene_->shader_data.AddDefine(HAS_SH);
            break;

        case DiffuseMode::TEXTURE:
            scene_->shader_data.RemoveDefine(HAS_SH);
            scene_->shader_data.AddDefine(HAS_DIFFUSE_ENV);
            break;

        default:
            break;
    }
}

Color AmbientLight::DiffuseSolidColor() const {
    return {env_map_light_.diffuse.x, env_map_light_.diffuse.y, env_map_light_.diffuse.z};
}

void AmbientLight::SetDiffuseSolidColor(const Color &value) {
    env_map_light_.diffuse = Vector3F(value.r, value.g, value.b);
    scene_->shader_data.SetData(env_map_property_, env_map_light_);
}

const SphericalHarmonics3 &AmbientLight::DiffuseSphericalHarmonics() { return diffuse_spherical_harmonics_; }

void AmbientLight::SetDiffuseSphericalHarmonics(const SphericalHarmonics3 &value) {
    diffuse_spherical_harmonics_ = value;
    if (!scene_) return;

    auto sh = PreComputeSh(value);
    scene_->shader_data.SetData(diffuse_sh_property_, sh);
}

float AmbientLight::DiffuseIntensity() const { return env_map_light_.diffuse_intensity; }

void AmbientLight::SetDiffuseIntensity(float value) {
    env_map_light_.diffuse_intensity = value;
    if (!scene_) return;

    scene_->shader_data.SetData(env_map_property_, env_map_light_);
}

// MARK: - Specular
bool AmbientLight::SpecularTextureDecodeRgbm() const { return specular_texture_decode_rgbm_; }

void AmbientLight::SetSpecularTextureDecodeRgbm(bool value) {}

std::shared_ptr<Texture> AmbientLight::SpecularTexture() { return specular_reflection_; }

void AmbientLight::SetSpecularTexture(const std::shared_ptr<Texture> &value) {
    specular_reflection_ = value;
    if (!scene_) return;

    auto &shader_data = scene_->shader_data;

    if (value) {
        shader_data.SetSampledTexture(specular_texture_property_,
                                      specular_reflection_->GetVkImageView(VK_IMAGE_VIEW_TYPE_CUBE), sampler_.get());
        env_map_light_.mip_map_level = static_cast<uint32_t>(value->GetMipmaps().size() - 1);
        scene_->shader_data.SetData(env_map_property_, env_map_light_);
        shader_data.AddDefine(HAS_SPECULAR_ENV);
    } else {
        shader_data.RemoveDefine(HAS_SPECULAR_ENV);
    }
}

float AmbientLight::SpecularIntensity() const { return env_map_light_.specular_intensity; }

void AmbientLight::SetSpecularIntensity(float value) {
    env_map_light_.specular_intensity = value;
    if (!scene_) return;

    scene_->shader_data.SetData(env_map_property_, env_map_light_);
}

std::array<float, 27> AmbientLight::PreComputeSh(const SphericalHarmonics3 &sh) {
    /**
     * Basis constants
     *
     * 0: 1/2 * Math.sqrt(1 / Math.PI)
     *
     * 1: -1/2 * Math.sqrt(3 / Math.PI)
     * 2: 1/2 * Math.sqrt(3 / Math.PI)
     * 3: -1/2 * Math.sqrt(3 / Math.PI)
     *
     * 4: 1/2 * Math.sqrt(15 / Math.PI)
     * 5: -1/2 * Math.sqrt(15 / Math.PI)
     * 6: 1/4 * Math.sqrt(5 / Math.PI)
     * 7: -1/2 * Math.sqrt(15 / Math.PI)
     * 8: 1/4 * Math.sqrt(15 / Math.PI)
     */

    /**
     * Convolution kernel
     *
     * 0: Math.PI
     * 1: (2 * Math.PI) / 3
     * 2: Math.PI / 4
     */

    const auto &src = sh.coefficients();
    std::array<float, 27> out{};
    // l0
    out[0] = src[0] * 0.886227f;  // kernel0 * basis0 = 0.886227
    out[1] = src[1] * 0.886227f;
    out[2] = src[2] * 0.886227f;

    // l1
    out[3] = src[3] * -1.023327f;  // kernel1 * basis1 = -1.023327;
    out[4] = src[4] * -1.023327f;
    out[5] = src[5] * -1.023327f;
    out[6] = src[6] * 1.023327f;  // kernel1 * basis2 = 1.023327
    out[7] = src[7] * 1.023327f;
    out[8] = src[8] * 1.023327f;
    out[9] = src[9] * -1.023327f;  // kernel1 * basis3 = -1.023327
    out[10] = src[10] * -1.023327f;
    out[11] = src[11] * -1.023327f;

    // l2
    out[12] = src[12] * 0.858086f;  // kernel2 * basis4 = 0.858086
    out[13] = src[13] * 0.858086f;
    out[14] = src[14] * 0.858086f;
    out[15] = src[15] * -0.858086f;  // kernel2 * basis5 = -0.858086
    out[16] = src[16] * -0.858086f;
    out[17] = src[17] * -0.858086f;
    out[18] = src[18] * 0.247708f;  // kernel2 * basis6 = 0.247708
    out[19] = src[19] * 0.247708f;
    out[20] = src[20] * 0.247708f;
    out[21] = src[21] * -0.858086f;  // kernel2 * basis7 = -0.858086
    out[22] = src[22] * -0.858086f;
    out[23] = src[23] * -0.858086f;
    out[24] = src[24] * 0.429042f;  // kernel2 * basis8 = 0.429042
    out[25] = src[25] * 0.429042f;
    out[26] = src[26] * 0.429042f;

    return out;
}

}  // namespace vox
