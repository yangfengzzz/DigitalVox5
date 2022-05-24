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
 * Blinn-phong Material.
 */
class BlinnPhongMaterial : public BaseMaterial {
public:
    struct alignas(16) BlinnPhongData {
        Color base_color = Color(1, 1, 1, 1);
        Color specular_color = Color(1, 1, 1, 1);
        Color emissive_color = Color(0, 0, 0, 1);
        float normal_intensity = 1.f;
        float shininess = 16.f;
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
     * Specular color.
     */
    [[nodiscard]] const Color &SpecularColor() const;

    void SetSpecularColor(const Color &new_value);

    /**
     * Specular texture.
     */
    [[nodiscard]] std::shared_ptr<Texture> SpecularTexture() const;

    void SetSpecularTexture(const std::shared_ptr<Texture> &new_value);

    void SetSpecularTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info);

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
     * Normal texture.
     */
    [[nodiscard]] std::shared_ptr<Texture> NormalTexture() const;

    void SetNormalTexture(const std::shared_ptr<Texture> &new_value);

    void SetNormalTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info);

    /**
     * Normal texture intensity.
     */
    [[nodiscard]] float NormalIntensity() const;

    void SetNormalIntensity(float new_value);

    /**
     * Set the specular reflection coefficient, the larger the value, the more convergent the specular reflection
     * effect.
     */
    [[nodiscard]] float Shininess() const;

    void SetShininess(float new_value);

    BlinnPhongMaterial(Device &device, const std::string &name = "");

private:
    BlinnPhongData blinn_phong_data_;
    const std::string blinn_phong_prop_;

    std::shared_ptr<Texture> base_texture_{nullptr};
    const std::string base_texture_prop_;

    std::shared_ptr<Texture> specular_texture_{nullptr};
    const std::string specular_texture_prop_;

    std::shared_ptr<Texture> emissive_texture_{nullptr};
    const std::string emissive_texture_prop_;

    std::shared_ptr<Texture> normal_texture_{nullptr};
    const std::string normal_texture_prop_;
};

}  // namespace vox
