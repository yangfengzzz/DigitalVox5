//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "joint.h"

namespace vox::physics {
/**
 * A joint which behaves in a similar way to a hinge or axle.
 */
class HingeJoint : public Joint {
public:
    HingeJoint(Collider *collider_0, Collider *collider_1);

    [[nodiscard]] float Angle() const;

    [[nodiscard]] float Velocity() const;

    void SetLimit(const PxJointAngularLimitPair &limits);

    [[nodiscard]] PxJointAngularLimitPair Limit() const;

    void SetDriveVelocity(float velocity, bool autowake = true);

    [[nodiscard]] float DriveVelocity() const;

    void SetDriveForceLimit(float limit);

    [[nodiscard]] float DriveForceLimit() const;

    void SetDriveGearRatio(float ratio);

    [[nodiscard]] float DriveGearRatio() const;

    void SetRevoluteJointFlags(const PxRevoluteJointFlags &flags);

    void SetRevoluteJointFlag(PxRevoluteJointFlag::Enum flag, bool value);

    [[nodiscard]] PxRevoluteJointFlags RevoluteJointFlags() const;

    void SetProjectionLinearTolerance(float tolerance);

    [[nodiscard]] float ProjectionLinearTolerance() const;

    void SetProjectionAngularTolerance(float tolerance);

    [[nodiscard]] float ProjectionAngularTolerance() const;
};

}  // namespace vox::physics