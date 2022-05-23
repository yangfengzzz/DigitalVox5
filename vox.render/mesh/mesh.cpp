//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/mesh.h"

namespace vox {
uint32_t Mesh::InstanceCount() const { return instance_count_; }

void Mesh::SetInstanceCount(uint32_t value) { instance_count_ = value; }

const SubMesh *Mesh::FirstSubMesh() const {
    if (!sub_meshes_.empty()) {
        return &sub_meshes_[0];
    }
    return nullptr;
}

const std::vector<SubMesh> &Mesh::SubMeshes() const { return sub_meshes_; }

void Mesh::AddSubMesh(SubMesh sub_mesh) { sub_meshes_.push_back(sub_mesh); }

void Mesh::AddSubMesh(uint32_t start, uint32_t count) { sub_meshes_.emplace_back(start, count); }

void Mesh::ClearSubMesh() { sub_meshes_.clear(); }

std::unique_ptr<UpdateFlag> Mesh::RegisterUpdateFlag() { return update_flag_manager_.Registration(); }

void Mesh::SetVertexInputState(const std::vector<VkVertexInputBindingDescription> &vertex_input_bindings,
                               const std::vector<VkVertexInputAttributeDescription> &vertex_input_attributes) {
    vertex_input_state_.bindings = vertex_input_bindings;
    vertex_input_state_.attributes = vertex_input_attributes;
}

const VertexInputState &Mesh::VertexInputState() const { return vertex_input_state_; }

void Mesh::SetIndexBufferBinding(std::unique_ptr<vox::IndexBufferBinding> &&binding) {
    index_buffer_binding_ = std::move(binding);
}

const IndexBufferBinding *Mesh::IndexBufferBinding() const {
    return index_buffer_binding_ ? index_buffer_binding_.get() : nullptr;
}

}  // namespace vox
