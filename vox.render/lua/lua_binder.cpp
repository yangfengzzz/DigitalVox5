//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "lua_binder.h"

#include "lua/lua_component_binder.h"
#include "lua/lua_entity_binder.h"
#include "lua/lua_global_binder.h"
#include "lua/lua_math_binder.h"

namespace vox {
void LuaBinder::call_binders(sol::state &lua_state) {
    LuaMathsBinder::bind_maths(lua_state);
    LuaEntityBinder::bind_entity(lua_state);
    LuaComponentBinder::bind_component(lua_state);
    LuaGlobalsBinder::bind_globals(lua_state);
}

}  // namespace vox
