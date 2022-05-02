//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text.h"

#include <utility>

namespace vox::ui {
Text::Text(std::string content) :
DataWidget(content_),
content_(std::move(content)) {
}

void Text::draw_impl() {
    ImGui::Text("%s", content_.c_str());
}

}
