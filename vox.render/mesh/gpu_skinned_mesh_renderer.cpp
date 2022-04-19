//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gpu_skinned_mesh_renderer.h"
#include "entity.h"
#include "scene.h"

namespace vox {
std::string GpuSkinnedMeshRenderer::name() {
    return "GPUSkinnedMeshRenderer";
}

GpuSkinnedMeshRenderer::GpuSkinnedMeshRenderer(Entity *entity) :
MeshRenderer(entity),
joint_matrix_property_("u_jointMatrix") {
}

GpuSkinnedMeshRenderer::SkinPtr GpuSkinnedMeshRenderer::skin() {
    return skin_;
}

void GpuSkinnedMeshRenderer::set_skin(const SkinPtr &skin) {
    skin_ = skin;
}

void GpuSkinnedMeshRenderer::update(float delta_time) {
    if (skin_) {
        if (!has_init_joints_) {
            init_joints();
            has_init_joints_ = true;
        }
        
        // Update join matrices
        auto m = entity()->transform_->world_matrix();
        auto inverse_transform = m.inverse();
        for (size_t i = 0; i < skin_->joints.size(); i++) {
            auto joint_node = skin_->joints[i];
            auto joint_mat = joint_node->transform_->world_matrix() * skin_->inverse_bind_matrices[i];
            joint_mat = inverse_transform * joint_mat;
            std::copy(joint_mat.data(), joint_mat.data() + 16, joint_matrix_.data() + i * 16);
        }
        shader_data_.set_data(joint_matrix_property_, joint_matrix_);
        shader_data_.add_define("JOINTS_COUNT" + std::to_string(skin_->joints.size()));
    }
}

void GpuSkinnedMeshRenderer::init_joints() {
    joint_matrix_.resize(skin_->joints.size() * 16);
    shader_data_.add_define("HAS_SKIN");
}

//MARK: - Reflection
void GpuSkinnedMeshRenderer::on_serialize(nlohmann::json &data) {
    
}

void GpuSkinnedMeshRenderer::on_deserialize(const nlohmann::json &data) {
    
}

void GpuSkinnedMeshRenderer::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
