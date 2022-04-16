//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_SPHERICAL_JOINT_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_SPHERICAL_JOINT_H_

#include "joint.h"

namespace vox::physics {
/**
 * A joint which behaves in a similar way to a ball and socket.
 */
class SphericalJoint : public Joint {
public:
    SphericalJoint(Collider *collider_0, Collider *collider_1);
    
    [[nodiscard]] PxJointLimitCone limit_cone() const;
    
    void set_limit_cone(const PxJointLimitCone &limit);
    
    [[nodiscard]] float swing_y_angle() const;
    
    [[nodiscard]] float swing_z_angle() const;
    
    void set_spherical_joint_flags(const PxSphericalJointFlags &flags);
    
    void set_spherical_joint_flag(PxSphericalJointFlag::Enum flag, bool value);
    
    [[nodiscard]] PxSphericalJointFlags spherical_joint_flags() const;
    
    void set_projection_linear_tolerance(float tolerance);
    
    [[nodiscard]] float projection_linear_tolerance() const;
};

}
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_SPHERICAL_JOINT_H_ */
