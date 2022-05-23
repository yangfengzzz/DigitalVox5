//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/point_light.h"

#include "vox.math/matrix_utils.h"
#include "vox.render/entity.h"
#include "vox.render/lighting/light_manager.h"

namespace vox {
std::string PointLight::name() { return "PointLight"; }

PointLight::PointLight(Entity *entity) : Light(entity) {}

void PointLight::OnEnable() { LightManager::GetSingleton().AttachPointLight(this); }

void PointLight::OnDisable() { LightManager::GetSingleton().DetachPointLight(this); }

void PointLight::UpdateShaderData(PointLightData &shader_data) {
    shader_data.color = Vector3F(color_.r * intensity_, color_.g * intensity_, color_.b * intensity_);
    auto position = GetEntity()->transform->WorldPosition();
    shader_data.position = Vector3F(position.x, position.y, position.z);
    shader_data.distance = distance_;
}

// MARK: - Shadow
Matrix4x4F PointLight::ShadowProjectionMatrix() { return makePerspective<float>(degreesToRadians(120.f), 1, 0.1, 100); }

// MARK: - Reflection
void PointLight::OnSerialize(nlohmann::json &data) {}

void PointLight::OnDeserialize(const nlohmann::json &data) {}

void PointLight::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
