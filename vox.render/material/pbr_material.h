//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/pbr_base_material.h"

namespace vox {
/**
 * PBR (Metallic-Roughness Workflow) Material.
 */
class PbrMaterial : public PbrBaseMaterial {
public:
    struct alignas(16) PBRData {
        float metallic = 1.f;
        float roughness = 1.f;
    };

    /**
     * Metallic.
     */
    [[nodiscard]] float Metallic() const;

    void SetMetallic(float new_value);

    /**
     * Roughness.
     */
    [[nodiscard]] float Roughness() const;

    void SetRoughness(float new_value);

    /**
     * Roughness metallic texture.
     * @remarks G channel is roughness, B channel is metallic
     */
    std::shared_ptr<Texture> MetallicRoughnessTexture();

    void SetMetallicRoughnessTexture(const std::shared_ptr<Texture> &new_value);

    void SetMetallicRoughnessTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info);

    /**
     * Create a pbr metallic-roughness workflow material instance.
     */
    PbrMaterial(Device &device, const std::string &name = "");

private:
    PBRData pbr_data_;
    const std::string pbr_prop_;

    std::shared_ptr<Texture> metallic_roughness_texture_{nullptr};
    const std::string metallic_roughness_texture_prop_;
};

}  // namespace vox
