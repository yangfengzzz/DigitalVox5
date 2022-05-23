//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/spot_light.h"

#include "vox.math/matrix_utils.h"
#include "vox.render/entity.h"
#include "vox.render/lighting/light_manager.h"

namespace vox {
std::string SpotLight::name() { return "SpotLight"; }

SpotLight::SpotLight(Entity *entity) : Light(entity) {}

void SpotLight::OnEnable() { LightManager::GetSingleton().AttachSpotLight(this); }

void SpotLight::OnDisable() { LightManager::GetSingleton().DetachSpotLight(this); }

void SpotLight::UpdateShaderData(SpotLightData &shader_data) {
    shader_data.color = Vector3F(color_.r * intensity_, color_.g * intensity_, color_.b * intensity_);
    auto position = GetEntity()->transform->WorldPosition();
    shader_data.position = Vector3F(position.x, position.y, position.z);
    auto direction = GetEntity()->transform->WorldForward();
    shader_data.direction = Vector3F(direction.x, direction.y, direction.z);
    shader_data.distance = distance_;
    shader_data.angle_cos = std::cos(angle_);
    shader_data.penumbra_cos = std::cos(angle_ + penumbra_);
}

// MARK: - Shadow
Matrix4x4F SpotLight::ShadowProjectionMatrix() {
    const auto kFov = std::min<float>(M_PI / 2, angle_ * 2.f * std::sqrt(2.f));
    return makePerspective<float>(kFov, 1, 0.1, distance_ + 5);
}

// MARK: - Reflection
void SpotLight::OnSerialize(nlohmann::json &data) {}

void SpotLight::OnDeserialize(const nlohmann::json &data) {}

void SpotLight::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
