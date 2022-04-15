//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sub_mesh.h"

#include <utility>

#include "material.h"

namespace vox::sg {
SubMesh::SubMesh(std::string name) :
name_{std::move(name)} {}

void SubMesh::set_attribute(const std::string &attribute_name, const VertexAttribute &attribute) {
    vertex_attributes_[attribute_name] = attribute;
    
    compute_shader_variant();
}

bool SubMesh::get_attribute(const std::string &attribute_name, VertexAttribute &attribute) const {
    auto attrib_it = vertex_attributes_.find(attribute_name);
    
    if (attrib_it == vertex_attributes_.end()) {
        return false;
    }
    
    attribute = attrib_it->second;
    
    return true;
}

void SubMesh::set_material(const Material &new_material) {
    material_ = &new_material;
    
    compute_shader_variant();
}

const Material *SubMesh::get_material() const {
    return material_;
}

const ShaderVariant &SubMesh::get_shader_variant() const {
    return shader_variant_;
}

void SubMesh::compute_shader_variant() {
    shader_variant_.clear();
    
    if (material_ != nullptr) {
        for (auto &texture : material_->textures_) {
            std::string tex_name = texture.first;
            std::transform(tex_name.begin(), tex_name.end(), tex_name.begin(), ::toupper);
            
            shader_variant_.add_define("HAS_" + tex_name);
        }
    }
    
    for (auto &attribute : vertex_attributes_) {
        std::string attrib_name = attribute.first;
        std::transform(attrib_name.begin(), attrib_name.end(), attrib_name.begin(), ::toupper);
        shader_variant_.add_define("HAS_" + attrib_name);
    }
}

ShaderVariant &SubMesh::get_mut_shader_variant() {
    return shader_variant_;
}

}        // namespace vox
