//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simulator/cloth/cloth_renderer.h"

#include "vox.cloth/foundation/PxStrideIterator.h"
#include "vox.render/entity.h"
#include "vox.render/mesh/buffer_mesh.h"
#include "vox.render/scene.h"
#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_common.h"

namespace vox::cloth {
namespace {
template <typename T>
void GatherIndices(std::vector<uint16_t> &indices,
                   const nv::cloth::BoundedData &triangles,
                   const nv::cloth::BoundedData &quads) {
    physx::PxStrideIterator<const T> t_it, q_it;

    indices.reserve(triangles.count * 3 + quads.count * 6);

    t_it = physx::PxMakeIterator(reinterpret_cast<const T *>(triangles.data), triangles.stride);
    for (physx::PxU32 i = 0; i < triangles.count; ++i, ++t_it) {
        indices.push_back(static_cast<uint16_t>(t_it.ptr()[0]));
        indices.push_back(static_cast<uint16_t>(t_it.ptr()[1]));
        indices.push_back(static_cast<uint16_t>(t_it.ptr()[2]));
    }

    // Only do quads in case there wasn't triangle data provided
    //  otherwise we risk to render triangles double
    if (indices.empty()) {
        q_it = physx::PxMakeIterator(reinterpret_cast<const T *>(quads.data), quads.stride);
        for (physx::PxU32 i = 0; i < quads.count; ++i, ++q_it) {
            indices.push_back(static_cast<uint16_t>(q_it.ptr()[0]));
            indices.push_back(static_cast<uint16_t>(q_it.ptr()[1]));
            indices.push_back(static_cast<uint16_t>(q_it.ptr()[2]));
            indices.push_back(static_cast<uint16_t>(q_it.ptr()[0]));
            indices.push_back(static_cast<uint16_t>(q_it.ptr()[2]));
            indices.push_back(static_cast<uint16_t>(q_it.ptr()[3]));
        }
    }
}
}  // namespace

std::string ClothRenderer::name() { return "ClothRenderer"; }

ClothRenderer::ClothRenderer(Entity *entity) : Renderer(entity) {}

void ClothRenderer::Render(std::vector<RenderElement> &opaque_queue,
                           std::vector<RenderElement> &alpha_test_queue,
                           std::vector<RenderElement> &transparent_queue) {
    shader_data_.RemoveDefine(HAS_UV);
    shader_data_.RemoveDefine(HAS_NORMAL);
    shader_data_.RemoveDefine(HAS_TANGENT);
    shader_data_.RemoveDefine(HAS_VERTEXCOLOR);

    shader_data_.AddDefine(HAS_NORMAL);

    auto &sub_meshes = mesh_->SubMeshes();
    for (size_t i = 0; i < sub_meshes.size(); i++) {
        MaterialPtr material;
        if (i < materials_.size()) {
            material = materials_[i];
        } else {
            material = nullptr;
        }
        if (material != nullptr) {
            RenderElement element(this, mesh_, &sub_meshes[i], material);
            PushPrimitive(element, opaque_queue, alpha_test_queue, transparent_queue);
        }
    }
}

void ClothRenderer::SetClothMeshDesc(const nv::cloth::ClothMeshDesc &desc) {
    uint32_t num_vertices = desc.points.count;
    vertices_.resize(num_vertices);

    physx::PxStrideIterator<const physx::PxVec3> p_it(reinterpret_cast<const physx::PxVec3 *>(desc.points.data),
                                                      desc.points.stride);
    for (physx::PxU32 i = 0; i < num_vertices; ++i) {
        vertices_[i].position = *p_it++;
        vertices_[i].normal = physx::PxVec3(0.f);
    }

    // build triangle indices
    if (desc.flags & nv::cloth::MeshFlag::e16_BIT_INDICES)
        GatherIndices<physx::PxU16>(indices_, desc.triangles, desc.quads);
    else
        GatherIndices<physx::PxU32>(indices_, desc.triangles, desc.quads);

    for (physx::PxU32 i = 0; i < indices_.size(); i += 3) {
        auto p_0 = vertices_[indices_[i]].position;
        auto p_1 = vertices_[indices_[i + 1]].position;
        auto p_2 = vertices_[indices_[i + 2]].position;

        auto normal = ((p_2 - p_0).cross(p_1 - p_0)).getNormalized();

        vertices_[indices_[i]].normal += normal;
        vertices_[indices_[i + 1]].normal += normal;
        vertices_[indices_[i + 2]].normal += normal;
    }

    for (physx::PxU32 i = 0; i < num_vertices; ++i) vertices_[i].normal.normalize();

    auto &vertex_input_attributes = vertex_input_state_.attributes;
    vertex_input_attributes.resize(2);
    vertex_input_attributes[0] = initializers::VertexInputAttributeDescription(0, (uint32_t)Attributes::POSITION,
                                                                               VK_FORMAT_R32G32B32_SFLOAT, 0);
    vertex_input_attributes[1] = initializers::VertexInputAttributeDescription(0, (uint32_t)Attributes::NORMAL,
                                                                               VK_FORMAT_R32G32B32_SFLOAT, 12);

    auto &vertex_input_bindings = vertex_input_state_.bindings;
    vertex_input_bindings.resize(1);
    vertex_input_bindings[0] = vox::initializers::VertexInputBindingDescription(0, 6 * 4, VK_VERTEX_INPUT_RATE_VERTEX);

    Initialize(vertices_.data(), (uint32_t)vertices_.size(), sizeof(Vertex), indices_.data(),
               (uint32_t)indices_.size() / 3);
}

void ClothRenderer::Initialize(
        void *vertices, uint32_t num_vertices, uint32_t vertex_size, uint16_t *faces, uint32_t num_faces) {
    num_vertices_ = num_vertices;
    vertex_size_ = vertex_size;
    num_faces_ = num_faces;

    auto mesh = std::make_shared<BufferMesh>();

    auto &device = entity_->Scene()->Device();
    auto &queue = device.GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0);

