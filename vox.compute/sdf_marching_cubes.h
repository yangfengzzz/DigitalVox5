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
#include "vox.render/rendering/postprocessing_computepass.h"
#include "vox.render/rendering/postprocessing_pipeline.h"

namespace vox::compute {
class SdfCollision;

struct MarchingCubesUniformBuffer {
    Matrix4x4F m_mw;
    Matrix4x4F m_mwp;
    Vector4F c_color;
    Vector4F v_light_dir;
    Vector4F g_origin;

    float g_cell_size;
    int32_t g_num_cells_x;
    int32_t g_num_cells_y;
    int32_t g_num_cells_z;

    int32_t g_max_marching_cubes_vertices;
    float g_marching_cubes_iso_level;
};

class SdfMarchingCubes {
public:
    SdfMarchingCubes();

    void Initialize(const char* name, Device& device, RenderContext& render_context);

    // Draw the SDF using marching cubes for debug purpose
    void Draw();

    // Draw the grid
    void DrawGrid();

    // Update mesh by running marching cubes
    void Update(CommandBuffer& command_buffer, RenderTarget& render_target);

    void SetSdf(SdfCollision* sdf) {
        assert(sdf);
        m_p_sdf_ = sdf;
    }

    // Setting the SDF ISO level for drawing.
    void SetSdfIsoLevel(float iso_level) { m_sdf_iso_level_ = iso_level; }

private:
    // SDF grid
    Point3F m_origin_;
    float m_cell_size_{};
    int m_num_cells_x_{};
    int m_num_cells_y_{};
    int m_num_cells_z_{};
    int m_num_total_cells_{};

    SdfCollision* m_p_sdf_{};

    MarchingCubesUniformBuffer m_uniform_buffer_data_;

    // For drawing lines
    // SuBatchLineRenderer m_LineRenderer;

    struct VertexData {
        float position[4];
        float normal[4];
    };

    // SDF ISO level. This value will be multiplied with the cell size before be passed to the compute shader.
    float m_sdf_iso_level_{};

    const int m_max_marching_cubes_vertices_;
    int m_num_mc_vertices_{};

    // compute shader
    PostProcessingComputePass* initialize_mc_vertices_pass_{nullptr};
    PostProcessingComputePass* run_marching_cubes_on_sdf_pass_{nullptr};
    std::unique_ptr<PostProcessingPipeline> marching_cubes_pipeline_{nullptr};
};

}  // namespace vox::compute
