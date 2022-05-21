//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "hinge_joint.h"

#include "../collider.h"
#include "../physics_manager.h"

namespace vox::physics {
HingeJoint::HingeJoint(Collider *collider_0, Collider *collider_1) : Joint() {
    auto actor_0 = collider_0 ? collider_0->Handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->Handle() : nullptr;
    native_joint_ = PxRevoluteJointCreate(*PhysicsManager::native_physics_(), actor_0,
                                          PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)), actor_1,
                                          PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

float HingeJoint::Angle() const { return static_cast<PxRevoluteJoint *>(native_joint_)->getAngle(); }

float HingeJoint::Velocity() const { return static_cast<PxRevoluteJoint *>(native_joint_)->getVelocity(); }

void HingeJoint::SetLimit(const PxJointAngularLimitPair &limits) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setLimit(limits);
}

PxJointAngularLimitPair HingeJoint::Limit() const { return static_cast<PxRevoluteJoint *>(native_joint_)->getLimit(); }

void HingeJoint::SetDriveVelocity(float velocity, bool autowake) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setDriveVelocity(velocity, autowake);
}

float HingeJoint::DriveVelocity() const { return static_cast<PxRevoluteJoint *>(native_joint_)->getDriveVelocity(); }

void HingeJoint::SetDriveForceLimit(float limit) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setDriveForceLimit(limit);
}

float HingeJoint::DriveForceLimit() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getDriveForceLimit();
}

void HingeJoint::SetDriveGearRatio(float ratio) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setDriveGearRatio(ratio);
}

float HingeJoint::DriveGearRatio() const { return static_cast<PxRevoluteJoint *>(native_joint_)->getDriveGearRatio(); }

void HingeJoint::SetRevoluteJointFlags(const PxRevoluteJointFlags &flags) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setRevoluteJointFlags(flags);
}

void HingeJoint::SetRevoluteJointFlag(PxRevoluteJointFlag::Enum flag, bool value) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setRevoluteJointFlag(flag, value);
}

PxRevoluteJointFlags HingeJoint::RevoluteJointFlags() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getRevoluteJointFlags();
}

void HingeJoint::SetProjectionLinearTolerance(float tolerance) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setProjectionLinearTolerance(tolerance);
}

float HingeJoint::ProjectionLinearTolerance() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getProjectionLinearTolerance();
}

void HingeJoint::SetProjectionAngularTolerance(float tolerance) {
    static_cast<PxRevoluteJoint *>(native_joint_)->setProjectionAngularTolerance(tolerance);
}

float HingeJoint::ProjectionAngularTolerance() const {
    return static_cast<PxRevoluteJoint *>(native_joint_)->getProjectionAngularTolerance();
}

}  // namespace vox::physics
