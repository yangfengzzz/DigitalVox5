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
#include "vector4.h"
#include "error.h"
#include "material.h"

namespace vox::sg {
class PBRMaterial : public Material {
public:
    explicit PBRMaterial(const std::string &name);
    
    ~PBRMaterial() override = default;
    
    Vector4F base_color_factor_{0.0f, 0.0f, 0.0f, 0.0f};
    
    float metallic_factor_{0.0f};
    
    float roughness_factor_{0.0f};
};

}        // namespace vox
