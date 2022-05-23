//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/physics/shape/collider_shape.h"

#ifdef DEBUG
#include "vox.render/mesh/mesh_renderer.h"
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
    [[nodiscard]] float Radius() const;

    void SetRadius(float value);

    /**
     * Height of capsule.
     */
    [[nodiscard]] float Height() const;

    void SetHeight(float value);

    /**
     * Up axis of capsule.
     */
    ColliderShapeUpAxis::Enum UpAxis();

    void SetUpAxis(ColliderShapeUpAxis::Enum value);

    void SetWorldScale(const Vector3F &scale) override;

#ifdef DEBUG
    void SetEntity(Entity *value) override;

    void SyncCapsuleGeometry();

    void SyncCapsuleAxis(ColliderShapeUpAxis::Enum up_axis);

    MeshRenderer *renderer_{nullptr};
#endif

private:
    float radius_ = 1;
    float height_ = 2;
    ColliderShapeUpAxis::Enum up_axis_ = ColliderShapeUpAxis::Enum::Y;
};

}  // namespace vox::physics