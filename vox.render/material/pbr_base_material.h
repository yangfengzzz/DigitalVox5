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
    [[nodiscard]] const Color &base_color() const;
    
    void set_base_color(const Color &new_value);
    
    /**
     * Base texture.
     */
    [[nodiscard]] std::shared_ptr<Image> base_texture() const;
    
    void set_base_texture(const std::shared_ptr<Image> &new_value);
    
    void set_base_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Normal texture.
     */
    [[nodiscard]] std::shared_ptr<Image> normal_texture() const;
    
    void set_normal_texture(const std::shared_ptr<Image> &new_value);
    
    void set_normal_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Normal texture intensity.
     */
    [[nodiscard]] float normal_texture_intensity() const;
    
    void set_normal_texture_intensity(float new_value);
    
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
     * Occlusion texture.
     */
    [[nodiscard]] std::shared_ptr<Image> occlusion_texture() const;
    
    void set_occlusion_texture(const std::shared_ptr<Image> &new_value);
    
    void set_occlusion_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Occlusion texture intensity.
     */
    [[nodiscard]] float occlusion_texture_intensity() const;
    
    void set_occlusion_texture_intensity(float new_value);
    
protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PbrBaseMaterial(Device &device, const std::string &name);
    
private:
    PBRBaseData pbr_base_data_;
    const std::string pbr_base_prop_;
    
    std::shared_ptr<Image> base_texture_{nullptr};
    const std::string base_texture_prop_;
    
    std::shared_ptr<Image> normal_texture_{nullptr};
    const std::string normal_texture_prop_;
    
    std::shared_ptr<Image> emissive_texture_{nullptr};
    const std::string emissive_texture_prop_;
    
    std::shared_ptr<Image> occlusion_texture_{nullptr};
    const std::string occlusion_texture_prop_;
};

}
