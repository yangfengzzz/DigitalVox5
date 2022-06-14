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

class SdfCollision {
public:
    SdfCollision(Device& p_device,
                 std::shared_ptr<Mesh> p_coll_mesh,
                 const char* model_name,
                 int num_cells_in_x,
                 float collision_margin);

    // Update and animate the collision mesh
    void Update(CommandBuffer& command_buffer, RenderTarget& render_target, SdfCollisionSystem& system);

    // Grid
    [[nodiscard]] float GetGridCellSize() const { return m_cell_size_; }
    [[nodiscard]] Point3F GetGridOrigin() const { return m_origin_; }
    void GetGridNumCells(int& x, int& y, int& z) const {
        x = m_num_cells_x_;
        y = m_num_cells_y_;
        z = m_num_cells_z_;
    }
    [[nodiscard]] int GetGridNumTotalCells() const { return m_num_total_cells_; }

    SDFCollisionParams& GetConstantBufferData() { return m_const_buffer_; }

private:
    SDFCollisionParams m_const_buffer_;
    std::shared_ptr<Mesh> m_p_input_collision_mesh_;

    // SDF grid
    Point3F m_origin_;
    float m_cell_size_;
    int m_num_cells_x_;
    int m_num_cells_y_;
    int m_num_cells_z_;
    int m_num_total_cells_;
    Vector3F m_min_;
    Vector3F m_max_;
    Vector3F m_padding_boundary_;
    float m_grid_allocation_multiplier_;

    // number of cells in X axis
    int m_num_cells_in_x_axis_;

    // SDF collision margin.
    float m_collision_margin_;

    void UpdateSdfGrid(const Point3F& tight_bbox_min, const Point3F& tight_bbox_max);
};

}  // namespace vox::compute
