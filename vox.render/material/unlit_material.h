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
 * Unlit Material.
 */
class UnlitMaterial : public BaseMaterial {
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
    
    void set_base_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo& info);
    
    /**
     * Tiling and offset of main textures.
     */
    [[nodiscard]] Vector4F tiling_offset() const;
    
    void set_tiling_offset(const Vector4F &new_value);
    
    /**
     * Create a unlit material instance.
     */
    explicit UnlitMaterial(Device& device, const std::string &name);
    
private:
    Color base_color_ = Color(1, 1, 1, 1);
    ShaderProperty base_color_prop_;
    
    std::shared_ptr<Image> base_texture_{nullptr};
    ShaderProperty base_texture_prop_;
    
    Vector4F tiling_offset_ = Vector4F(1, 1, 0, 0);
    ShaderProperty tiling_offset_prop_;
};

}
