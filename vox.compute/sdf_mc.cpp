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
SdfMarchingCube::SdfMarchingCube(Entity* entity) : Component(entity) {}

void SdfMarchingCube::Initialize(const char* name, Device& device, RenderContext& render_context) {
    const int kElementsEdgeTableLength = 256 * sizeof(int);
    const int kElementsTriTableLength = 256 * 16 * sizeof(int);
    uniform_buffer_ = std::make_unique<core::Buffer>(device, sizeof(MarchingCubesUniformBuffer),
                                                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    mc_triangle_vertices_buffer_ =
            std::make_unique<core::Buffer>(device, sizeof(VertexData) * m_max_marching_cubes_vertices_,
                                           VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    num_mc_vertices_ = std::make_unique<core::Buffer>(device, sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                      VMA_MEMORY_USAGE_GPU_ONLY);
    mc_edge_table_ = std::make_unique<core::Buffer>(device, kElementsEdgeTableLength,
                                                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    mc_triangle_table_ = std::make_unique<core::Buffer>(device, kElementsTriTableLength,
                                                        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    {
        // keep stage buffer alive until submit finish
        std::vector<core::Buffer> transient_buffers;
        auto& queue = device.GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0);
        auto& command_buffer = device.RequestCommandBuffer();
        command_buffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        {
            core::Buffer stage_buffer{device, kElementsEdgeTableLength, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VMA_MEMORY_USAGE_CPU_ONLY};
            stage_buffer.Update((void*)MARCHING_CUBES_EDGE_TABLE, kElementsEdgeTableLength);
            command_buffer.CopyBuffer(stage_buffer, *mc_edge_table_, kElementsEdgeTableLength);
            transient_buffers.push_back(std::move(stage_buffer));
        }
        {
            core::Buffer stage_buffer{device, kElementsTriTableLength, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VMA_MEMORY_USAGE_CPU_ONLY};
            stage_buffer.Update((void*)MARCHING_CUBES_TRIANGLE_TABLE, kElementsTriTableLength);
            command_buffer.CopyBuffer(stage_buffer, *mc_triangle_table_, kElementsTriTableLength);
            transient_buffers.push_back(std::move(stage_buffer));
        }
        command_buffer.End();
        queue.Submit(command_buffer, device.RequestFence());
        device.GetFencePool().wait();
        device.GetFencePool().reset();
        device.GetCommandPool().ResetPool();
    }

    marching_cubes_pipeline_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    initialize_mc_vertices_pass_ = &marching_cubes_pipeline_->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));
    run_marching_cubes_on_sdf_pass_ = &marching_cubes_pipeline_->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));
}

void SdfMarchingCube::Update(CommandBuffer& command_buffer, RenderTarget& render_target) {
    m_uniform_buffer_data_.origin = m_p_sdf_->GetGridOrigin();
    m_uniform_buffer_data_.cell_size = m_p_sdf_->GetGridCellSize();
    m_uniform_buffer_data_.max_marching_cubes_vertices = m_max_marching_cubes_vertices_;
    m_uniform_buffer_data_.marching_cubes_iso_level = m_uniform_buffer_data_.cell_size * m_sdf_iso_level_;
    m_p_sdf_->GetGridNumCells(m_uniform_buffer_data_.num_cells_x, m_uniform_buffer_data_.num_cells_y,
                              m_uniform_buffer_data_.num_cells_z);
    m_num_total_cells_ = m_p_sdf_->GetGridNumTotalCells();
    uniform_buffer_->Update(&m_uniform_buffer_data_, sizeof(MarchingCubesUniformBuffer));

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
