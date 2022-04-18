//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "pbr_base_material.h"

namespace vox {
/**
 * PBR (Metallic-Roughness Workflow) Material.
 */
class PbrMaterial : public PbrBaseMaterial {
public:
    /**
     * Metallic.
     */
    [[nodiscard]] float metallic() const;
    
    void set_metallic(float new_value);
    
    /**
     * Roughness.
     */
    [[nodiscard]] float roughness() const;
    
    void set_roughness(float new_value);
    
    /**
     * Roughness metallic texture.
     * @remarks G channel is roughness, B channel is metallic
     */
    std::shared_ptr<Image> metallic_roughness_texture();
    
    void set_metallic_roughness_texture(const std::shared_ptr<Image> &new_value);
    
    void set_metallic_roughness_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Create a pbr metallic-roughness workflow material instance.
     */
    PbrMaterial(Device &device, const std::string &name = "");
    
private:
    float metallic_{1.f};
    const std::string metallic_prop_;
    
    float roughness_{1.f};
    const std::string roughness_prop_;
    
    std::shared_ptr<Image> metallic_roughness_texture_{nullptr};
    const std::string metallic_roughness_texture_prop_;
};

}
