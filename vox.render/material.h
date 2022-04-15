//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "vector3.h"
#include "error.h"

namespace vox::sg {
class Texture;

/**
 * @brief How the alpha value of the main factor and texture should be interpreted
 */
enum class AlphaMode {
    /// Alpha value is ignored
    Opaque,
    /// Either full opaque or fully transparent
    Mask,
    /// Output is combined with the background
    Blend
};

class Material {
public:
    std::string name;
    
    explicit Material(std::string name);
    
    Material(Material &&other) = default;
    
    virtual ~Material() = default;
    
    std::unordered_map<std::string, Texture *> textures;
    
    /// Emissive color of the material
    Vector3F emissive{0.0f, 0.0f, 0.0f};
    
    /// Whether the material is double sided
    bool double_sided{false};
    
    /// Cutoff threshold when in Mask mode
    float alpha_cutoff{0.5f};
    
    /// Alpha rendering mode
    AlphaMode alpha_mode{AlphaMode::Opaque};
};

}        // namespace vox
