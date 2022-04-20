//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "base_material.h"
#include "color.h"
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
    [[nodiscard]] const Color &base_color() const;
    
    void set_base_color(const Color &new_value);
    
    /**
     * Base texture.
     */
    [[nodiscard]] std::shared_ptr<Image> base_texture() const;
    
    void set_base_texture(const std::shared_ptr<Image> &new_value);
    
    void set_base_texture(const std::shared_ptr<Image> &new_value, const VkSamplerCreateInfo &info);
    
    /**
     * Create a unlit material instance.
     */
    UnlitMaterial(Device &device, const std::string &name = "");
    
private:
    Color base_color_ = Color(1, 1, 1, 1);
    const std::string base_color_prop_;
    
    std::shared_ptr<Image> base_texture_{nullptr};
    const std::string base_texture_prop_;
};

}
