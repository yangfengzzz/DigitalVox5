//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_data.h"

namespace vox {
ShaderData::ShaderData(Device &device) :
device_(device) {
}

core::Buffer *ShaderData::get_data(const std::string &property_name) {
    auto property = ShaderProperty::get_property_by_name(property_name);
    if (property.has_value()) {
        return get_data(property.value().unique_id);
    }
    return nullptr;
}

core::Buffer *ShaderData::get_data(const ShaderProperty &property) {
    return get_data(property.unique_id);
}

core::Buffer *ShaderData::get_data(uint32_t unique_id) {
    auto iter = shader_buffers_.find(unique_id);
    if (iter != shader_buffers_.end()) {
        return iter->second.get();
    }
    
    auto functor_iter = shader_buffer_functors_.find(unique_id);
    if (functor_iter != shader_buffer_functors_.end()) {
        return functor_iter->second();
    }
    
    return nullptr;
}

void ShaderData::set_buffer_functor(const std::string &property_name,
                                    const std::function<core::Buffer *()> &functor) {
    auto property = ShaderProperty::get_property_by_name(property_name);
    if (property.has_value()) {
        set_buffer_functor(property.value(), functor);
    } else {
        assert(false && "can't find property");
    }
}

void ShaderData::set_buffer_functor(const ShaderProperty &property,
                                    const std::function<core::Buffer *()> &functor) {
    shader_buffer_functors_.insert(std::make_pair(property.unique_id, functor));
}

void ShaderData::add_define(const std::string &def) {
    variant_.add_define(def);
}

void ShaderData::add_undefine(const std::string &undef) {
    variant_.add_undefine(undef);
}

}
