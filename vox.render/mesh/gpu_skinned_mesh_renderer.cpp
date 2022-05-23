//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/gpu_skinned_mesh_renderer.h"

#include "vox.render/entity.h"
#include "vox.render/scene.h"
#include "vox.render/shader/internal_variant_name.h"

namespace vox {
std::string GpuSkinnedMeshRenderer::name() { return "GPUSkinnedMeshRenderer"; }

GpuSkinnedMeshRenderer::GpuSkinnedMeshRenderer(Entity *entity)
    : MeshRenderer(entity), joint_matrix_property_("jointMatrix") {}

GpuSkinnedMeshRenderer::SkinPtr GpuSkinnedMeshRenderer::Skin() { return skin_; }

void GpuSkinnedMeshRenderer::SetSkin(const SkinPtr &skin) { skin_ = skin; }

void GpuSkinnedMeshRenderer::Update(float delta_time) {
    if (skin_) {
        if (!has_init_joints_) {
            InitJoints();
            has_init_joints_ = true;
        }

        // Update join matrices
        auto m = GetEntity()->transform->WorldMatrix();
        auto inverse_transform = m.inverse();
        for (size_t i = 0; i < skin_->joints.size(); i++) {
            auto joint_node = skin_->joints[i];
            auto joint_mat = joint_node->transform->WorldMatrix() * skin_->inverse_bind_matrices[i];
            joint_mat = inverse_transform * joint_mat;
            std::copy(joint_mat.data(), joint_mat.data() + 16, joint_matrix_.data() + i * 16);
        }
        shader_data_.SetData(joint_matrix_property_, joint_matrix_);
        shader_data_.AddDefine(JOINTS_COUNT + std::to_string(skin_->joints.size()));
    }
}

void GpuSkinnedMeshRenderer::InitJoints() {
    joint_matrix_.resize(skin_->joints.size() * 16);
    shader_data_.AddDefine(HAS_SKIN);
}

// MARK: - Reflection
void GpuSkinnedMeshRenderer::OnSerialize(nlohmann::json &data) {}

void GpuSkinnedMeshRenderer::OnDeserialize(const nlohmann::json &data) {}

void GpuSkinnedMeshRenderer::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
