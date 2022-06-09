//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/wireframe/wireframe_manager.h"

#include "vox.render/entity.h"
#include "vox.render/material/base_material.h"
#include "vox.render/mesh/buffer_mesh.h"
#include "vox.render/scene.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {

//-----------------------------------------------------------------------
WireframeManager *WireframeManager::GetSingletonPtr() { return ms_singleton; }

WireframeManager &WireframeManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

WireframeManager::WireframeManager(Entity *entity) : entity_(entity) {
    material_ = std::make_shared<BaseMaterial>(entity->Scene()->Device());
    material_->vertex_source_ = ShaderManager::GetSingleton().LoadShader("");
    material_->fragment_source_ = ShaderManager::GetSingleton().LoadShader("");

    auto &vertex_input_attributes = vertex_input_state_.attributes;
    vertex_input_attributes.resize(2);
    vertex_input_attributes[0] = initializers::VertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
    vertex_input_attributes[1] = initializers::VertexInputAttributeDescription(0, 1, VK_FORMAT_R32_UINT, 12);

    auto &vertex_input_bindings = vertex_input_state_.bindings;
    vertex_input_bindings.resize(1);
    vertex_input_bindings[0] = vox::initializers::VertexInputBindingDescription(0, 4 * 4, VK_VERTEX_INPUT_RATE_VERTEX);

    lines_.renderer = entity_->AddComponent<MeshRenderer>();
    lines_.renderer->SetMaterial(material_);
}

void WireframeManager::RenderDebugBuffer::Clear() {
    vertex.clear();
    indices.clear();
}

void WireframeManager::Clear() {
    points_.Clear();
    lines_.Clear();
    triangles_.Clear();
}

void WireframeManager::AddLine(const Vector3F &a, const Vector3F &b, uint32_t color) {
    lines_.vertex.push_back({a, color});
    lines_.indices.push_back(static_cast<uint32_t>(lines_.indices.size()));
    lines_.vertex.push_back({b, color});
    lines_.indices.push_back(static_cast<uint32_t>(lines_.indices.size()));
}

void WireframeManager::AddLine(const Matrix4x4F &t, const Vector3F &a, const Vector3F &b, uint32_t color) {
    lines_.vertex.push_back({t * a, color});
    lines_.indices.push_back(static_cast<uint32_t>(lines_.indices.size()));
    lines_.vertex.push_back({t * b, color});
    lines_.indices.push_back(static_cast<uint32_t>(lines_.indices.size()));
}

void WireframeManager::Flush() {
    if (!lines_.vertex.empty()) {
    }
}

}  // namespace vox
