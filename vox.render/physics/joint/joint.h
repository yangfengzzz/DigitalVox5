//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "../physics.h"
#include "transform3.h"

namespace vox::physics {
/**
 * A base class providing common functionality for joints.
 */
class Joint {
public:
    void SetActors(Collider *actor_0, Collider *actor_1);

    void SetLocalPose(PxJointActorIndex::Enum actor, const Transform3F &local_pose);

    [[nodiscard]] Transform3F LocalPose(PxJointActorIndex::Enum actor) const;

    [[nodiscard]] Transform3F RelativeTransform() const;

    [[nodiscard]] Vector3F RelativeLinearVelocity() const;

    [[nodiscard]] Vector3F RelativeAngularVelocity() const;

    void SetBreakForce(float force, float torque);

    void GetBreakForce(float &force, float &torque) const;

    void SetConstraintFlags(const PxConstraintFlags &flags);

    void SetConstraintFlag(PxConstraintFlag::Enum flag, bool value);

    [[nodiscard]] PxConstraintFlags ConstraintFlags() const;

    void SetInvMassScale0(float inv_mass_scale);

    [[nodiscard]] float InvMassScale0() const;

    void SetInvInertiaScale0(float inv_inertia_scale);

    [[nodiscard]] float InvInertiaScale0() const;

    void SetInvMassScale1(float inv_mass_scale);

    [[nodiscard]] float InvMassScale1() const;

    void SetInvInertiaScale1(float inv_inertia_scale);

    [[nodiscard]] float InvInertiaScale1() const;

protected:
    PxJoint *native_joint_;
};

}  // namespace vox::physics