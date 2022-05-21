//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_PLANE_COLLIDER_SHAPE_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_PLANE_COLLIDER_SHAPE_H_

#include "collider_shape.h"

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
    Vector3F rotation();

    void set_rotation(const Vector3F &value);
};

}  // namespace vox::physics
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_PLANE_COLLIDER_SHAPE_H_ */
