//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spherical_joint.h"
#include "../physics_manager.h"
#include "../collider.h"

namespace vox::physics {
SphericalJoint::SphericalJoint(Collider *collider_0, Collider *collider_1) {
    auto actor_0 = collider_0 ? collider_0->handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->handle() : nullptr;
    native_joint_ = PxSphericalJointCreate(*PhysicsManager::native_physics_(),
                                           actor_0, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)),
                                           actor_1, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

PxJointLimitCone SphericalJoint::limit_cone() const {
    return static_cast<PxSphericalJoint *>(native_joint_)->getLimitCone();
}

void SphericalJoint::set_limit_cone(const PxJointLimitCone &limit) {
    static_cast<PxSphericalJoint *>(native_joint_)->setLimitCone(limit);
}

float SphericalJoint::swing_y_angle() const {
    return static_cast<PxSphericalJoint *>(native_joint_)->getSwingYAngle();
}

float SphericalJoint::swing_z_angle() const {
    return static_cast<PxSphericalJoint *>(native_joint_)->getSwingZAngle();
}

void SphericalJoint::set_spherical_joint_flags(const PxSphericalJointFlags &flags) {
    static_cast<PxSphericalJoint *>(native_joint_)->setSphericalJointFlags(flags);
}

void SphericalJoint::set_spherical_joint_flag(PxSphericalJointFlag::Enum flag, bool value) {
    static_cast<PxSphericalJoint *>(native_joint_)->setSphericalJointFlag(flag, value);
}

PxSphericalJointFlags SphericalJoint::spherical_joint_flags() const {
    return static_cast<PxSphericalJoint *>(native_joint_)->getSphericalJointFlags();
}

void SphericalJoint::set_projection_linear_tolerance(float tolerance) {
    static_cast<PxSphericalJoint *>(native_joint_)->setProjectionLinearTolerance(tolerance);
}

float SphericalJoint::projection_linear_tolerance() const {
    return static_cast<PxSphericalJoint *>(native_joint_)->getProjectionLinearTolerance();
}

}
