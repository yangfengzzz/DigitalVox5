//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/behaviour.h"

#include "vox.render/entity.h"
#include "vox.render/ui/widgets/texts/text_colored.h"

namespace vox {
Event<Behaviour *> Behaviour::created_event_;
Event<Behaviour *> Behaviour::destroyed_event_;

std::string Behaviour::name() { return "Behaviour"; }

Behaviour::Behaviour(Entity *entity) : Script(entity) { created_event_.Invoke(this); }

Behaviour::~Behaviour() { destroyed_event_.Invoke(this); }

bool Behaviour::RegisterToLuaContext(sol::state &lua_state, const std::string &script_folder) {
    auto result = lua_state.safe_script_file(script_folder + script_name_ + ".lua", &sol::script_pass_on_error);

    if (!result.valid()) {
        sol::error err = result;
        LOGE("{}", err.what())
        return false;
    } else {
        if (result.return_count() == 1 && result[0].is<sol::table>()) {
            object_ = result[0];
            object_["owner"] = entity_;
            return true;
        } else {
            LOGE("'{}.lua' missing return expression\n", script_name_)
            return false;
        }
    }
}

void Behaviour::UnregisterFromLuaContext() { object_ = sol::lua_nil; }

sol::table &Behaviour::Table() { return object_; }

// MARK: - Lua Call

void Behaviour::OnScriptAwake() { BehaviourCall("OnScriptAwake"); }

void Behaviour::OnScriptEnable() { BehaviourCall("OnScriptEnable"); }

void Behaviour::OnScriptDisable() { BehaviourCall("OnScriptDisable"); }

void Behaviour::OnDestroy() { BehaviourCall("OnDestroy"); }

void Behaviour::OnStart() { BehaviourCall("OnStart"); }

void Behaviour::OnUpdate(float delta_time) { BehaviourCall("OnUpdate", delta_time); }

void Behaviour::OnLateUpdate(float delta_time) { BehaviourCall("OnLateUpdate", delta_time); }

void Behaviour::OnBeginRender(Camera *camera) {}

void Behaviour::OnEndRender(Camera *camera) {}

void Behaviour::OnTriggerEnter(const physics::ColliderShapePtr &other) {}

void Behaviour::OnTriggerExit(const physics::ColliderShapePtr &other) {}

void Behaviour::OnTriggerStay(const physics::ColliderShapePtr &other) {}

void Behaviour::InputEvent(const vox::InputEvent &input_event) {}

void Behaviour::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {}

void Behaviour::OnInspector(ui::WidgetContainer &root) {
    if (object_.valid()) {
        root.CreateWidget<ui::TextColored>("Ready", Color::green);
        root.CreateWidget<ui::TextColored>("Your script gets interpreted by the engine with success", Color::white);
    } else {
        root.CreateWidget<ui::TextColored>("Compilation failed!", Color::red);
        root.CreateWidget<ui::TextColored>("Check the console for more information", Color::white);
    }
}

}  // namespace vox
