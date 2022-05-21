//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "direct_light.h"
#include "entity.h"
#include "light_manager.h"

namespace vox {
std::string DirectLight::name() {
    return "DirectLight";
}

DirectLight::DirectLight(Entity *entity) :
Light(entity) {
}

void DirectLight::on_enable() {
	LightManager::GetSingleton().attach_direct_light(this);
}

void DirectLight::on_disable() {
	LightManager::GetSingleton().detach_direct_light(this);
}

void DirectLight::update_shader_data(DirectLightData &shader_data) {
    shader_data.color = Vector3F(color_.r * intensity_, color_.g * intensity_, color_.b * intensity_);
    auto direction = entity()->transform_->world_forward();
    shader_data.direction = Vector3F(direction.x, direction.y, direction.z);
}

//MARK: - Shadow
Vector3F DirectLight::direction() {
    return entity()->transform_->world_forward();
}

Matrix4x4F DirectLight::shadow_projection_matrix() {
    assert(false && "cascade shadow don't use this projection");
    throw std::exception();
}

//MARK: - Reflection
void DirectLight::on_serialize(nlohmann::json &data) {
    
}

void DirectLight::on_deserialize(const nlohmann::json &data) {
    
}

void DirectLight::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
