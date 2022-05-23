//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lua/lua_binder.h"

#include "vox.render/lua/lua_component_binder.h"
#include "vox.render/lua/lua_entity_binder.h"
#include "vox.render/lua/lua_global_binder.h"
#include "vox.render/lua/lua_math_binder.h"

namespace vox {
void LuaBinder::CallBinders(sol::state &lua_state) {
    LuaMathsBinder::BindMaths(lua_state);
    LuaEntityBinder::BindEntity(lua_state);
    LuaComponentBinder::BindComponent(lua_state);
    LuaGlobalsBinder::BindGlobals(lua_state);
}

}  // namespace vox
