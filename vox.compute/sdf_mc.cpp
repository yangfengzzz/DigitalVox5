//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/sdf_mc.h"

#include "vox.compute/marching_cubes_tables.h"
#include "vox.compute/sdf_grid.h"
#include "vox.render/shader/shader_manager.h"

namespace vox::compute {
SdfMarchingCube::SdfMarchingCube(Entity* entity) : Component(entity), m_max_marching_cubes_vertices_(128 * 1024) {}

void SdfMarchingCube::Initialize(const char* name, Device& device, RenderContext& render_context) {
    marching_cubes_pipeline_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    initialize_mc_vertices_pass_ = &marching_cubes_pipeline_->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));
    run_marching_cubes_on_sdf_pass_ = &marching_cubes_pipeline_->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));
}

void SdfMarchingCube::Update(CommandBuffer& command_buffer, RenderTarget& render_target) {
    m_origin_ = {m_p_sdf_->GetGridOrigin().x, m_p_sdf_->GetGridOrigin().y, m_p_sdf_->GetGridOrigin().z};
    m_cell_size_ = m_p_sdf_->GetGridCellSize();
    m_p_sdf_->GetGridNumCells(m_num_cells_x_, m_num_cells_y_, m_num_cells_z_);
    m_num_total_cells_ = m_p_sdf_->GetGridNumTotalCells();

    // Set the constant buffer parameters
    MarchingCubesUniformBuffer& constBuffer = m_uniform_buffer_data_;
    constBuffer.g_max_marching_cubes_vertices = m_max_marching_cubes_vertices_;
    constBuffer.g_marching_cubes_iso_level = m_cell_size_ * m_sdf_iso_level_;

    // Set the constant buffer parameters
    constBuffer.g_origin = {m_origin_.x, m_origin_.y, m_origin_.z, 0.0f};
    constBuffer.g_cell_size = m_cell_size_;
    constBuffer.g_num_cells_x = m_num_cells_x_;
    constBuffer.g_num_cells_y = m_num_cells_y_;
    constBuffer.g_num_cells_z = m_num_cells_z_;

    // Run InitializeMCVertices. One thread per each cell
    {
        auto num_dispatch_size =
                (uint32_t)std::ceil((float)m_max_marching_cubes_vertices_ / (float)SIM_THREAD_GROUP_SIZE);
        initialize_mc_vertices_pass_->SetDispatchSize({num_dispatch_size, 1, 1});
    }
    // Run RunMarchingCubesOnSdf. One thread per each cell
    {
        auto num_dispatch_size = (uint32_t)std::ceil((float)m_num_total_cells_ / (float)SIM_THREAD_GROUP_SIZE);
        run_marching_cubes_on_sdf_pass_->SetDispatchSize({num_dispatch_size, 1, 1});
    }
    marching_cubes_pipeline_->Draw(command_buffer, render_target);
}

}  // namespace vox::compute
