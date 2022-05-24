//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/matrix4x4.h"
#include "vox.math/spherical_harmonics3.h"
#include "vox.render/core/sampler.h"
#include "vox.render/scene_forward.h"
#include "vox.render/texture.h"

namespace vox {
/**
 * Diffuse mode.
 */
enum class DiffuseMode {
    /** Solid color mode. */
    SOLID_COLOR,

    /** Texture mode. */
    TEXTURE,

    /**
     * SH mode
     * @remarks
     * Use SH3 to represent irradiance environment maps efficiently, allowing for interactive rendering of diffuse
     * objects under distant illumination.
     */
    SPHERICAL_HARMONICS
};

/**
 * Ambient light.
 */
class AmbientLight {
public:
    struct alignas(16) EnvMapLight {
        Vector3F diffuse;
        uint32_t mip_map_level;
        float diffuse_intensity;
        float specular_intensity;
    };

    AmbientLight();

    void SetScene(Scene *value);

    /**
     * Diffuse mode of ambient light.
     */
    DiffuseMode GetDiffuseMode();

    void SetDiffuseMode(DiffuseMode value);

    /**
     * Diffuse reflection solid color.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SolidColor`.
     */
    [[nodiscard]] Color DiffuseSolidColor() const;

    void SetDiffuseSolidColor(const Color &value);

    /**
     * Diffuse reflection spherical harmonics 3.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SphericalHarmonics`.
     */
    const SphericalHarmonics3 &DiffuseSphericalHarmonics();

    void SetDiffuseSphericalHarmonics(const SphericalHarmonics3 &value);

    /**
     * Diffuse reflection intensity.
     */
    [[nodiscard]] float DiffuseIntensity() const;

    void SetDiffuseIntensity(float value);

public:
    /**
     * Whether to decode from SpecularTexture with RGBM format.
     */
    [[nodiscard]] bool SpecularTextureDecodeRgbm() const;

    void SetSpecularTextureDecodeRgbm(bool value);

    /**
     * Specular reflection texture.
     * @remarks This texture must be baked from MetalLoader::createSpecularTexture
     */
    std::shared_ptr<Texture> SpecularTexture();

    void SetSpecularTexture(const std::shared_ptr<Texture> &value);

    /**
     * Specular reflection intensity.
     */
    [[nodiscard]] float SpecularIntensity() const;

    void SetSpecularIntensity(float value);

private:
    static std::array<float, 27> PreComputeSh(const SphericalHarmonics3 &sh);

    VkSamplerCreateInfo sampler_create_info_;
    std::unique_ptr<core::Sampler> sampler_{nullptr};

    EnvMapLight env_map_light_;
    const std::string env_map_property_;

    SphericalHarmonics3 diffuse_spherical_harmonics_;
    std::array<float, 27> sh_array_{};
    const std::string diffuse_sh_property_;

    bool specular_texture_decode_rgbm_{false};
    std::shared_ptr<Texture> specular_reflection_{nullptr};
    const std::string specular_texture_property_;

    Scene *scene_{nullptr};
    DiffuseMode diffuse_mode_ = DiffuseMode::SOLID_COLOR;
};

}  // namespace vox
