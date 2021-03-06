//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/joint/fixed_joint.h"

#include "vox.render/physics/collider.h"
#include "vox.render/physics/physics_manager.h"

namespace vox::physics {
FixedJoint::FixedJoint(Collider *collider_0, Collider *collider_1) : Joint() {
    auto actor_0 = collider_0 ? collider_0->Handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->Handle() : nullptr;
    native_joint_ =
            PxFixedJointCreate(*PhysicsManager::native_physics_(), actor_0, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)),
                               actor_1, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

void FixedJoint::SetProjectionLinearTolerance(float tolerance) {
    return static_cast<PxFixedJoint *>(native_joint_)->setProjectionLinearTolerance(tolerance);
}

float FixedJoint::ProjectionLinearTolerance() const {
    return static_cast<PxFixedJoint *>(native_joint_)->getProjectionLinearTolerance();
}

void FixedJoint::SetProjectionAngularTolerance(float tolerance) {
    return static_cast<PxFixedJoint *>(native_joint_)->setProjectionAngularTolerance(tolerance);
}

float FixedJoint::ProjectionAngularTolerance() const {
    return static_cast<PxFixedJoint *>(native_joint_)->getProjectionAngularTolerance();
}

}  // namespace vox::physics
