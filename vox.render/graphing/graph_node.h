//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <json.hpp>

namespace vox {
namespace graphing {
class Node {
public:
    Node() {};
    
    Node(size_t id, const char *title, const char *style = NULL, const nlohmann::json &data = {});
    
    template<typename T>
    static std::uintptr_t handle_to_uintptr_t(T handle) {
        return reinterpret_cast<std::uintptr_t>(reinterpret_cast<void *>(handle));
    }
    
    nlohmann::json attributes;
};
}        // namespace graphing
}        // namespace vox
