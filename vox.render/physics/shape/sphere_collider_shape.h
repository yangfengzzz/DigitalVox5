//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_SPHERE_COLLIDER_SHAPE_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_SPHERE_COLLIDER_SHAPE_H_

#include "collider_shape.h"

namespace vox::physics {
/**
 * Physical collider shape for sphere.
 */
class SphereColliderShape : public ColliderShape {
public:
    SphereColliderShape();

    /**
     * Radius of sphere shape.
     */
    [[nodiscard]] float radius() const;

    void set_radius(float value);

    void set_world_scale(const Vector3F &scale) override;

#ifdef DEBUG
    void set_entity(Entity *value) override;

    void sync_sphere_geometry();
#endif

private:
    float radius_ = 1.0;
};

}  // namespace vox::physics

#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_SPHERE_COLLIDER_SHAPE_H_ */
