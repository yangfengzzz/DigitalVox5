//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light.h"
#include "entity.h"

namespace vox {
Light::Light(Entity *entity) :
Component(entity) {
}

Matrix4x4F Light::view_matrix() {
    return entity()->transform_->world_matrix().inverse();
}

Matrix4x4F Light::inverse_view_matrix() {
    return entity()->transform_->world_matrix();
}

//MARK: - Shadow
bool Light::enable_shadow() const {
    return enable_shadow_;
}

void Light::set_enable_shadow(bool enabled) {
    enable_shadow_ = enabled;
}

float Light::shadow_bias() const {
    return shadow_bias_;
}

void Light::set_shadow_bias(float value) {
    shadow_bias_ = value;
}

float Light::shadow_intensity() const {
    return shadow_intensity_;
}

void Light::set_shadow_intensity(float value) {
    shadow_intensity_ = value;
}

float Light::shadow_radius() const {
    return shadow_radius_;
}

void Light::set_shadow_radius(float value) {
    shadow_radius_ = value;
}

}
