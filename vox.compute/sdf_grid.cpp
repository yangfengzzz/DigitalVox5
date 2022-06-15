//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/sdf_grid.h"

#include <utility>

#include "vox.render/shader/shader_manager.h"

namespace vox::compute {
void SdfCollisionSystem::Initialize(Device& device, RenderContext& render_context) {
    signed_distance_field_pipeline = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    initialize_signed_distance_field_pass = &signed_distance_field_pipeline->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));
    construct_signed_distance_field_pass = &signed_distance_field_pipeline->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));
    finalize_signed_distance_field_pass = &signed_distance_field_pipeline->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));

    collide_hair_vertices_with_sdf_pipeline = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    collide_hair_vertices_with_sdf_pass = &collide_hair_vertices_with_sdf_pipeline->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));
}

SdfGrid::SdfGrid(Device& device,
                 std::shared_ptr<Mesh> coll_mesh,
                 const char* model_name,
                 int num_cells_in_x,
                 float collision_margin)
    : m_collision_margin_(collision_margin),
      m_num_cells_in_x_axis_(num_cells_in_x),
      m_grid_allocation_multiplier_(1.4f),
      m_num_total_cells_(INT_MAX) {
    m_input_collision_mesh_ = std::move(coll_mesh);

    // initialize SDF grid using the associated model's bounding box
    auto lower_corner = m_input_collision_mesh_->bounds_.lower_corner;
    auto upper_corner = m_input_collision_mesh_->bounds_.upper_corner;
    m_constant_buffer_data_.cell_size = (upper_corner.x - lower_corner.x) / (float)m_num_cells_in_x_axis_;
    int num_extra_padding_cells = (int)(0.8f * (float)m_num_cells_in_x_axis_);
    m_padding_boundary_ = {(float)num_extra_padding_cells * m_constant_buffer_data_.cell_size,
                           (float)num_extra_padding_cells * m_constant_buffer_data_.cell_size,
                           (float)num_extra_padding_cells * m_constant_buffer_data_.cell_size};

    UpdateSdfGrid(lower_corner, upper_corner);

    lower_corner -= m_padding_boundary_;
    upper_corner += m_padding_boundary_;

    m_constant_buffer_data_.num_cells_x = (int)((upper_corner.x - lower_corner.x) / m_constant_buffer_data_.cell_size);
    m_constant_buffer_data_.num_cells_y = (int)((upper_corner.y - lower_corner.y) / m_constant_buffer_data_.cell_size);
    m_constant_buffer_data_.num_cells_z = (int)((upper_corner.z - lower_corner.z) / m_constant_buffer_data_.cell_size);
    m_num_total_cells_ = std::min(m_num_total_cells_,
                                  (int)((int)m_grid_allocation_multiplier_ * m_constant_buffer_data_.num_cells_x *
                                        m_constant_buffer_data_.num_cells_y * m_constant_buffer_data_.num_cells_z));

    m_constant_buffer_ = std::make_unique<core::Buffer>(
            device, sizeof(SDFGridParams), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    m_signed_distance_field_ =
            std::make_unique<core::Buffer>(device, sizeof(int) * m_num_total_cells_,
                                           VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
}

void SdfGrid::UpdateSdfGrid(const Point3F& tight_bbox_min, const Point3F& tight_bbox_max) {
    m_constant_buffer_data_.origin = tight_bbox_min - m_padding_boundary_;
}

void SdfGrid::Update(CommandBuffer& command_buffer, RenderTarget& render_target, SdfCollisionSystem& system) {
    if (!m_input_collision_mesh_) return;

    // Update the grid data based on the current bounding box
    auto lower_corner = m_input_collision_mesh_->bounds_.lower_corner;
    auto upper_corner = m_input_collision_mesh_->bounds_.upper_corner;
    UpdateSdfGrid(lower_corner, upper_corner);
    m_constant_buffer_->Update(&m_constant_buffer_data_, sizeof(SDFGridParams));

    // Run InitializeSignedDistanceField. One thread per one cell.
    {
        uint32_t num_dispatch_size = (int)std::ceil((float)m_num_total_cells_ / (float)SIM_THREAD_GROUP_SIZE);
        system.initialize_signed_distance_field_pass->SetDispatchSize({num_dispatch_size, 1, 1});
    }
    // Run ConstructSignedDistanceField. One thread per each triangle
    {
        uint32_t num_dispatch_size = (int)std::ceil((float)m_input_collision_mesh_->SubMeshes()[0].Count() / 3 /
                                                    (float)SIM_THREAD_GROUP_SIZE);
        system.construct_signed_distance_field_pass->SetDispatchSize({num_dispatch_size, 1, 1});
    }
    // Run FinalizeSignedDistanceField. One thread per each triangle
    {
        uint32_t num_dispatch_size = (int)std::ceil((float)m_num_total_cells_ / (float)SIM_THREAD_GROUP_SIZE);
        system.finalize_signed_distance_field_pass->SetDispatchSize({num_dispatch_size, 1, 1});
    }
    system.signed_distance_field_pipeline->Draw(command_buffer, render_target);
}

}  // namespace vox::compute
