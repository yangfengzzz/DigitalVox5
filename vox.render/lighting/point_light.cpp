//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "point_light.h"
#include "matrix_utils.h"
#include "entity.h"
#include "light_manager.h"

namespace vox {
std::string PointLight::name() {
    return "PointLight";
}

PointLight::PointLight(Entity *entity) :
Light(entity) {
}

void PointLight::on_enable() {
    LightManager::get_singleton().attachPointLight(this);
}

void PointLight::on_disable() {
    LightManager::get_singleton().detachPointLight(this);
}

void PointLight::update_shader_data(PointLightData &shader_data) {
    shader_data.color = Vector3F(color_.r * intensity_, color_.g * intensity_, color_.b * intensity_);
    auto position = entity()->transform_->world_position();
    shader_data.position = Vector3F(position.x, position.y, position.z);
    shader_data.distance = distance_;
}

//MARK: - Shadow
Matrix4x4F PointLight::shadow_projection_matrix() {
    return makePerspective<float>(degreesToRadians(120.f), 1, 0.1, 100);
}

//MARK: - Reflection
void PointLight::on_serialize(nlohmann::json &data) {
    
}

void PointLight::on_deserialize(const nlohmann::json &data) {
    
}

void PointLight::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
