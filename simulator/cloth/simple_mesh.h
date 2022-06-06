//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.cloth/foundation/PxVec2.h"
#include "vox.cloth/foundation/PxVec3.h"

namespace vox::cloth {

class Mesh {
    [[nodiscard]] virtual uint32_t GetVertexStride() const = 0;
};

/**
 * SimpleMesh: position + normal + uv
 * We use only this type everywhere, once other versions will be required we should generalize Mesh and refactor code.
 */
class SimpleMesh : public Mesh {
public:
    struct Vertex {
        physx::PxVec3 position;
        physx::PxVec3 normal;
        physx::PxVec2 uv;
    };

    [[nodiscard]] uint32_t GetVertexStride() const override { return sizeof(Vertex); }

    std::vector<Vertex> vertices_;
    std::vector<uint16_t> indices_;

    physx::PxVec3 extents_;
    physx::PxVec3 center_;
};

}  // namespace vox::cloth