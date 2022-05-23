//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/physics/shape/collider_shape.h"

namespace vox::physics {
/**
 * Physical collider shape plane.
 */
class PlaneColliderShape : public ColliderShape {
public:
    PlaneColliderShape();

    /**
     * The local rotation of this plane.
     */
    Vector3F Rotation();

    void SetRotation(const Vector3F &value);
};

}  // namespace vox::physics