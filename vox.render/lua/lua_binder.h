//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <sol/sol.hpp>

namespace vox {
/**
 * Handle lua binding
 */
class LuaBinder {
public:
    /**
     * Bind engine features to lua
     */
    static void CallBinders(sol::state &lua_state);
};

}  // namespace vox
