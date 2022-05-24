//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.math/vector4.h"
#include "vox.render/material/base_material.h"
#include "vox.render/texture.h"

namespace vox {
/**
 * PBR (Physically-Based Rendering) Material.
 */
class PbrBaseMaterial : public BaseMaterial {
public:
    struct alignas(16) PBRBaseData {
        Color base_color = Color(1, 1, 1, 1);
        Color emissive_color = Color(0, 0, 0, 1);
        float normal_texture_intensity = 1.f;
        float occlusion_texture_intensity = 1.f;
    };

    /**
     * Base color.
     */
    [[nodiscard]] const Color &BaseColor() const;

    void SetBaseColor(const Color &new_value);

    /**
     * Base texture.
     */
    [[nodiscard]] std::shared_ptr<Texture> BaseTexture() const;

    void SetBaseTexture(const std::shared_ptr<Texture> &new_value);

    void SetBaseTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info);

    /**
     * Normal texture.
     */
    [[nodiscard]] std::shared_ptr<Texture> NormalTexture() const;

    void SetNormalTexture(const std::shared_ptr<Texture> &new_value);

    void SetNormalTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info);

    /**
     * Normal texture intensity.
     */
    [[nodiscard]] float NormalTextureIntensity() const;

    void SetNormalTextureIntensity(float new_value);

    /**
     * Emissive color.
     */
    [[nodiscard]] const Color &EmissiveColor() const;

    void SetEmissiveColor(const Color &new_value);

    /**
     * Emissive texture.
     */
    [[nodiscard]] std::shared_ptr<Texture> EmissiveTexture() const;

    void SetEmissiveTexture(const std::shared_ptr<Texture> &new_value);

    void SetEmissiveTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info);

    /**
     * Occlusion texture.
     */
    [[nodiscard]] std::shared_ptr<Texture> OcclusionTexture() const;

    void SetOcclusionTexture(const std::shared_ptr<Texture> &new_value);

    void SetOcclusionTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info);

    /**
     * Occlusion texture intensity.
     */
    [[nodiscard]] float OcclusionTextureIntensity() const;

    void SetOcclusionTextureIntensity(float new_value);

protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PbrBaseMaterial(Device &device, const std::string &name);

private:
    PBRBaseData pbr_base_data_;
    const std::string pbr_base_prop_;

    std::shared_ptr<Texture> base_texture_{nullptr};
    const std::string base_texture_prop_;

    std::shared_ptr<Texture> normal_texture_{nullptr};
    const std::string normal_texture_prop_;

    std::shared_ptr<Texture> emissive_texture_{nullptr};
    const std::string emissive_texture_prop_;

    std::shared_ptr<Texture> occlusion_texture_{nullptr};
    const std::string occlusion_texture_prop_;
};

}  // namespace vox
