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
    
    BlinnPhongMaterial(Device &device, const std::string &name = "");
    
private:
    BlinnPhongData blinn_phong_data_;
    const std::string blinn_phong_prop_;
    
    std::shared_ptr<Image> base_texture_{nullptr};
    const std::string base_texture_prop_;
    
    std::shared_ptr<Image> specular_texture_{nullptr};
    const std::string specular_texture_prop_;
    
    std::shared_ptr<Image> emissive_texture_{nullptr};
    const std::string emissive_texture_prop_;
    
    std::shared_ptr<Image> normal_texture_{nullptr};
    const std::string normal_texture_prop_;
};

}
