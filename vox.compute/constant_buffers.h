//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.compute/common.h"
#include "vox.math/matrix4x4.h"
#include "vox.math/vector4.h"
#include "vox.render/core/command_buffer.h"

namespace vox::compute {
struct TressFXSimulationParams {
    Vector4F m_wind;
    Vector4F m_wind_1;
    Vector4F m_wind_2;
    Vector4F m_wind_3;

    Vector4F m_shape;  // damping, local stiffness, global stiffness, global range.
    // float m_Damping;
    // float m_StiffnessForLocalShapeMatching;
    // float m_StiffnessForGlobalShapeMatching;
    // float m_GlobalShapeMatchingEffectiveRange;

    Vector4F m_grav_time_tip;  // gravity, time step size,
    // float m_GravityMagnitude;
    // float m_TimeStep;
    // float m_TipSeparationFactor;
    // float m_velocityShockPropagation;

    Point<int32_t, 4> m_sim_ints;  // 4th component unused.
    // int m_NumLengthConstraintIterations;
    // int m_NumLocalShapeMatchingIterations;
    // int m_bCollision;
    // int m_CPULocalIterations;

    Point<int32_t, 4> m_counts;
    // int m_NumOfStrandsPerThreadGroup;
    // int m_NumFollowHairsPerGuideHair;
    // int m_NumVerticesPerStrand; // should be 2^n (n is integer and greater than 2) and less than
    // or equal to SIM_THREAD_GROUP_SIZE. i.e. 8, 16, 32 or 64

    Vector4F m_vsp;

    float g_reset_positions;
    float g_clamp_position_delta;
    float g_pad_1;
    float g_pad_2;

    Matrix4x4F m_bone_skinning_matrix[MAX_NUM_BONES];

#if COLLISION_CAPSULES
    Vector4F m_center_and_radius_0[MAX_NUM_COLLISION_CAPSULES];
    Vector4F m_center_and_radius_1[MAX_NUM_COLLISION_CAPSULES];
    Point<int32_t, 4> m_num_collision_capsules;
#endif

    void SetDamping(float d) { m_shape.x = d; }
    void SetLocalStiffness(float s) { m_shape.y = s; }
    void SetGlobalStiffness(float s) { m_shape.z = s; }
    void SetGlobalRange(float r) { m_shape.w = r; }

    void SetGravity(float g) { m_grav_time_tip.x = g; }
    void SetTimeStep(float dt) { m_grav_time_tip.y = dt; }
    void SetTipSeparation(float ts) { m_grav_time_tip.z = ts; }

    void SetVelocityShockPropagation(float vsp) { m_vsp.x = vsp; }
    void SetVspAccelThreshold(float vsp_accel_threshold) { m_vsp.y = vsp_accel_threshold; }

    void SetLengthIterations(int i) { m_sim_ints[0] = i; }
    void SetLocalIterations(int i) { m_sim_ints[1] = i; }
    void SetCollision(bool on) { m_sim_ints[2] = on ? 1 : 0; }

    void SetVerticesPerStrand(int n) {
        m_counts[0] = SIM_THREAD_GROUP_SIZE / n;
        m_counts[2] = n;
    }
    void SetFollowHairsPerGuidHair(int n) { m_counts[1] = n; }
};

struct CapsuleCollisionConstantBuffer {
    Vector4F m_center_and_radius[MAX_NUM_COLLISION_CAPSULES];
    Vector4F m_center_and_radius_squared[MAX_NUM_COLLISION_CAPSULES];
    int m_num_collision_capsules;
};

struct SDFGridParams {
    Vector4F m_origin;
    float m_cell_size;
    int m_num_cells_x;
    int m_num_cells_y;
    int m_num_cells_z;
    int m_max_marching_cubes_vertices;
    float m_marching_cubes_iso_level;
    float m_collision_margin;
    int m_num_hair_vertices_per_strand;
    int m_num_total_hair_vertices;
    float pad_1;
    float pad_2;
    float pad_3;
};

}  // namespace vox::compute