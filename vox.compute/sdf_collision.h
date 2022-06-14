//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/matrix4x4.h"
#include "vox.math/vector4.h"
#include "vox.render/core/command_buffer.h"
#include "vox.render/mesh/mesh.h"

namespace vox::compute {
class SDFCollision {
public:
    SDFCollision(Device& pDevice,
                 std::shared_ptr<Mesh> pCollMesh,
                 const char* modelName,
                 int numCellsInX,
                 float collisionMargin);
};

}  // namespace vox::compute
