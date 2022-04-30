//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "behaviour.h"
#include "entity.h"
#include "ui/widgets/texts/text_colored.h"

namespace vox {
std::string Behaviour::name() {
    return "Behaviour";
}

Behaviour::Behaviour(Entity *entity) :
Script(entity) {
    created_event_.invoke(this);
}

Behaviour::~Behaviour() {
    destroyed_event_.invoke(this);
}

bool Behaviour::register_to_lua_context(sol::state &lua_state, const std::string &script_folder) {
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

void Behaviour::unregister_from_lua_context() {
    object_ = sol::lua_nil;
}

sol::table &Behaviour::table() {
    return object_;
}

//MARK: - Lua Call

void Behaviour::onAwake() {
    behaviour_call("onAwake");
}

void Behaviour::onEnable() {
    behaviour_call("onEnable");
}

void Behaviour::onDisable() {
    behaviour_call("onDisable");
}

void Behaviour::on_destroy() {
    behaviour_call("onDestroy");
}

void Behaviour::on_start() {
    behaviour_call("onStart");
}

void Behaviour::on_update(float delta_time) {
    behaviour_call("onUpdate", delta_time);
}

void Behaviour::on_late_update(float delta_time) {
    behaviour_call("onLateUpdate", delta_time);
}

void Behaviour::on_begin_render(Camera *camera) {
}

void Behaviour::on_end_render(Camera *camera) {
}

void Behaviour::on_trigger_enter(const physics::ColliderShapePtr &other) {
}

void Behaviour::on_trigger_exit(const physics::ColliderShapePtr &other) {
}

void Behaviour::on_trigger_stay(const physics::ColliderShapePtr &other) {
}

void Behaviour::input_event(const InputEvent &input_event) {
}

void Behaviour::resize(uint32_t win_width, uint32_t win_height,
                       uint32_t fb_width, uint32_t fb_height) {
}

void Behaviour::on_inspector(ui::WidgetContainer &root) {
    if (object_.valid()) {
        root.create_widget<ui::TextColored>("Ready", Color::Green);
        root.create_widget<ui::TextColored>("Your script gets interpreted by the engine with success", Color::White);
    } else {
        root.create_widget<ui::TextColored>("Compilation failed!", Color::Red);
        root.create_widget<ui::TextColored>("Check the console for more information", Color::White);
    }
}

}
