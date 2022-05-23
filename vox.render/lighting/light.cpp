//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/light.h"

#include "vox.render/entity.h"

namespace vox {
Light::Light(Entity *entity) : Component(entity) {}

Matrix4x4F Light::ViewMatrix() { return GetEntity()->transform->WorldMatrix().inverse(); }

Matrix4x4F Light::InverseViewMatrix() { return GetEntity()->transform->WorldMatrix(); }

// MARK: - Shadow
bool Light::EnableShadow() const { return enable_shadow_; }

void Light::SetEnableShadow(bool enabled) { enable_shadow_ = enabled; }

float Light::ShadowBias() const { return shadow_bias_; }

void Light::SetShadowBias(float value) { shadow_bias_ = value; }

float Light::ShadowIntensity() const { return shadow_intensity_; }

void Light::SetShadowIntensity(float value) { shadow_intensity_ = value; }

float Light::ShadowRadius() const { return shadow_radius_; }

void Light::SetShadowRadius(float value) { shadow_radius_ = value; }

}  // namespace vox
