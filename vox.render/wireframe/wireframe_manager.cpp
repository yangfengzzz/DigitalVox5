//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/wireframe/wireframe_manager.h"

#include "vox.render/entity.h"
#include "vox.render/material/base_material.h"
#include "vox.render/mesh/mesh_manager.h"
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
    material_->vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/wireframe.vert");
    material_->fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/wireframe.frag");
    material_->input_assembly_state_.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    
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
        auto mesh = MeshManager::GetSingleton().LoadBufferMesh();
        mesh->SetVertexInputState(vertex_input_state_.bindings, vertex_input_state_.attributes);
        mesh->AddSubMesh(0, static_cast<uint32_t>(lines_.indices.size()));
        lines_.renderer->SetMesh(mesh);

        auto vertex_byte_length = static_cast<uint32_t>(lines_.vertex.size() * sizeof(RenderDebugVertex));
        auto &device = entity_->Scene()->Device();
        auto &queue = device.GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0);

        // keep stage buffer alive until submit finish
        std::vector<core::Buffer> transient_buffers;
        auto &command_buffer = device.RequestCommandBuffer();

        command_buffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        core::Buffer stage_buffer{device, vertex_byte_length, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  VMA_MEMORY_USAGE_CPU_ONLY};
        stage_buffer.Update(lines_.vertex.data(), vertex_byte_length);
        if (lines_.vertex_buffer == nullptr || lines_.vertex_buffer->GetSize() != vertex_byte_length) {
            lines_.vertex_buffer = std::make_unique<core::Buffer>(
                    device, vertex_byte_length, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    VMA_MEMORY_USAGE_GPU_ONLY);
        }
        command_buffer.CopyBuffer(stage_buffer, *lines_.vertex_buffer, vertex_byte_length);
        mesh->SetVertexBufferBinding(0, lines_.vertex_buffer.get());
        transient_buffers.push_back(std::move(stage_buffer));

        {
            auto indices_size = lines_.indices.size() * sizeof(uint32_t);
            core::Buffer stage_buffer{device, indices_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VMA_MEMORY_USAGE_CPU_ONLY};
            stage_buffer.Update(lines_.indices.data(), indices_size);
            core::Buffer new_index_buffer{device, indices_size,
                                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                          VMA_MEMORY_USAGE_GPU_ONLY};
            command_buffer.CopyBuffer(stage_buffer, new_index_buffer, indices_size);
            mesh->SetIndexBufferBinding(
                    std::make_unique<vox::IndexBufferBinding>(std::move(new_index_buffer), VK_INDEX_TYPE_UINT32));
            transient_buffers.push_back(std::move(stage_buffer));
        }
        command_buffer.End();
        queue.Submit(command_buffer, device.RequestFence());
        device.GetFencePool().wait();
        device.GetFencePool().reset();
        device.GetCommandPool().ResetPool();
    } else {
        lines_.renderer->SetMesh(nullptr);
    }

    Clear();
}

}  // namespace vox
