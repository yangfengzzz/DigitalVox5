//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_SPRING_JOINT_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_SPRING_JOINT_H_

#include "joint.h"

namespace vox::physics {
/**
 * A joint that maintains an upper or lower bound (or both) on the distance between two points on different objects.
 */
class SpringJoint : public Joint {
public:
    SpringJoint(Collider *collider_0, Collider *collider_1);

    [[nodiscard]] float distance() const;

    void set_min_distance(float distance);

    [[nodiscard]] float min_distance() const;

    void set_max_distance(float distance);

    [[nodiscard]] float max_distance() const;

    void set_tolerance(float tolerance);

    [[nodiscard]] float tolerance() const;

    void set_stiffness(float stiffness);

    [[nodiscard]] float stiffness() const;

    void set_damping(float damping);

    [[nodiscard]] float damping() const;

    void set_distance_joint_flags(const PxDistanceJointFlags &flags);

    void set_distance_joint_flag(PxDistanceJointFlag::Enum flag, bool value);

    [[nodiscard]] PxDistanceJointFlags distance_joint_flags() const;
};

}  // namespace vox::physics
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_SPRING_JOINT_H_ */
