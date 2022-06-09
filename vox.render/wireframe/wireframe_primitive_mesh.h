//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector3.h"
#include "vox.render/mesh/model_mesh.h"

namespace vox {
/**
 * Used to generate common primitive meshes.
 */
class WireframePrimitiveMesh {
public:
    static ModelMeshPtr CreateCuboidWireFrame(float width, float height, float depth);

    static ModelMeshPtr CreateSphereWireFrame(float radius);

    static ModelMeshPtr CreateCapsuleWireFrame(float radius, float height);

private:
    static void CreateCircleWireFrame(float radius,
                                      uint32_t vertex_begin,
                                      uint32_t vertex_count,
                                      uint8_t axis,
                                      const Vector3F &shift,
                                      std::vector<Vector3F> &positions,
                                      std::vector<uint32_t> &indices);

    static void CreateEllipticWireFrame(float radius,
                                        float height,
                                        uint32_t vertex_begin,
                                        uint32_t vertex_count,
                                        uint8_t axis,
                                        std::vector<Vector3F> &positions,
                                        std::vector<uint32_t> &indices);
};

}  // namespace vox
