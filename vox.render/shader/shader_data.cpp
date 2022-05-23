//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/shader_data.h"

namespace vox {
ShaderData::ShaderData(Device &device) : device_(device) {}

void ShaderData::BindData(CommandBuffer &command_buffer, DescriptorSetLayout &descriptor_set_layout) {
    for (auto &allocation : shader_buffer_pools_) {
        if (auto layout_binding = descriptor_set_layout.GetLayoutBinding(allocation.first)) {
            command_buffer.BindBuffer(allocation.second.GetBuffer(), allocation.second.GetOffset(),
                                      allocation.second.GetSize(), 0, layout_binding->binding, 0);
        }
    }

    for (auto &buffer : shader_buffers_) {
        if (auto layout_binding = descriptor_set_layout.GetLayoutBinding(buffer.first)) {
            command_buffer.BindBuffer(*buffer.second, 0, buffer.second->GetSize(), 0, layout_binding->binding, 0);
        }
    }

    for (auto &buffer : shader_buffer_functors_) {
        if (auto layout_binding = descriptor_set_layout.GetLayoutBinding(buffer.first)) {
            auto buffer_ptr = buffer.second();
            command_buffer.BindBuffer(*buffer_ptr, 0, buffer_ptr->GetSize(), 0, layout_binding->binding, 0);
        }
    }

    for (auto &texture : sampled_textures_) {
        if (auto layout_binding = descriptor_set_layout.GetLayoutBinding(texture.first)) {
            command_buffer.BindImage(texture.second->GetImageView(), *texture.second->GetSampler(), 0,
                                     layout_binding->binding, 0);
        }
    }

    for (auto &texture : storage_textures_) {
        if (auto layout_binding = descriptor_set_layout.GetLayoutBinding(texture.first)) {
            command_buffer.BindImage(texture.second->GetImageView(), 0, layout_binding->binding, 0);
        }
    }
}

void ShaderData::SetData(const std::string &property_name, BufferAllocation &&value) {
    shader_buffer_pools_.insert(std::make_pair(property_name, std::move(value)));
}

void ShaderData::SetBufferFunctor(const std::string &property_name, const std::function<core::Buffer *()> &functor) {
    shader_buffer_functors_.insert(std::make_pair(property_name, functor));
}

void ShaderData::SetSampledTexture(const std::string &texture_name,
                                   const core::ImageView &image_view,
                                   core::Sampler *sampler) {
    auto iter = sampled_textures_.find(texture_name);
    if (iter == sampled_textures_.end()) {
        sampled_textures_.insert(
                std::make_pair(texture_name, std::make_unique<core::SampledImage>(image_view, sampler)));
    } else {
        iter->second = std::make_unique<core::SampledImage>(image_view, sampler);
    }
}

void ShaderData::SetStorageTexture(const std::string &texture_name, const core::ImageView &image_view) {
    auto iter = storage_textures_.find(texture_name);
    if (iter == storage_textures_.end()) {
        storage_textures_.insert(
                std::make_pair(texture_name, std::make_unique<core::SampledImage>(image_view, nullptr)));
    } else {
        iter->second = std::make_unique<core::SampledImage>(image_view, nullptr);
    }
}

void ShaderData::AddDefine(const std::string &def) { variant_.AddDefine(def); }

void ShaderData::RemoveDefine(const std::string &undef) { variant_.RemoveDefine(undef); }

void ShaderData::MergeVariants(const ShaderVariant &variant, ShaderVariant &result) const {
    ShaderVariant::UnionCollection(variant, variant_, result);
}

}  // namespace vox
