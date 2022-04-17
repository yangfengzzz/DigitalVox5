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

void ShaderData::set_texture(const std::string &texture_name,
                             const std::shared_ptr<Image> &image,
                             core::Sampler *sampler) {
    auto property = ShaderProperty::get_property_by_name(texture_name);
    if (property.has_value()) {
        set_texture(property.value(), image, sampler);
    } else {
        assert(false && "can't find property");
    }
}

void ShaderData::set_texture(const ShaderProperty &texture_prop,
                             const std::shared_ptr<Image> &image,
                             core::Sampler *sampler) {
    auto iter = shader_textures_.find(texture_prop.unique_id);
    if (iter == shader_textures_.end()) {
        shader_textures_.insert(std::make_pair(texture_prop.unique_id,
                                               std::make_unique<core::SampledImage>(image->get_vk_image_view(),
                                                                                    sampler)));
    } else {
        iter->second = std::make_unique<core::SampledImage>(image->get_vk_image_view(), sampler);
    }
}

core::SampledImage *ShaderData::get_sampled_image(const std::string &property_name) {
    auto property = ShaderProperty::get_property_by_name(property_name);
    if (property.has_value()) {
        return get_sampled_image(property.value());
    }
    return nullptr;
}

core::SampledImage *ShaderData::get_sampled_image(const ShaderProperty &property) {
    auto iter = shader_textures_.find(property.unique_id);
    if (iter != shader_textures_.end()) {
        return iter->second.get();
    }
    return nullptr;
}

void ShaderData::add_define(const std::string &def) {
    variant_.add_define(def);
}

void ShaderData::add_undefine(const std::string &undef) {
    variant_.add_undefine(undef);
}

}
