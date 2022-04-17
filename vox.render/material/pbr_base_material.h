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
    /**
     * Base color.
     */
    [[nodiscard]] Color base_color() const;
    
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
    [[nodiscard]] Color emissive_color() const;
    
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
    
    /**
     * Tiling and offset of main textures.
     */
    [[nodiscard]] Vector4F tiling_offset() const;
    
    void set_tiling_offset(const Vector4F &new_value);
    
protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PbrBaseMaterial(Device &device, const std::string &name);
    
private:
    Vector4F tiling_offset_ = Vector4F(1, 1, 0, 0);
    ShaderProperty tiling_offset_prop_;
    
    float normal_texture_intensity_ = 1.f;
    ShaderProperty normal_texture_intensity_prop_;
    
    float occlusion_texture_intensity_ = 1.f;
    ShaderProperty occlusion_texture_intensity_prop_;
    
    Color base_color_ = Color(1, 1, 1, 1);
    ShaderProperty base_color_prop_;
    
    Color emissive_color_ = Color(0, 0, 0, 1);
    ShaderProperty emissive_color_prop_;
    
    std::shared_ptr<Image> base_texture_{nullptr};
    ShaderProperty base_texture_prop_;
    
    std::shared_ptr<Image> normal_texture_{nullptr};
    ShaderProperty normal_texture_prop_;
    
    std::shared_ptr<Image> emissive_texture_{nullptr};
    ShaderProperty emissive_texture_prop_;
    
    std::shared_ptr<Image> occlusion_texture_{nullptr};
    ShaderProperty occlusion_texture_prop_;
};

}
