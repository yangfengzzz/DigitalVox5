//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/pbr_base_material.h"

namespace vox {
/**
 * PBR (Specular-Glossiness Workflow) Material.
 */
class PbrSpecularMaterial : public PbrBaseMaterial {
public:
    struct alignas(16) PBRSpecularData {
        Color specular_color = Color(1, 1, 1, 1);
        float glossiness = 1.f;
        ;
    };

    /**
     * Specular color.
     */
    [[nodiscard]] const Color &SpecularColor() const;

    void SetSpecularColor(const Color &new_value);

    /**
     * Glossiness.
     */
    [[nodiscard]] float Glossiness() const;

    void SetGlossiness(float new_value);

    /**
     * Specular glossiness texture.
     * @remarks RGB is specular, A is glossiness
     */
    [[nodiscard]] std::shared_ptr<Texture> SpecularGlossinessTexture() const;

    void SetSpecularGlossinessTexture(const std::shared_ptr<Texture> &new_value);

    void SetSpecularGlossinessTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info);

    /**
     * Create a pbr specular-glossiness workflow material instance.
     */
    PbrSpecularMaterial(Device &device, const std::string &name = "");

private:
    PBRSpecularData pbr_specular_data_;
    const std::string pbr_specular_prop_;

    std::shared_ptr<Texture> specular_glossiness_texture_{nullptr};
    const std::string specular_glossiness_texture_prop_;
};

}  // namespace vox
