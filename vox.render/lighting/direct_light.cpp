//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/direct_light.h"

#include "vox.render/entity.h"
#include "vox.render/lighting/light_manager.h"

namespace vox {
std::string DirectLight::name() { return "DirectLight"; }

DirectLight::DirectLight(Entity *entity) : Light(entity) {}

void DirectLight::OnEnable() { LightManager::GetSingleton().AttachDirectLight(this); }

void DirectLight::OnDisable() { LightManager::GetSingleton().DetachDirectLight(this); }

void DirectLight::UpdateShaderData(DirectLightData &shader_data) {
    shader_data.color = Vector3F(color_.r * intensity_, color_.g * intensity_, color_.b * intensity_);
    auto direction = GetEntity()->transform->WorldForward();
    shader_data.direction = Vector3F(direction.x, direction.y, direction.z);
}

// MARK: - Shadow
Vector3F DirectLight::Direction() { return GetEntity()->transform->WorldForward(); }

Matrix4x4F DirectLight::ShadowProjectionMatrix() {
    assert(false && "cascade shadow don't use this projection");
    throw std::exception();
}

// MARK: - Reflection
void DirectLight::OnSerialize(nlohmann::json &data) {}

void DirectLight::OnDeserialize(const nlohmann::json &data) {}

void DirectLight::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
