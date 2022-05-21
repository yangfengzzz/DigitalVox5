//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_JOINT_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_JOINT_H_

#include "../physics.h"
#include "transform3.h"

namespace vox::physics {
/**
 * A base class providing common functionality for joints.
 */
class Joint {
public:
    void set_actors(Collider *actor_0, Collider *actor_1);

    void set_local_pose(PxJointActorIndex::Enum actor, const Transform3F &local_pose);

    [[nodiscard]] Transform3F local_pose(PxJointActorIndex::Enum actor) const;

    [[nodiscard]] Transform3F relative_transform() const;

    [[nodiscard]] Vector3F relative_linear_velocity() const;

    [[nodiscard]] Vector3F relative_angular_velocity() const;

    void set_break_force(float force, float torque);

    void get_break_force(float &force, float &torque) const;

    void set_constraint_flags(const PxConstraintFlags &flags);

    void set_constraint_flag(PxConstraintFlag::Enum flag, bool value);

    [[nodiscard]] PxConstraintFlags constraint_flags() const;

    void set_inv_mass_scale_0(float inv_mass_scale);

    [[nodiscard]] float inv_mass_scale_0() const;

    void set_inv_inertia_scale_0(float inv_inertia_scale);

    [[nodiscard]] float inv_inertia_scale_0() const;

    void set_inv_mass_scale_1(float inv_mass_scale);

    [[nodiscard]] float inv_mass_scale_1() const;

    void set_inv_inertia_scale_1(float inv_inertia_scale);

    [[nodiscard]] float inv_inertia_scale_1() const;

protected:
    PxJoint *native_joint_;
};

}  // namespace vox::physics

#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_JOINT_H_ */
