//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_CONFIGURABLE_JOINT_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_CONFIGURABLE_JOINT_H_

#include "joint.h"

namespace vox::physics {
/**
 * A Configurable joint is a general constraint between two actors.
 */
class ConfigurableJoint : public Joint {
public:
    ConfigurableJoint(Collider *collider_0, Collider *collider_1);
    
    void set_motion(PxD6Axis::Enum axis, PxD6Motion::Enum type);
    
    [[nodiscard]] PxD6Motion::Enum motion(PxD6Axis::Enum axis) const;
    
    [[nodiscard]] float twist_angle() const;
    
    [[nodiscard]] float swing_y_angle() const;
    
    [[nodiscard]] float swing_z_angle() const;
    
public:
    void set_distance_limit(const PxJointLinearLimit &limit);
    
    [[nodiscard]] PxJointLinearLimit distance_limit() const;
    
    void set_linear_limit(PxD6Axis::Enum axis, const PxJointLinearLimitPair &limit);
    
    [[nodiscard]] PxJointLinearLimitPair linear_limit(PxD6Axis::Enum axis) const;
    
    void set_twist_limit(const PxJointAngularLimitPair &limit);
    
    [[nodiscard]] PxJointAngularLimitPair twist_limit() const;
    
    void set_swing_limit(const PxJointLimitCone &limit);
    
    [[nodiscard]] PxJointLimitCone swing_limit() const;
    
    void pyramid_swing_limit(const PxJointLimitPyramid &limit);
    
    [[nodiscard]] PxJointLimitPyramid pyramid_swing_limit() const;
    
public:
    void set_drive(PxD6Drive::Enum index, const PxD6JointDrive &drive);
    
    [[nodiscard]] PxD6JointDrive drive(PxD6Drive::Enum index) const;
    
    void set_drive_position(const Transform3F &pose, bool autowake = true);
    
    [[nodiscard]] Transform3F drive_position() const;
    
    void set_drive_velocity(const Vector3F &linear, const Vector3F &angular, bool autowake = true);
    
    void drive_velocity(Vector3F &linear, Vector3F &angular) const;
    
    void set_projection_linear_tolerance(float tolerance);
    
    [[nodiscard]] float projection_linear_tolerance() const;
    
    void set_projection_angular_tolerance(float tolerance);
    
    [[nodiscard]] float projection_angular_tolerance() const;
};
}
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_CONFIGURABLE_JOINT_H_ */
