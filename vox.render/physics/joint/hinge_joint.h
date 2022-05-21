//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_HINGE_JOINT_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_HINGE_JOINT_H_

#include "joint.h"

namespace vox::physics {
/**
 * A joint which behaves in a similar way to a hinge or axle.
 */
class HingeJoint : public Joint {
public:
    HingeJoint(Collider *collider_0, Collider *collider_1);

    [[nodiscard]] float angle() const;

    [[nodiscard]] float velocity() const;

    void set_limit(const PxJointAngularLimitPair &limits);

    [[nodiscard]] PxJointAngularLimitPair limit() const;

    void set_drive_velocity(float velocity, bool autowake = true);

    [[nodiscard]] float drive_velocity() const;

    void set_drive_force_limit(float limit);

    [[nodiscard]] float drive_force_limit() const;

    void set_drive_gear_ratio(float ratio);

    [[nodiscard]] float drive_gear_ratio() const;

    void set_revolute_joint_flags(const PxRevoluteJointFlags &flags);

    void set_revolute_joint_flag(PxRevoluteJointFlag::Enum flag, bool value);

    [[nodiscard]] PxRevoluteJointFlags revolute_joint_flags() const;

    void set_projection_linear_tolerance(float tolerance);

    [[nodiscard]] float projection_linear_tolerance() const;

    void set_projection_angular_tolerance(float tolerance);

    [[nodiscard]] float projection_angular_tolerance() const;
};

}  // namespace vox::physics
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_HINGE_JOINT_H_ */
