//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.compute/constant_buffers.h"
#include "vox.math/matrix4x4.h"
#include "vox.math/vector4.h"
#include "vox.render/core/command_buffer.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/rendering/postprocessing_computepass.h"
#include "vox.render/rendering/postprocessing_pipeline.h"

namespace vox::compute {
struct SDFGridParams {
    Point3F origin;
    float cell_size;

    uint32_t num_cells_x;
    uint32_t num_cells_y;
    uint32_t num_cells_z;
    uint32_t max_marching_cubes_vertices;

    float marching_cubes_iso_level;
    float collision_margin;
    int num_hair_vertices_per_strand;
    int num_total_hair_vertices;
};

struct SdfCollisionSystem {
public:
    void Initialize(Device& device, RenderContext& render_context);

    PostProcessingComputePass* initialize_signed_distance_field_pass{nullptr};
    PostProcessingComputePass* construct_signed_distance_field_pass{nullptr};
    PostProcessingComputePass* finalize_signed_distance_field_pass{nullptr};
    std::unique_ptr<PostProcessingPipeline> signed_distance_field_pipeline{nullptr};

    PostProcessingComputePass* collide_hair_vertices_with_sdf_pass{nullptr};
    std::unique_ptr<PostProcessingPipeline> collide_hair_vertices_with_sdf_pipeline{nullptr};
};

class SdfGrid {
public:
    SdfGrid(Device& device,
            std::shared_ptr<Mesh> coll_mesh,
            const char* model_name,
            int num_cells_in_x,
            float collision_margin);

    // Update and animate the collision mesh
    void Update(CommandBuffer& command_buffer, RenderTarget& render_target, SdfCollisionSystem& system);

    [[nodiscard]] const core::Buffer& GetSdfDataGpuBuffer() const { return *m_signed_distance_field_; }
    core::Buffer& GetSdfDataGpuBuffer() { return *m_signed_distance_field_; }

    [[nodiscard]] float GetSdfCollisionMargin() const { return m_collision_margin_; }
    [[nodiscard]] float GetGridCellSize() const { return m_constant_buffer_data_.cell_size; }
    [[nodiscard]] Point3F GetGridOrigin() const { return m_constant_buffer_data_.origin; }
    void GetGridNumCells(uint32_t& x, uint32_t& y, uint32_t& z) const {
        x = m_constant_buffer_data_.num_cells_x;
        y = m_constant_buffer_data_.num_cells_y;
        z = m_constant_buffer_data_.num_cells_z;
    }
    [[nodiscard]] int GetGridNumTotalCells() const { return m_num_total_cells_; }
    SDFGridParams& GetConstantBufferData() { return m_constant_buffer_data_; }

private:
    SDFGridParams m_constant_buffer_data_;
    std::unique_ptr<core::Buffer> m_constant_buffer_{nullptr};

    std::shared_ptr<Mesh> m_input_collision_mesh_;
    std::unique_ptr<core::Buffer> m_signed_distance_field_{nullptr};

    int m_num_total_cells_;
    Vector3F m_padding_boundary_;
    float m_grid_allocation_multiplier_;
    // number of cells in X axis
    int m_num_cells_in_x_axis_;
    // SDF collision margin.
    float m_collision_margin_;

    void UpdateSdfGrid(const Point3F& tight_bbox_min, const Point3F& tight_bbox_max);
};

}  // namespace vox::compute
