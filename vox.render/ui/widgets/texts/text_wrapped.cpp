//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_wrapped.h"

namespace vox::ui {
TextWrapped::TextWrapped(const std::string &content) :
Text(content) {
}

void TextWrapped::draw_impl() {
    ImGui::TextWrapped("%s", content_.c_str());
}

}
