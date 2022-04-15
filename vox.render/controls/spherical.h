//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_CONTROLS_SPHERICAL_H_
#define DIGITALVOX_VOX_RENDER_CONTROLS_SPHERICAL_H_

#include "vector3.h"

namespace vox::control {
// Spherical.
class Spherical {
public:
    explicit Spherical(float radius = 1.0, float phi = 0, float theta = 0);
    
    void set(float radius, float phi, float theta);
    
    void make_safe();
    
    void set_from_vec3(const Vector3F &v3);
    
    void set_to_vec3(Vector3F &v3) const;
    
private:
    friend class OrbitControl;
    
    friend class FreeControl;
    
    float radius_;
    float phi_;
    float theta_;
};

}

#endif /* DIGITALVOX_VOX_RENDER_CONTROLS_SPHERICAL_H_ */
