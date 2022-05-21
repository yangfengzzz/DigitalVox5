//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_HIT_RESULT_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_HIT_RESULT_H_

#include "../entity.h"
#include "point3.h"
#include "vector3.h"

namespace vox::physics {

/**
 * Structure used to get information back from a raycast or a sweep.
 */
struct HitResult {
    /** The entity that was hit. */
    Entity *entity = nullptr;
    /** The distance from the ray's origin to the impact point. */
    float distance = 0;
    /** The impact point in world space where the ray hit the collider. */
    Point3F point = Point3F();
    /** The normal of the surface the ray hit. */
    Vector3F normal = Vector3F();
};

}  // namespace vox::physics

#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_HIT_RESULT_H_ */
