//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "joint.h"

namespace vox::physics {
/**
 * A joint which behaves in a similar way to a ball and socket.
 */
class SphericalJoint : public Joint {
public:
    SphericalJoint(Collider *collider_0, Collider *collider_1);

    [[nodiscard]] PxJointLimitCone LimitCone() const;

    void SetLimitCone(const PxJointLimitCone &limit);

    [[nodiscard]] float SwingYAngle() const;

    [[nodiscard]] float SwingZAngle() const;

    void SetSphericalJointFlags(const PxSphericalJointFlags &flags);

    void SetSphericalJointFlag(PxSphericalJointFlag::Enum flag, bool value);

    [[nodiscard]] PxSphericalJointFlags SphericalJointFlags() const;

    void SetProjectionLinearTolerance(float tolerance);

    [[nodiscard]] float ProjectionLinearTolerance() const;
};

}  // namespace vox::physics