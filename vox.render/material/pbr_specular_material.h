//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "pbr_base_material.h"

namespace vox {
/**
 * PBR (Specular-Glossiness Workflow) Material.
 */
class PbrSpecularMaterial : public PbrBaseMaterial {
public:
    /**
     * Specular color.
     */
    [[nodiscard]] const Color &specular_color() const;
    
    void set_specular_color(const Color &new_value);
    
    /**
     * Glossiness.
     */
    [[nodiscard]] float glossiness() const;
    
    void set_glossiness(float new_value);
    
    /**
     * Specular glossiness texture.
     * @remarks RGB is specular, A is glossiness
     */
    [[nodiscard]] std::shared_ptr<Image> specular_glossiness_texture() const;
    
    void set_specular_glossiness_texture(const std::shared_ptr<Image> &new_value);
    
    void set_specular_glossiness_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Create a pbr specular-glossiness workflow material instance.
     */
    PbrSpecularMaterial(Device &device, const std::string &name = "");
    
private:
    float glossiness_{1.f};
    ShaderProperty glossiness_prop_;
    
    Color specular_color_ = Color(1, 1, 1, 1);
    ShaderProperty specular_color_prop_;
    
    std::shared_ptr<Image> specular_glossiness_texture_{nullptr};
    ShaderProperty specular_glossiness_texture_prop_;
};

}
