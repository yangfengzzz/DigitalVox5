//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "translational_joint.h"

#include "../collider.h"
#include "../physics_manager.h"

namespace vox::physics {
TranslationalJoint::TranslationalJoint(Collider *collider_0, Collider *collider_1) : Joint() {
    auto actor_0 = collider_0 ? collider_0->handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->handle() : nullptr;
    native_joint_ = PxPrismaticJointCreate(*PhysicsManager::native_physics_(), actor_0,
                                           PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)), actor_1,
                                           PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

float TranslationalJoint::position() const { return static_cast<PxPrismaticJoint *>(native_joint_)->getPosition(); }

float TranslationalJoint::velocity() const { return static_cast<PxPrismaticJoint *>(native_joint_)->getVelocity(); }

void TranslationalJoint::set_limit(const PxJointLinearLimitPair &pair) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setLimit(pair);
}

PxJointLinearLimitPair TranslationalJoint::limit() const {
    return static_cast<PxPrismaticJoint *>(native_joint_)->getLimit();
}

void TranslationalJoint::set_prismatic_joint_flags(const PxPrismaticJointFlags &flags) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setPrismaticJointFlags(flags);
}

void TranslationalJoint::set_prismatic_joint_flag(PxPrismaticJointFlag::Enum flag, bool value) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setPrismaticJointFlag(flag, value);
}

PxPrismaticJointFlags TranslationalJoint::translational_joint_flags() const {
    return static_cast<PxPrismaticJoint *>(native_joint_)->getPrismaticJointFlags();
}

void TranslationalJoint::set_projection_linear_tolerance(float tolerance) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setProjectionLinearTolerance(tolerance);
}

float TranslationalJoint::projection_linear_tolerance() const {
    return static_cast<PxPrismaticJoint *>(native_joint_)->getProjectionLinearTolerance();
}

void TranslationalJoint::set_projection_angular_tolerance(float tolerance) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setProjectionAngularTolerance(tolerance);
}

float TranslationalJoint::projection_angular_tolerance() const {
    return static_cast<PxPrismaticJoint *>(native_joint_)->getProjectionAngularTolerance();
}

}  // namespace vox::physics
