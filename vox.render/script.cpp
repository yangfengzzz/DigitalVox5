//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "script.h"
#include "entity.h"
#include "scene.h"
#include "components_manager.h"

namespace vox {
std::string Script::name() {
    return "Script";
}

Script::Script(Entity *entity) :
Component(entity) {
    
}

Script::~Script() {
    ComponentsManager::getSingleton().addDestroyComponent(this);
}

void Script::setIsStarted(bool value) {
    _started = value;
}

bool Script::isStarted() {
    return _started;
}

void Script::on_awake() {
    onAwake();
}

void Script::on_enable() {
    auto componentsManager = ComponentsManager::getSingletonPtr();
    if (!_started) {
        componentsManager->addOnStartScript(this);
    }
    componentsManager->addOnUpdateScript(this);
    entity_->add_script(this);
    onEnable();
}

void Script::on_disable() {
    auto componentsManager = ComponentsManager::getSingletonPtr();
    componentsManager->removeOnStartScript(this);
    componentsManager->removeOnUpdateScript(this);
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
