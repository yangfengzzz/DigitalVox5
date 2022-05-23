//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/physics/joint/joint.h"

namespace vox::physics {
/**
 * A Configurable joint is a general constraint between two actors.
 */
class ConfigurableJoint : public Joint {
public:
    ConfigurableJoint(Collider *collider_0, Collider *collider_1);

    void SetMotion(PxD6Axis::Enum axis, PxD6Motion::Enum type);

    [[nodiscard]] PxD6Motion::Enum Motion(PxD6Axis::Enum axis) const;

    [[nodiscard]] float TwistAngle() const;

    [[nodiscard]] float SwingYAngle() const;

    [[nodiscard]] float SwingZAngle() const;

public:
    void SetDistanceLimit(const PxJointLinearLimit &limit);

    [[nodiscard]] PxJointLinearLimit DistanceLimit() const;

    void SetLinearLimit(PxD6Axis::Enum axis, const PxJointLinearLimitPair &limit);

    [[nodiscard]] PxJointLinearLimitPair LinearLimit(PxD6Axis::Enum axis) const;

    void SetTwistLimit(const PxJointAngularLimitPair &limit);

    [[nodiscard]] PxJointAngularLimitPair TwistLimit() const;

    void SetSwingLimit(const PxJointLimitCone &limit);

    [[nodiscard]] PxJointLimitCone SwingLimit() const;

    void PyramidSwingLimit(const PxJointLimitPyramid &limit);

    [[nodiscard]] PxJointLimitPyramid PyramidSwingLimit() const;

public:
    void SetDrive(PxD6Drive::Enum index, const PxD6JointDrive &drive);

    [[nodiscard]] PxD6JointDrive Drive(PxD6Drive::Enum index) const;

    void SetDrivePosition(const Transform3F &pose, bool autowake = true);

    [[nodiscard]] Transform3F DrivePosition() const;

    void SetDriveVelocity(const Vector3F &linear, const Vector3F &angular, bool autowake = true);

    void DriveVelocity(Vector3F &linear, Vector3F &angular) const;

    void SetProjectionLinearTolerance(float tolerance);

    [[nodiscard]] float ProjectionLinearTolerance() const;

    void SetProjectionAngularTolerance(float tolerance);

    [[nodiscard]] float ProjectionAngularTolerance() const;
};
}  // namespace vox::physics
