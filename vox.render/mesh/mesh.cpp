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

void Mesh::set_vertex_input_state(const std::vector<VkVertexInputBindingDescription> &vertex_input_bindings,
                                  const std::vector<VkVertexInputAttributeDescription> &vertex_input_attributes) {
    vertex_input_state_.bindings = vertex_input_bindings;
    vertex_input_state_.attributes = vertex_input_attributes;
}

const VertexInputState &Mesh::vertex_input_state() const {
    return vertex_input_state_;
}

void Mesh::set_index_buffer_binding(std::unique_ptr<IndexBufferBinding> &&binding) {
    index_buffer_binding_ = std::move(binding);
}

const IndexBufferBinding *Mesh::index_buffer_binding() const {
    return index_buffer_binding_ ? index_buffer_binding_.get() : nullptr;
}

}
