//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <sol/sol.hpp>
#include <vector>

#include "behaviour.h"
#include "singleton.h"

namespace vox {
/**
 * Handles script interpretation
 */
class ScriptInterpreter : public Singleton<ScriptInterpreter> {
public:
    static ScriptInterpreter &GetSingleton();

    static ScriptInterpreter *GetSingletonPtr();

    /**
     * Constructor
     */
    ScriptInterpreter(std::string script_root_folder);

    /**
     * Destructor
     */
    ~ScriptInterpreter();

    /**
     * Handle the creation of the lua context and bind engine functions to lua
     */
    void create_lua_context_and_bind_globals();

    /**
     * Destroy the lua context
     */
    void destroy_lua_context();

    /**
     * Consider a behaviour
     */
    void consider(Behaviour *to_consider);

    /**
     * Unconsider a behaviour
     */
    void unconsider(Behaviour *to_unconsider);

    /**
     * Refresh every scripts
     */
    void refresh_all();

    /**
     * Returns true if every scripts are OK
     */
    bool is_ok() const;

private:
    std::unique_ptr<sol::state> lua_state_{nullptr};
    std::string script_root_folder_;
    std::vector<Behaviour *> behaviours_;
    bool is_ok_{};
};

template <>
inline ScriptInterpreter *Singleton<ScriptInterpreter>::ms_singleton{nullptr};

}  // namespace vox
