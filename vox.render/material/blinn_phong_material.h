//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "base_material.h"
#include "color.h"
#include "vector4.h"
#include "../image.h"

namespace vox {
/**
 * Blinn-phong Material.
 */
class BlinnPhongMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    [[nodiscard]] const Color &base_color() const;
    
    void set_base_color(const Color &new_value);
    
    /**
     * Base texture.
     */
    [[nodiscard]] std::shared_ptr<Image> base_texture() const;
    
    void set_base_texture(const std::shared_ptr<Image> &new_value);
    
    void set_base_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Specular color.
     */
    [[nodiscard]] const Color &specular_color() const;
    
    void set_specular_color(const Color &new_value);
    
    /**
     * Specular texture.
     */
    [[nodiscard]] std::shared_ptr<Image> specular_texture() const;
    
    void set_specular_texture(const std::shared_ptr<Image> &new_value);
    
    void set_specular_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Emissive color.
     */
    [[nodiscard]] const Color &emissive_color() const;
    
    void set_emissive_color(const Color &new_value);
    
    /**
     * Emissive texture.
     */
    [[nodiscard]] std::shared_ptr<Image> emissive_texture() const;
    
    void set_emissive_texture(const std::shared_ptr<Image> &new_value);
    
    void set_emissive_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Normal texture.
     */
    [[nodiscard]] std::shared_ptr<Image> normal_texture() const;
    
    void set_normal_texture(const std::shared_ptr<Image> &new_value);
    
    void set_normal_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Normal texture intensity.
     */
    [[nodiscard]] float normal_intensity() const;
    
    void set_normal_intensity(float new_value);
    
    /**
     * Set the specular reflection coefficient, the larger the value, the more convergent the specular reflection effect.
     */
    [[nodiscard]] float shininess() const;
    
    void set_shininess(float new_value);
    
    /**
     * Tiling and offset of main textures.
     */
    [[nodiscard]] const Vector4F &tiling_offset() const;
    
    void set_tiling_offset(const Vector4F &new_value);
    
    BlinnPhongMaterial(Device &device, const std::string &name);
    
private:
    Color base_color_ = Color(1, 1, 1, 1);
    ShaderProperty diffuse_color_prop_;
    
    Color specular_color_ = Color(1, 1, 1, 1);
    ShaderProperty specular_color_prop_;
    
    Color emissive_color_ = Color(0, 0, 0, 1);
    ShaderProperty emissive_color_prop_;
    
    Vector4F tiling_offset_ = Vector4F(1, 1, 0, 0);
    ShaderProperty tiling_offset_prop_;
    
    float shininess_ = 16.f;
    ShaderProperty shininess_prop_;
    
    float normal_intensity_ = 1.f;
    ShaderProperty normal_intensity_prop_;
    
    std::shared_ptr<Image> base_texture_{nullptr};
    ShaderProperty base_texture_prop_;
    
    std::shared_ptr<Image> specular_texture_{nullptr};
    ShaderProperty specular_texture_prop_;
    
    std::shared_ptr<Image> emissive_texture_{nullptr};
    ShaderProperty emissive_texture_prop_;
    
    std::shared_ptr<Image> normal_texture_{nullptr};
    ShaderProperty normal_texture_prop_;
};

}
