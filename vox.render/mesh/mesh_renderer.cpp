//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/mesh_renderer.h"

#include "vox.render/entity.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_common.h"

namespace vox {
std::string MeshRenderer::name() { return "MeshRenderer"; }

MeshRenderer::MeshRenderer(Entity *entity) : Renderer(entity) {}

void MeshRenderer::SetMesh(const MeshPtr &mesh) {
    auto &last_mesh = mesh_;
    if (last_mesh != mesh) {
        if (last_mesh != nullptr) {
            mesh_update_flag_.reset();
        }
        if (mesh != nullptr) {
            mesh_update_flag_ = mesh->RegisterUpdateFlag();
        }
        mesh_ = mesh;
    }
}

MeshPtr MeshRenderer::Mesh() { return mesh_; }

void MeshRenderer::Render(std::vector<RenderElement> &opaque_queue,
                          std::vector<RenderElement> &alpha_test_queue,
                          std::vector<RenderElement> &transparent_queue) {
    if (mesh_ != nullptr) {
        if (mesh_update_flag_->flag_) {
            const auto &vertex_input_state = mesh_->VertexInputState();

            shader_data_.RemoveDefine(HAS_UV);
            shader_data_.RemoveDefine(HAS_NORMAL);
            shader_data_.RemoveDefine(HAS_TANGENT);
            shader_data_.RemoveDefine(HAS_VERTEXCOLOR);

            for (auto attribute : vertex_input_state.attributes) {
                if (attribute.location == (uint32_t)Attributes::UV_0) {
                    shader_data_.AddDefine(HAS_UV);
                }
                if (attribute.location == (uint32_t)Attributes::NORMAL) {
                    shader_data_.AddDefine(HAS_NORMAL);
                }
                if (attribute.location == (uint32_t)Attributes::TANGENT) {
                    shader_data_.AddDefine(HAS_TANGENT);
                }
                if (attribute.location == (uint32_t)Attributes::COLOR_0) {
                    shader_data_.AddDefine(HAS_VERTEXCOLOR);
                }
            }
            mesh_update_flag_->flag_ = false;
        }

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
}

void MeshRenderer::UpdateBounds(BoundingBox3F &world_bounds) {
    if (mesh_ != nullptr) {
        const auto kLocalBounds = mesh_->bounds_;
        const auto kWorldMatrix = entity_->transform->WorldMatrix();
        world_bounds = kLocalBounds.transform(kWorldMatrix);
    } else {
        world_bounds.lower_corner = Point3F(0, 0, 0);
        world_bounds.upper_corner = Point3F(0, 0, 0);
    }
}

// MARK: - Reflection
void MeshRenderer::OnSerialize(nlohmann::json &data) {}

void MeshRenderer::OnDeserialize(const nlohmann::json &data) {}

void MeshRenderer::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
