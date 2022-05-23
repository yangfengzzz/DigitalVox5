//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lua/script_interpreter.h"

#include <utility>

#include "vox.render/lua/lua_binder.h"

namespace vox {
ScriptInterpreter *ScriptInterpreter::GetSingletonPtr() { return ms_singleton; }

ScriptInterpreter &ScriptInterpreter::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

ScriptInterpreter::ScriptInterpreter(std::string script_root_folder)
    : script_root_folder_(std::move(script_root_folder)) {
    CreateLuaContextAndBindGlobals();

    /* Listen to behaviours */
    Behaviour::created_event_ += std::bind(&ScriptInterpreter::Consider, this, std::placeholders::_1);
    Behaviour::destroyed_event_ += std::bind(&ScriptInterpreter::Unconsider, this, std::placeholders::_1);
}

ScriptInterpreter::~ScriptInterpreter() { DestroyLuaContext(); }

void ScriptInterpreter::CreateLuaContextAndBindGlobals() {
    if (!lua_state_) {
        lua_state_ = std::make_unique<sol::state>();
        lua_state_->open_libraries(sol::lib::base, sol::lib::math);
        LuaBinder::CallBinders(*lua_state_);
        is_ok_ = true;

        std::for_each(behaviours_.begin(), behaviours_.end(), [this](Behaviour *behaviour) {
            if (!behaviour->RegisterToLuaContext(*lua_state_, script_root_folder_)) is_ok_ = false;
        });

        if (!is_ok_) LOGE("Script interpreter failed to register scripts. Check your lua scripts\n")
    }
}

void ScriptInterpreter::DestroyLuaContext() {
    if (lua_state_) {
        std::for_each(behaviours_.begin(), behaviours_.end(),
                      [](Behaviour *behaviour) { behaviour->UnregisterFromLuaContext(); });

        lua_state_.reset();
        is_ok_ = false;
    }
}

void ScriptInterpreter::Consider(Behaviour *to_consider) {
    if (lua_state_) {
        behaviours_.push_back(to_consider);

        if (!to_consider->RegisterToLuaContext(*lua_state_, script_root_folder_)) is_ok_ = false;
    }
}

void ScriptInterpreter::Unconsider(Behaviour *to_unconsider) {
    if (lua_state_) to_unconsider->UnregisterFromLuaContext();

    behaviours_.erase(std::remove_if(behaviours_.begin(), behaviours_.end(),
                                     [to_unconsider](Behaviour *behaviour) { return to_unconsider == behaviour; }),
                      behaviours_.end());

    RefreshAll();  // Unconsidering a script is impossible with Lua, we have to reparse every behaviours
}

void ScriptInterpreter::RefreshAll() {
    DestroyLuaContext();
    CreateLuaContextAndBindGlobals();
}

bool ScriptInterpreter::IsOk() const { return is_ok_; }

}  // namespace vox
