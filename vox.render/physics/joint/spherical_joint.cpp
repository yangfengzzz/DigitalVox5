//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/joint/spherical_joint.h"

#include "vox.render/physics/collider.h"
#include "vox.render/physics/physics_manager.h"

namespace vox::physics {
SphericalJoint::SphericalJoint(Collider *collider_0, Collider *collider_1) {
    auto actor_0 = collider_0 ? collider_0->Handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->Handle() : nullptr;
    native_joint_ = PxSphericalJointCreate(*PhysicsManager::native_physics_(), actor_0,
                                           PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)), actor_1,
                                           PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

PxJointLimitCone SphericalJoint::LimitCone() const {
    return static_cast<PxSphericalJoint *>(native_joint_)->getLimitCone();
}

void SphericalJoint::SetLimitCone(const PxJointLimitCone &limit) {
    static_cast<PxSphericalJoint *>(native_joint_)->setLimitCone(limit);
}

float SphericalJoint::SwingYAngle() const { return static_cast<PxSphericalJoint *>(native_joint_)->getSwingYAngle(); }

float SphericalJoint::SwingZAngle() const { return static_cast<PxSphericalJoint *>(native_joint_)->getSwingZAngle(); }

void SphericalJoint::SetSphericalJointFlags(const PxSphericalJointFlags &flags) {
    static_cast<PxSphericalJoint *>(native_joint_)->setSphericalJointFlags(flags);
}

void SphericalJoint::SetSphericalJointFlag(PxSphericalJointFlag::Enum flag, bool value) {
    static_cast<PxSphericalJoint *>(native_joint_)->setSphericalJointFlag(flag, value);
}

PxSphericalJointFlags SphericalJoint::SphericalJointFlags() const {
    return static_cast<PxSphericalJoint *>(native_joint_)->getSphericalJointFlags();
}

void SphericalJoint::SetProjectionLinearTolerance(float tolerance) {
    static_cast<PxSphericalJoint *>(native_joint_)->setProjectionLinearTolerance(tolerance);
}

float SphericalJoint::ProjectionLinearTolerance() const {
    return static_cast<PxSphericalJoint *>(native_joint_)->getProjectionLinearTolerance();
}

}  // namespace vox::physics
