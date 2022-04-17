//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "shader_property.h"
#include "core/buffer.h"
#include "../image.h"

namespace vox {
/**
 * Shader data collection,Correspondence includes shader properties data and macros data.
 */
class ShaderData {
public:
    explicit ShaderData(Device &device);
    
    core::Buffer *get_data(const std::string &property_name);
    
    core::Buffer *get_data(const ShaderProperty &property);
    
    core::Buffer *get_data(uint32_t unique_id);
    
    void set_buffer_functor(const std::string &property_name,
                            const std::function<core::Buffer *()> &functor);
    
    void set_buffer_functor(const ShaderProperty &property,
                            const std::function<core::Buffer *()> &functor);
    
    template<typename T>
    void set_data(const std::string &property_name, const T &value) {
        auto property = ShaderProperty::get_property_by_name(property_name);
        if (property.has_value()) {
            set_data(property.value(), value);
        } else {
            assert(false && "can't find property");
        }
    }
    
    template<typename T>
    void set_data(const ShaderProperty &property, const T &value) {
        auto iter = shader_buffers_.find(property.unique_id);
        if (iter == shader_buffers_.end()) {
            shader_buffers_.insert(std::make_pair(property.unique_id,
                                                  std::make_unique<core::Buffer>(device_, sizeof(T),
                                                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                                 VMA_MEMORY_USAGE_CPU_TO_GPU)));
        }
        iter = shader_buffers_.find(property.unique_id);
        iter->second->convert_and_update(value);
    }
    
    template<typename T>
    void set_data(const ShaderProperty &property, const std::vector<T> &value) {
        auto iter = shader_buffers_.find(property.unique_id);
        if (iter == shader_buffers_.end()) {
            shader_buffers_.insert(std::make_pair(property.unique_id,
                                                  std::make_unique<core::Buffer>(device_, sizeof(T) * value.size(),
                                                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                                 VMA_MEMORY_USAGE_CPU_TO_GPU)));
        }
        iter = shader_buffers_.find(property.unique_id);
        iter->second->update(value.data());
    }
    
    template<typename T, size_t N>
    void set_data(const ShaderProperty &property, const std::array<T, N> &value) {
        auto iter = shader_buffers_.find(property.unique_id);
        if (iter == shader_buffers_.end()) {
            shader_buffers_.insert(std::make_pair(property.unique_id,
                                                  std::make_unique<core::Buffer>(device_, sizeof(T) * N,
                                                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                                 VMA_MEMORY_USAGE_CPU_TO_GPU)));
        }
        iter = shader_buffers_.find(property.unique_id);
        iter->second->update(value.data());
    }
    
private:
    Device &device_;
    std::unordered_map<uint32_t, std::unique_ptr<core::Buffer>> shader_buffers_{};
    std::unordered_map<uint32_t, std::function<core::Buffer *()>> shader_buffer_functors_{};
    std::unordered_map<uint32_t, std::shared_ptr<Image>> shader_textures_{};
};

}

