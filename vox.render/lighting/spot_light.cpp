//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spot_light.h"
#include "matrix_utils.h"
#include "entity.h"
#include "light_manager.h"

namespace vox {
std::string SpotLight::name() {
    return "SpotLight";
}

SpotLight::SpotLight(Entity *entity) :
Light(entity) {
}

void SpotLight::on_enable() {
    LightManager::get_singleton().attachSpotLight(this);
}

void SpotLight::on_disable() {
    LightManager::get_singleton().detachSpotLight(this);
}

void SpotLight::update_shader_data(SpotLightData &shader_data) {
    shader_data.color = Vector3F(color_.r * intensity_, color_.g * intensity_, color_.b * intensity_);
    auto position = entity()->transform_->world_position();
    shader_data.position = Vector3F(position.x, position.y, position.z);
    auto direction = entity()->transform_->world_forward();
    shader_data.direction = Vector3F(direction.x, direction.y, direction.z);
    shader_data.distance = distance_;
    shader_data.angle_cos = std::cos(angle_);
    shader_data.penumbra_cos = std::cos(angle_ + penumbra_);
}

// MARK: - Shadow
Matrix4x4F SpotLight::shadow_projection_matrix() {
    const auto kFov = std::min<float>(M_PI / 2, angle_ * 2.f * std::sqrt(2.f));
    return makePerspective<float>(kFov, 1, 0.1, distance_ + 5);
}

//MARK: - Reflection
void SpotLight::on_serialize(nlohmann::json &data) {
    
}

void SpotLight::on_deserialize(const nlohmann::json &data) {
    
}

void SpotLight::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
