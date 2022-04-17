//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_CAPSULE_COLLIDER_SHAPE_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_CAPSULE_COLLIDER_SHAPE_H_

#include "collider_shape.h"

#ifdef DEBUG
#include "mesh/mesh_renderer.h"
#endif

namespace vox::physics {
/**
 * The up axis of the collider shape.
 */
struct ColliderShapeUpAxis {
    enum Enum {
        /** Up axis is X. */
        X,
        /** Up axis is Y. */
        Y,
        /** Up axis is Z. */
        Z
    };
};

/**
 * Physical collider shape for capsule.
 */
class CapsuleColliderShape : public ColliderShape {
public:
    CapsuleColliderShape();
    
    /**
     * Radius of capsule.
     */
    [[nodiscard]] float radius() const;
    
    void set_radius(float value);
    
    /**
     * Height of capsule.
     */
    [[nodiscard]] float height() const;
    
    void set_height(float value);
    
    /**
     * Up axis of capsule.
     */
    ColliderShapeUpAxis::Enum up_axis();
    
    void set_up_axis(ColliderShapeUpAxis::Enum value);
    
    void set_world_scale(const Vector3F &scale) override;
    
#ifdef DEBUG
    void set_entity(Entity *value) override;
    
    void sync_capsule_geometry();
    
    void sync_capsule_axis(ColliderShapeUpAxis::Enum up_axis);
    
    MeshRenderer *renderer_{nullptr};
#endif
    
private:
    float radius_ = 1;
    float height_ = 2;
    ColliderShapeUpAxis::Enum up_axis_ = ColliderShapeUpAxis::Enum::Y;
};

}
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_CAPSULE_COLLIDER_SHAPE_H_ */
