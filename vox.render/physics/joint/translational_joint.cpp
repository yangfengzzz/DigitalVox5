//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/joint/translational_joint.h"

#include "vox.render/physics/collider.h"
#include "vox.render/physics/physics_manager.h"

namespace vox::physics {
TranslationalJoint::TranslationalJoint(Collider *collider_0, Collider *collider_1) : Joint() {
    auto actor_0 = collider_0 ? collider_0->Handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->Handle() : nullptr;
    native_joint_ = PxPrismaticJointCreate(*PhysicsManager::native_physics_(), actor_0,
                                           PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)), actor_1,
                                           PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

float TranslationalJoint::Position() const { return static_cast<PxPrismaticJoint *>(native_joint_)->getPosition(); }

float TranslationalJoint::Velocity() const { return static_cast<PxPrismaticJoint *>(native_joint_)->getVelocity(); }

void TranslationalJoint::SetLimit(const PxJointLinearLimitPair &pair) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setLimit(pair);
}

PxJointLinearLimitPair TranslationalJoint::Limit() const {
    return static_cast<PxPrismaticJoint *>(native_joint_)->getLimit();
}

void TranslationalJoint::SetPrismaticJointFlags(const PxPrismaticJointFlags &flags) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setPrismaticJointFlags(flags);
}

void TranslationalJoint::SetPrismaticJointFlag(PxPrismaticJointFlag::Enum flag, bool value) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setPrismaticJointFlag(flag, value);
}

PxPrismaticJointFlags TranslationalJoint::TranslationalJointFlags() const {
    return static_cast<PxPrismaticJoint *>(native_joint_)->getPrismaticJointFlags();
}

void TranslationalJoint::SetProjectionLinearTolerance(float tolerance) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setProjectionLinearTolerance(tolerance);
}

float TranslationalJoint::ProjectionLinearTolerance() const {
    return static_cast<PxPrismaticJoint *>(native_joint_)->getProjectionLinearTolerance();
}

void TranslationalJoint::SetProjectionAngularTolerance(float tolerance) {
    static_cast<PxPrismaticJoint *>(native_joint_)->setProjectionAngularTolerance(tolerance);
}

float TranslationalJoint::ProjectionAngularTolerance() const {
    return static_cast<PxPrismaticJoint *>(native_joint_)->getProjectionAngularTolerance();
}

}  // namespace vox::physics
