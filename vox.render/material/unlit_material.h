//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.render/material/base_material.h"
#include "vox.render/texture.h"

namespace vox {
/**
 * Unlit Material.
 */
class UnlitMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    [[nodiscard]] const Color &BaseColor() const;

    void SetBaseColor(const Color &new_value);

    /**
     * Base texture.
     */
    [[nodiscard]] std::shared_ptr<Texture> BaseTexture() const;

    void SetBaseTexture(const std::shared_ptr<Texture> &new_value);

    void SetBaseTexture(const std::shared_ptr<Texture> &new_value, const VkSamplerCreateInfo &info);

    /**
     * Create a unlit material instance.
     */
    UnlitMaterial(Device &device, const std::string &name = "");

private:
    Color base_color_ = Color(1, 1, 1, 1);
    const std::string base_color_prop_;

    std::shared_ptr<Texture> base_texture_{nullptr};
    const std::string base_texture_prop_;
};

}  // namespace vox
