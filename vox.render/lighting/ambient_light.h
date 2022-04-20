//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "scene_forward.h"
#include "spherical_harmonics3.h"
#include "../image.h"
#include "core/sampler.h"
#include "matrix4x4.h"

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
     * Use SH3 to represent irradiance environment maps efficiently, allowing for interactive rendering of diffuse objects under distant illumination.
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
    
    void set_scene(Scene *value);
    
    /**
     * Diffuse mode of ambient light.
     */
    DiffuseMode diffuse_mode();
    
    void set_diffuse_mode(DiffuseMode value);
    
    /**
     * Diffuse reflection solid color.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SolidColor`.
     */
    [[nodiscard]] Color diffuse_solid_color() const;
    
    void set_diffuse_solid_color(const Color &value);
    
    /**
     * Diffuse reflection spherical harmonics 3.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SphericalHarmonics`.
     */
    const SphericalHarmonics3 &diffuse_spherical_harmonics();
    
    void set_diffuse_spherical_harmonics(const SphericalHarmonics3 &value);
    
    /**
     * Diffuse reflection texture.
     * @remarks This texture must be baked from MetalLoader::createIrradianceTexture
     */
    std::shared_ptr<Image> diffuse_texture();
    
    void set_diffuse_texture(const std::shared_ptr<Image> &value);
    
    /**
     * Diffuse reflection intensity.
     */
    [[nodiscard]] float diffuse_intensity() const;
    
    void set_diffuse_intensity(float value);
    
public:
    /**
     * Whether to decode from specular_texture with RGBM format.
     */
    [[nodiscard]] bool specular_texture_decode_rgbm() const;
    
    void set_specular_texture_decode_rgbm(bool value);
    
    /**
     * Specular reflection texture.
     * @remarks This texture must be baked from MetalLoader::createSpecularTexture
     */
    std::shared_ptr<Image> specular_texture();
    
    void set_specular_texture(const std::shared_ptr<Image> &value);
    
    /**
     * Specular reflection intensity.
     */
    [[nodiscard]] float specular_intensity() const;
    
    void set_specular_intensity(float value);
    
public:
    /**
     * brdf lookup texture.
     * @remarks This texture must be baked from MetalLoader::createBRDFLookupTable
     */
    std::shared_ptr<Image> brdf_texture();
    
    void set_brdf_texture(const std::shared_ptr<Image> &value);
    
private:
    static std::array<float, 27> pre_compute_sh(const SphericalHarmonics3 &sh);
    
    VkSamplerCreateInfo sampler_create_info_;
    std::unique_ptr<core::Sampler> sampler_{nullptr};
    
    EnvMapLight env_map_light_;
    const std::string env_map_property_;
    
    SphericalHarmonics3 diffuse_spherical_harmonics_;
    std::array<float, 27> sh_array_{};
    const std::string diffuse_sh_property_;
    
    std::shared_ptr<Image> diffuse_texture_{nullptr};
    const std::string diffuse_texture_property_;
    
    bool specular_texture_decode_rgbm_{false};
    std::shared_ptr<Image> specular_reflection_{nullptr};
    const std::string specular_texture_property_;
    
    std::shared_ptr<Image> brdf_lut_texture_{nullptr};
    const std::string brdf_texture_property_;
    
    Scene *scene_{nullptr};
    DiffuseMode diffuse_mode_ = DiffuseMode::SOLID_COLOR;
};

}
