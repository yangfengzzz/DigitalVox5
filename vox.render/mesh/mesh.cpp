//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh.h"

namespace vox {
uint32_t Mesh::instance_count() const {
    return instance_count_;
}

void Mesh::set_instance_count(uint32_t value) {
	instance_count_ = value;
}

const SubMesh *Mesh::sub_mesh() const {
    if (!sub_meshes_.empty()) {
        return &sub_meshes_[0];
    }
    return nullptr;
}

const std::vector<SubMesh> &Mesh::sub_meshes() const {
    return sub_meshes_;
}

void Mesh::add_sub_mesh(SubMesh sub_mesh) {
    sub_meshes_.push_back(sub_mesh);
}

void Mesh::add_sub_mesh(uint32_t start, uint32_t count) {
    sub_meshes_.emplace_back(start, count);
}

void Mesh::clear_sub_mesh() {
    sub_meshes_.clear();
}

std::unique_ptr<UpdateFlag> Mesh::register_update_flag() {
    return update_flag_manager_.registration();
}

void Mesh::set_vertex_input_state(const VkPipelineVertexInputStateCreateInfo &state) {
    vertex_input_state_ = state;
    update_flag_manager_.distribute();
}

void Mesh::set_vertex_buffer_binding(size_t index, std::unique_ptr<core::Buffer> &&binding) {
    if (vertex_buffer_bindings_.size() > index) {
        vertex_buffer_bindings_[index] = std::move(binding);
    } else {
        vertex_buffer_bindings_.insert(vertex_buffer_bindings_.begin() + index, std::move(binding));
    }
}

void Mesh::set_index_buffer_binding(std::unique_ptr<IndexBufferBinding> &&binding) {
    index_buffer_binding_ = std::move(binding);
}

const VkPipelineVertexInputStateCreateInfo &Mesh::vertex_input_state() const {
    return vertex_input_state_;
}

const std::vector<std::unique_ptr<core::Buffer>> &Mesh::vertex_buffer_bindings() const {
    return vertex_buffer_bindings_;
}

const IndexBufferBinding &Mesh::index_buffer_binding() const {
    return *index_buffer_binding_;
}

}
