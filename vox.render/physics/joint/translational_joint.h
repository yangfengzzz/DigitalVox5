//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/physics/joint/joint.h"

namespace vox::physics {
/**
 * A translational joint permits relative translational movement between two bodies along
 * an axis, but no relative rotational movement.
 */
class TranslationalJoint : public Joint {
public:
    TranslationalJoint(Collider *collider_0, Collider *collider_1);

    [[nodiscard]] float Position() const;

    [[nodiscard]] float Velocity() const;

    void SetLimit(const PxJointLinearLimitPair &pair);

    [[nodiscard]] PxJointLinearLimitPair Limit() const;

    void SetPrismaticJointFlags(const PxPrismaticJointFlags &flags);

    void SetPrismaticJointFlag(PxPrismaticJointFlag::Enum flag, bool value);

    [[nodiscard]] PxPrismaticJointFlags TranslationalJointFlags() const;

    void SetProjectionLinearTolerance(float tolerance);

    [[nodiscard]] float ProjectionLinearTolerance() const;

    void SetProjectionAngularTolerance(float tolerance);

    [[nodiscard]] float ProjectionAngularTolerance() const;
};

}  // namespace vox::physics