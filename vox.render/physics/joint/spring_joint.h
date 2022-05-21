//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "joint.h"

namespace vox::physics {
/**
 * A joint that maintains an upper or lower bound (or both) on the distance between two points on different objects.
 */
class SpringJoint : public Joint {
public:
    SpringJoint(Collider *collider_0, Collider *collider_1);

    [[nodiscard]] float Distance() const;

    void SetMinDistance(float distance);

    [[nodiscard]] float MinDistance() const;

    void SetMaxDistance(float distance);

    [[nodiscard]] float MaxDistance() const;

    void SetTolerance(float tolerance);

    [[nodiscard]] float Tolerance() const;

    void SetStiffness(float stiffness);

    [[nodiscard]] float Stiffness() const;

    void SetDamping(float damping);

    [[nodiscard]] float Damping() const;

    void SetDistanceJointFlags(const PxDistanceJointFlags &flags);

    void SetDistanceJointFlag(PxDistanceJointFlag::Enum flag, bool value);

    [[nodiscard]] PxDistanceJointFlags DistanceJointFlags() const;
};

}  // namespace vox::physics