//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "model_mesh.h"
#include "vector3.h"

namespace vox {
/**
 * Used to generate common primitive meshes.
 */
class WireframePrimitiveMesh {
public:
    static ModelMeshPtr create_cuboid_wire_frame(Device &device, float width, float height, float depth);
    
    static ModelMeshPtr create_sphere_wire_frame(Device &device, float radius);
    
    static ModelMeshPtr create_capsule_wire_frame(Device &device, float radius, float height);
    
private:
    static void create_circle_wire_frame(float radius,
                                         uint32_t vertex_begin,
                                         uint32_t vertex_count,
                                         uint8_t axis,
                                         const Vector3F &shift,
                                         std::vector<Vector3F> &positions,
                                         std::vector<uint32_t> &indices);
    
    static void create_elliptic_wire_frame(float radius, float height,
                                           uint32_t vertex_begin,
                                           uint32_t vertex_count,
                                           uint8_t axis,
                                           std::vector<Vector3F> &positions,
                                           std::vector<uint32_t> &indices);
};

}
