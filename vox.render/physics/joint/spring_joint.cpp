//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spring_joint.h"

#include "../collider.h"
#include "../physics_manager.h"

namespace vox::physics {
SpringJoint::SpringJoint(Collider *collider_0, Collider *collider_1) : Joint() {
    auto actor_0 = collider_0 ? collider_0->handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->handle() : nullptr;
    native_joint_ = PxDistanceJointCreate(*PhysicsManager::native_physics_(), actor_0,
                                          PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)), actor_1,
                                          PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

float SpringJoint::distance() const { return static_cast<PxDistanceJoint *>(native_joint_)->getDistance(); }

void SpringJoint::set_min_distance(float distance) {
    static_cast<PxDistanceJoint *>(native_joint_)->setMinDistance(distance);
}

float SpringJoint::min_distance() const { return static_cast<PxDistanceJoint *>(native_joint_)->getMinDistance(); }

void SpringJoint::set_max_distance(float distance) {
    static_cast<PxDistanceJoint *>(native_joint_)->setMaxDistance(distance);
}

float SpringJoint::max_distance() const { return static_cast<PxDistanceJoint *>(native_joint_)->getMaxDistance(); }

void SpringJoint::set_tolerance(float tolerance) {
    static_cast<PxDistanceJoint *>(native_joint_)->setTolerance(tolerance);
}

float SpringJoint::tolerance() const { return static_cast<PxDistanceJoint *>(native_joint_)->getTolerance(); }

void SpringJoint::set_stiffness(float stiffness) {
    static_cast<PxDistanceJoint *>(native_joint_)->setStiffness(stiffness);
}

float SpringJoint::stiffness() const { return static_cast<PxDistanceJoint *>(native_joint_)->getStiffness(); }

void SpringJoint::set_damping(float damping) { static_cast<PxDistanceJoint *>(native_joint_)->setDamping(damping); }

float SpringJoint::damping() const { return static_cast<PxDistanceJoint *>(native_joint_)->getDamping(); }

void SpringJoint::set_distance_joint_flags(const PxDistanceJointFlags &flags) {
    static_cast<PxDistanceJoint *>(native_joint_)->setDistanceJointFlags(flags);
}

void SpringJoint::set_distance_joint_flag(PxDistanceJointFlag::Enum flag, bool value) {
    static_cast<PxDistanceJoint *>(native_joint_)->setDistanceJointFlag(flag, value);
}

PxDistanceJointFlags SpringJoint::distance_joint_flags() const {
    return static_cast<PxDistanceJoint *>(native_joint_)->getDistanceJointFlags();
}

}  // namespace vox::physics
