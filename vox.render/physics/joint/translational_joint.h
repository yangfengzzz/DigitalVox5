//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_TRANSLATIONAL_JOINT_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_TRANSLATIONAL_JOINT_H_

#include "joint.h"

namespace vox::physics {
/**
 * A translational joint permits relative translational movement between two bodies along
 * an axis, but no relative rotational movement.
 */
class TranslationalJoint : public Joint {
public:
    TranslationalJoint(Collider *collider_0, Collider *collider_1);
    
    [[nodiscard]] float position() const;
    
    [[nodiscard]] float velocity() const;
    
    void set_limit(const PxJointLinearLimitPair &pair);
    
    [[nodiscard]] PxJointLinearLimitPair limit() const;
    
    void set_prismatic_joint_flags(const PxPrismaticJointFlags &flags);
    
    void set_prismatic_joint_flag(PxPrismaticJointFlag::Enum flag, bool value);
    
    [[nodiscard]] PxPrismaticJointFlags translational_joint_flags() const;
    
    void set_projection_linear_tolerance(float tolerance);
    
    [[nodiscard]] float projection_linear_tolerance() const;
    
    void set_projection_angular_tolerance(float tolerance);
    
    [[nodiscard]] float projection_angular_tolerance() const;
};

}

#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_TRANSLATIONAL_JOINT_H_ */
