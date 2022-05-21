//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "script.h"

#include "components_manager.h"
#include "entity.h"
#include "physics/physics_manager.h"
#include "scene.h"

namespace vox {
std::string Script::name() { return "Script"; }

Script::Script(vox::Entity *entity) : Component(entity) {}

Script::~Script() { ComponentsManager::GetSingleton().AddDestroyComponent(this); }

void Script::SetIsStarted(bool value) { started_ = value; }

bool Script::IsStarted() const { return started_; }

void Script::OnAwake() { OnScriptAwake(); }

void Script::OnEnable() {
    auto components_manager = ComponentsManager::GetSingletonPtr();
    if (!started_) {
        components_manager->AddOnStartScript(this);
    }
    components_manager->AddOnUpdateScript(this);
    physics::PhysicsManager::GetSingleton().add_on_physics_update_script(this);
    entity_->AddScript(this);
    OnScriptEnable();
}

void Script::OnDisable() {
    auto components_manager = ComponentsManager::GetSingletonPtr();
    components_manager->RemoveOnStartScript(this);
    components_manager->RemoveOnUpdateScript(this);
    physics::PhysicsManager::GetSingleton().remove_on_physics_update_script(this);
    entity_->RemoveScript(this);
    OnScriptDisable();
}

// MARK: - Reflection
void Script::OnSerialize(nlohmann::json &data) {}

void Script::OnDeserialize(const nlohmann::json &data) {}

void Script::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
