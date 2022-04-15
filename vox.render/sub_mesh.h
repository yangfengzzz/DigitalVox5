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

#include "vk_common.h"
#include "core/buffer.h"
#include "core/shader_module.h"

namespace vox::sg {
class Material;

struct VertexAttribute {
    VkFormat format = VK_FORMAT_UNDEFINED;
    
    std::uint32_t stride = 0;
    
    std::uint32_t offset = 0;
};

class SubMesh {
public:
    std::string name_{};
    
    explicit SubMesh(std::string name = {});
    
    virtual ~SubMesh() = default;
    
    VkIndexType index_type_{};
    
    std::uint32_t index_offset_ = 0;
    
    std::uint32_t vertices_count_ = 0;
    
    std::uint32_t vertex_indices_ = 0;
    
    std::unordered_map<std::string, core::Buffer> vertex_buffers_;
    
    std::unique_ptr<core::Buffer> index_buffer_;
    
    void set_attribute(const std::string &name, const VertexAttribute &attribute);
    
    bool get_attribute(const std::string &name, VertexAttribute &attribute) const;
    
    void set_material(const Material &material);
    
    [[nodiscard]] const Material *get_material() const;
    
    [[nodiscard]] const ShaderVariant &get_shader_variant() const;
    
    ShaderVariant &get_mut_shader_variant();
    
private:
    std::unordered_map<std::string, VertexAttribute> vertex_attributes_;
    
    const Material *material_{nullptr};
    
    ShaderVariant shader_variant_;
    
    void compute_shader_variant();
};

}        // namespace vox
