//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "shader_data_group.h"
#include <string>
#include <unordered_map>
#include <optional>

namespace vox {
/**
 * Shader property.
 */
struct ShaderProperty {
    /** Shader property name. */
    const std::string name;
    
    const ShaderDataGroup group;
    
    const uint32_t unique_id;
    
    ShaderProperty(std::string name, ShaderDataGroup group);
    
public:
    /**
     * Get shader property by name.
     * @param name - Name of the shader property
     * @returns Shader property
     */
    static std::optional<ShaderProperty> get_property_by_name(const std::string &name);
    
    static std::optional<ShaderDataGroup> get_shader_property_group(const std::string &property_name);
    
    /**
     * Create shader property by name.
     * @param name - Name of the shader property
     * @param group - Group of shader data
     * @returns Shader property
     */
    static ShaderProperty create(const std::string &name, ShaderDataGroup group);
    
private:
    static std::unordered_map<std::string, ShaderProperty> property_name_map;
};

}