    // keep stage buffer alive until submit finish
    std::vector<core::Buffer> transient_buffers;
    auto &command_buffer = device.RequestCommandBuffer();

    command_buffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    core::Buffer stage_buffer{device, num_vertices * vertex_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VMA_MEMORY_USAGE_CPU_ONLY};

    stage_buffer.Update(vertices, num_vertices * vertex_size);

    vertex_buffers_ = std::make_unique<core::Buffer>(
            device, num_vertices * vertex_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

    command_buffer.CopyBuffer(stage_buffer, *vertex_buffers_, num_vertices * vertex_size);
    mesh->SetVertexBufferBinding(0, vertex_buffers_.get());
    transient_buffers.push_back(std::move(stage_buffer));

    {
        core::Buffer stage_buffer{device, num_faces * 3 * sizeof(uint16_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  VMA_MEMORY_USAGE_CPU_ONLY};

        stage_buffer.Update(faces, num_faces * 3 * sizeof(uint16_t));

        core::Buffer new_index_buffer{device, num_faces * 3 * sizeof(uint16_t),
                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                      VMA_MEMORY_USAGE_GPU_ONLY};

        command_buffer.CopyBuffer(stage_buffer, new_index_buffer, num_faces * 3 * sizeof(uint16_t));
        mesh->SetIndexBufferBinding(
                std::make_unique<vox::IndexBufferBinding>(std::move(new_index_buffer), VK_INDEX_TYPE_UINT16));
        transient_buffers.push_back(std::move(stage_buffer));
    }
    command_buffer.End();

    queue.Submit(command_buffer, device.RequestFence());

    device.GetFencePool().wait();
    device.GetFencePool().reset();
    device.GetCommandPool().ResetPool();

    mesh->AddSubMesh(0, num_faces * 3);
    mesh->SetVertexInputState(vertex_input_state_.bindings, vertex_input_state_.attributes);
    mesh_ = mesh;
}

void ClothRenderer::Update(const physx::PxVec3 *positions, uint32_t num_vertices) {
    physx::PxStrideIterator<const physx::PxVec3> p_it(positions, sizeof(physx::PxVec3));
    Vertex *vertices = vertices_.data();
    const uint16_t *indices = indices_.data();
    for (physx::PxU32 i = 0; i < num_vertices; ++i) {
        vertices[i].position = *p_it++;
        vertices[i].normal = physx::PxVec3(0.f);
    }

    const auto kNumIndices = (physx::PxU32)indices_.size();
    for (physx::PxU32 i = 0; i < kNumIndices; i += 3) {
        const auto kP0 = vertices[indices[i]].position;
        const auto kP1 = vertices[indices[i + 1]].position;
        const auto kP2 = vertices[indices[i + 2]].position;

        const auto kNormal = ((kP2 - kP0).cross(kP1 - kP0)).getNormalized();

        vertices[indices[i]].normal += kNormal;
        vertices[indices[i + 1]].normal += kNormal;
        vertices[indices[i + 2]].normal += kNormal;
    }

    for (physx::PxU32 i = 0; i < num_vertices; ++i) vertices[i].normal.normalize();

    auto &device = entity_->Scene()->Device();
    auto &queue = device.GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0);

    // keep stage buffer alive until submit finish
    std::vector<core::Buffer> transient_buffers;
    auto &command_buffer = device.RequestCommandBuffer();

    command_buffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    core::Buffer stage_buffer{device, num_vertices * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VMA_MEMORY_USAGE_CPU_ONLY};

    stage_buffer.Update(vertices, num_vertices * sizeof(Vertex));

    command_buffer.CopyBuffer(stage_buffer, *vertex_buffers_, num_vertices * sizeof(Vertex));
    transient_buffers.push_back(std::move(stage_buffer));

    command_buffer.End();

    queue.Submit(command_buffer, device.RequestFence());

    device.GetFencePool().wait();
    device.GetFencePool().reset();
    device.GetCommandPool().ResetPool();
}

void ClothRenderer::UpdateBounds(BoundingBox3F &world_bounds) {
    world_bounds.lower_corner.x = -std::numeric_limits<float>::max();
    world_bounds.lower_corner.y = -std::numeric_limits<float>::max();
    world_bounds.lower_corner.z = -std::numeric_limits<float>::max();
    world_bounds.upper_corner.x = std::numeric_limits<float>::max();
    world_bounds.upper_corner.y = std::numeric_limits<float>::max();
    world_bounds.upper_corner.z = std::numeric_limits<float>::max();
}

// MARK: - Reflection
void ClothRenderer::OnSerialize(nlohmann::json &data) {}

void ClothRenderer::OnDeserialize(const nlohmann::json &data) {}

void ClothRenderer::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox::cloth
