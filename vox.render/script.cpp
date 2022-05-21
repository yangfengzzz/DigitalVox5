//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "script.h"
#include "entity.h"
#include "scene.h"
#include "components_manager.h"
#include "physics/physics_manager.h"

namespace vox {
std::string Script::name() {
    return "Script";
}

Script::Script(Entity *entity) :
Component(entity) {
    
}

Script::~Script() {
    ComponentsManager::GetSingleton().add_destroy_component(this);
}

void Script::set_is_started(bool value) {
    started_ = value;
}

bool Script::is_started() const {
    return started_;
}

void Script::on_awake() {
    onAwake();
}

void Script::on_enable() {
    auto components_manager = ComponentsManager::GetSingletonPtr();
    if (!started_) {
        components_manager->add_on_start_script(this);
    }
    components_manager->add_on_update_script(this);
    physics::PhysicsManager::GetSingleton().add_on_physics_update_script(this);
    entity_->add_script(this);
    onEnable();
}

void Script::on_disable() {
    auto components_manager = ComponentsManager::GetSingletonPtr();
    components_manager->remove_on_start_script(this);
    components_manager->remove_on_update_script(this);
    physics::PhysicsManager::GetSingleton().remove_on_physics_update_script(this);
    entity_->remove_script(this);
    onDisable();
}

//MARK: - Reflection
void Script::on_serialize(nlohmann::json &data) {
    
}

void Script::on_deserialize(const nlohmann::json &data) {
    
}

void Script::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
