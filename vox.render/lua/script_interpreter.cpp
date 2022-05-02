//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "script_interpreter.h"

#include <utility>
#include "lua_binder.h"

namespace vox {
ScriptInterpreter *ScriptInterpreter::get_singleton_ptr() {
    return ms_singleton_;
}

ScriptInterpreter &ScriptInterpreter::get_singleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

ScriptInterpreter::ScriptInterpreter(std::string script_root_folder) :
script_root_folder_(std::move(script_root_folder)) {
    create_lua_context_and_bind_globals();
    
    /* Listen to behaviours */
    Behaviour::created_event_ += std::bind(&ScriptInterpreter::consider, this, std::placeholders::_1);
    Behaviour::destroyed_event_ += std::bind(&ScriptInterpreter::unconsider, this, std::placeholders::_1);
}

ScriptInterpreter::~ScriptInterpreter() {
    destroy_lua_context();
}

void ScriptInterpreter::create_lua_context_and_bind_globals() {
    if (!lua_state_) {
        lua_state_ = std::make_unique<sol::state>();
        lua_state_->open_libraries(sol::lib::base, sol::lib::math);
        LuaBinder::call_binders(*lua_state_);
        is_ok_ = true;
        
        std::for_each(behaviours_.begin(), behaviours_.end(), [this](Behaviour *behaviour) {
            if (!behaviour->register_to_lua_context(*lua_state_, script_root_folder_))
                is_ok_ = false;
        });
        
        if (!is_ok_)
            LOGE("Script interpreter failed to register scripts. Check your lua scripts\n")
            }
}

void ScriptInterpreter::destroy_lua_context() {
    if (lua_state_) {
        std::for_each(behaviours_.begin(), behaviours_.end(), [](Behaviour *behaviour) {
            behaviour->unregister_from_lua_context();
        });
        
        lua_state_.reset();
        is_ok_ = false;
    }
}

void ScriptInterpreter::consider(Behaviour *to_consider) {
    if (lua_state_) {
        behaviours_.push_back(to_consider);
        
        if (!to_consider->register_to_lua_context(*lua_state_, script_root_folder_))
            is_ok_ = false;
    }
}

void ScriptInterpreter::unconsider(Behaviour *to_unconsider) {
    if (lua_state_)
        to_unconsider->unregister_from_lua_context();
    
    behaviours_.erase(std::remove_if(behaviours_.begin(), behaviours_.end(), [to_unconsider](Behaviour *behaviour) {
        return to_unconsider == behaviour;
    }), behaviours_.end());
    
    refresh_all(); // Unconsidering a script is impossible with Lua, we have to reparse every behaviours
}

void ScriptInterpreter::refresh_all() {
    destroy_lua_context();
    create_lua_context_and_bind_globals();
}

bool ScriptInterpreter::is_ok() const {
    return is_ok_;
}

}
