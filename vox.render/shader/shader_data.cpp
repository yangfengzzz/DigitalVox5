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

void ShaderData::bind_data(CommandBuffer &command_buffer, DescriptorSetLayout &descriptor_set_layout) {
    for (auto &buffer : shader_buffers_) {
        if (auto layout_binding = descriptor_set_layout.get_layout_binding(buffer.first)) {
            command_buffer.bind_buffer(*buffer.second, 0, buffer.second->get_size(), 0, layout_binding->binding, 0);
        }
    }
    
    for (auto &buffer : shader_buffer_functors_) {
        if (auto layout_binding = descriptor_set_layout.get_layout_binding(buffer.first)) {
            auto buffer_ptr = buffer.second();
            command_buffer.bind_buffer(*buffer_ptr, 0, buffer_ptr->get_size(), 0, layout_binding->binding, 0);
        }
    }
    
    for (auto &texture : sampled_textures_) {
        if (auto layout_binding = descriptor_set_layout.get_layout_binding(texture.first)) {
            command_buffer.bind_image(texture.second->get_image_view(),
                                      *texture.second->get_sampler(),
                                      0, layout_binding->binding, 0);
        }
    }
    
    for (auto &texture : storage_textures_) {
        if (auto layout_binding = descriptor_set_layout.get_layout_binding(texture.first)) {
            command_buffer.bind_image(texture.second->get_image_view(),
                                      0, layout_binding->binding, 0);
        }
    }
}

void ShaderData::set_buffer_functor(const std::string &property_name,
                                    const std::function<core::Buffer *()> &functor) {
    shader_buffer_functors_.insert(std::make_pair(property_name, functor));
}

void ShaderData::set_sampled_texture(const std::string &texture_name,
                                     const core::ImageView &image_view,
                                     core::Sampler *sampler) {
    auto iter = sampled_textures_.find(texture_name);
    if (iter == sampled_textures_.end()) {
        sampled_textures_.insert(std::make_pair(texture_name,
                                                std::make_unique<core::SampledImage>(image_view,
                                                                                     sampler)));
    } else {
        iter->second = std::make_unique<core::SampledImage>(image_view, sampler);
    }
}

void ShaderData::set_storage_texture(const std::string &texture_name,
                                     const core::ImageView &image_view) {
    auto iter = storage_textures_.find(texture_name);
    if (iter == storage_textures_.end()) {
        storage_textures_.insert(std::make_pair(texture_name,
                                                std::make_unique<core::SampledImage>(image_view, nullptr)));
    } else {
        iter->second = std::make_unique<core::SampledImage>(image_view, nullptr);
    }
}

void ShaderData::add_define(const std::string &def) {
    variant_.add_define(def);
}

void ShaderData::remove_define(const std::string &undef) {
    variant_.remove_define(undef);
}

void ShaderData::merge_variants(const ShaderVariant &variant, ShaderVariant &result) const {
    ShaderVariant::union_collection(variant, variant_, result);
}


}
