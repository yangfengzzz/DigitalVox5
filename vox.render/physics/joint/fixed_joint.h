//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_FIXED_JOINT_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_FIXED_JOINT_H_

#include "joint.h"

namespace vox::physics {
/*
 * A fixed joint permits no relative movement between two bodies. ie the bodies are glued together.
 */
class FixedJoint : public Joint {
public:
    FixedJoint(Collider *collider_0, Collider *collider_1);
    
    void set_projection_linear_tolerance(float tolerance);
    
    [[nodiscard]] float projection_linear_tolerance() const;
    
    void set_projection_angular_tolerance(float tolerance);
    
    [[nodiscard]] float projection_angular_tolerance() const;
};

}

#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_JOINT_FIXED_JOINT_H_ */
