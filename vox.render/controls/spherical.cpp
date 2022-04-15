//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spherical.h"
#include "constants.h"

namespace vox::control {
Spherical::Spherical(float radius, float phi, float theta) :
radius_(radius),
phi_(phi),
theta_(theta) {
}

void Spherical::set(float radius, float phi, float theta) {
    radius_ = radius;
    phi_ = phi;
    theta_ = theta;
}

void Spherical::make_safe() {
    phi_ = clamp<float>(phi_, kEpsilonF, M_PI - kEpsilonF);
}

void Spherical::set_from_vec3(const Vector3F &v3) {
    radius_ = v3.length();
    if (radius_ == 0) {
        theta_ = 0;
        phi_ = 0;
    } else {
        theta_ = std::atan2(v3.x, v3.z);
        phi_ = std::acos(clamp<float>(v3.y / radius_, -1, 1));
    }
}

void Spherical::set_to_vec3(Vector3F &v3) const {
    const auto kSinPhiRadius = std::sin(phi_) * radius_;
    
    v3.x = kSinPhiRadius * std::sin(theta_);
    v3.y = std::cos(phi_) * radius_;
    v3.z = kSinPhiRadius * std::cos(theta_);
}

}
