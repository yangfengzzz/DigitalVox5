//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.compute/constant_buffers.h"
#include "vox.math/matrix4x4.h"
#include "vox.math/vector4.h"
#include "vox.render/component.h"
#include "vox.render/core/command_buffer.h"
#include "vox.render/rendering/postprocessing_computepass.h"
#include "vox.render/rendering/postprocessing_pipeline.h"

namespace vox::compute {
class SdfGrid;

struct MarchingCubesUniformBuffer {
    Point3F origin;
    float cell_size;

    int32_t num_cells_x;
    int32_t num_cells_y;
    int32_t num_cells_z;
    int32_t max_marching_cubes_vertices;

    float marching_cubes_iso_level;
};

class SdfMarchingCube : public Component {
public:
    static constexpr int m_max_marching_cubes_vertices_ = 128 * 1024;

    explicit SdfMarchingCube(Entity* entity);

    void Initialize(const char* name, Device& device, RenderContext& render_context);

    // Update mesh by running marching cubes
    void Update(CommandBuffer& command_buffer, RenderTarget& render_target);

    void SetSdf(SdfGrid* sdf) {
        assert(sdf);
        m_p_sdf_ = sdf;
    }

    // Setting the SDF ISO level for drawing.
    inline void SetSdfIsoLevel(float iso_level) { m_sdf_iso_level_ = iso_level; }

private:
    // SDF grid
    SdfGrid* m_p_sdf_{};
    MarchingCubesUniformBuffer m_uniform_buffer_data_;
    std::unique_ptr<core::Buffer> uniform_buffer_{nullptr};
    // SDF ISO level. This value will be multiplied with the cell size before be passed to the compute shader.
    float m_sdf_iso_level_{};
    int m_num_total_cells_{};

    struct VertexData {
        Vector4F position;
        Vector4F normal;
    };
    std::unique_ptr<core::Buffer> mc_triangle_vertices_buffer_{nullptr};
    std::unique_ptr<core::Buffer> num_mc_vertices_{nullptr};
    std::unique_ptr<core::Buffer> mc_edge_table_{nullptr};
    std::unique_ptr<core::Buffer> mc_triangle_table_{nullptr};

    // compute shader
    PostProcessingComputePass* initialize_mc_vertices_pass_{nullptr};
    PostProcessingComputePass* run_marching_cubes_on_sdf_pass_{nullptr};
    std::unique_ptr<PostProcessingPipeline> marching_cubes_pipeline_{nullptr};
};

}  // namespace vox::compute
