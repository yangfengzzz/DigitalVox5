//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_property.h"
#include <utility>

namespace vox {
std::unordered_map<std::string, ShaderProperty> ShaderProperty::property_name_map = {};

ShaderProperty::ShaderProperty(std::string name, ShaderDataGroup group) :
name(std::move(name)),
group(group),
unique_id(static_cast<uint32_t>(property_name_map.size())) {
}

//MARK: - Property
ShaderProperty ShaderProperty::create(const std::string &name, ShaderDataGroup group) {
    auto iter = ShaderProperty::property_name_map.find(name);
    if (iter != ShaderProperty::property_name_map.end()) {
        return iter->second;
    } else {
        auto property = ShaderProperty(name, group);
        ShaderProperty::property_name_map.insert(std::make_pair(name, property));
        return property;
    }
}

std::optional<ShaderProperty> ShaderProperty::get_property_by_name(const std::string &name) {
    auto iter = ShaderProperty::property_name_map.find(name);
    if (iter != ShaderProperty::property_name_map.end()) {
        return iter->second;
    } else {
        return std::nullopt;
    }
}

std::optional<ShaderDataGroup> ShaderProperty::get_shader_property_group(const std::string &property_name) {
    auto iter = ShaderProperty::property_name_map.find(property_name);
    if (iter != ShaderProperty::property_name_map.end()) {
        return iter->second.group;
    } else {
        return std::nullopt;
    }
}

}
