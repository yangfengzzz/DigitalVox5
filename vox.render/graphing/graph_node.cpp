//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "graphing/graph_node.h"

namespace vox {
namespace graphing {
Node::Node(size_t id, const char *title, const char *style, const nlohmann::json &data) {
    attributes["id"] = id;
    attributes["label"] = title;
    attributes["data"] = data;
    attributes["style"] = style;
}
}        // namespace graphing
}        // namespace vox
