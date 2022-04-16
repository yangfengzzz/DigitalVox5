//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "hinge_joint.h"
#include "../physics_manager.h"
#include "../collider.h"

namespace vox::physics {
HingeJoint::HingeJoint(Collider *collider_0, Collider *collider_1) : Joint() {
    auto actor_0 = collider_0 ? collider_0->handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->handle() : nullptr;
    native_joint_ = PxRevoluteJointCreate(*PhysicsManager::native_physics_(),
                                          actor_0, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)),
                                          actor_1, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

float HingeJoint::angle() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getAngle();
}

float HingeJoint::velocity() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getVelocity();
}

void HingeJoint::set_limit(const PxJointAngularLimitPair &limits) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setLimit(limits);
}

PxJointAngularLimitPair HingeJoint::limit() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getLimit();
}

void HingeJoint::set_drive_velocity(float velocity, bool autowake) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setDriveVelocity(velocity, autowake);
}

float HingeJoint::drive_velocity() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getDriveVelocity();
}

void HingeJoint::set_drive_force_limit(float limit) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setDriveForceLimit(limit);
}

float HingeJoint::drive_force_limit() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getDriveForceLimit();
}

void HingeJoint::set_drive_gear_ratio(float ratio) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setDriveGearRatio(ratio);
}

float HingeJoint::drive_gear_ratio() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getDriveGearRatio();
}

void HingeJoint::set_revolute_joint_flags(const PxRevoluteJointFlags &flags) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setRevoluteJointFlags(flags);
}

void HingeJoint::set_revolute_joint_flag(PxRevoluteJointFlag::Enum flag, bool value) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setRevoluteJointFlag(flag, value);
}

PxRevoluteJointFlags HingeJoint::revolute_joint_flags() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getRevoluteJointFlags();
}

void HingeJoint::set_projection_linear_tolerance(float tolerance) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setProjectionLinearTolerance(tolerance);
}

float HingeJoint::projection_linear_tolerance() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getProjectionLinearTolerance();
}

void HingeJoint::set_projection_angular_tolerance(float tolerance) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setProjectionAngularTolerance(tolerance);
}

float HingeJoint::projection_angular_tolerance() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getProjectionAngularTolerance();
}

}
