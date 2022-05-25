//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/logging.h"
#include "vox.render/behaviour.h"

namespace vox {
template <typename... Args>
inline void Behaviour::BehaviourCall(const std::string &function_name, Args &&...args) {
    if (object_.valid()) {
        if (object_[function_name].valid()) {
            sol::protected_function pfr = object_[function_name];
            auto pfr_result = pfr.call(object_, std::forward<Args>(args)...);
            if (!pfr_result.valid()) {
                sol::error err = pfr_result;
                LOGE("{}", err.what())
            }
        }
    }
}

}  // namespace vox
