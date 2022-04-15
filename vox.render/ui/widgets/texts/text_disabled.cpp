//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_disabled.h"

namespace vox::ui {
TextDisabled::TextDisabled(const std::string &p_content) :
Text(p_content) {
}

void TextDisabled::draw_impl() {
    ImGui::TextDisabled("%s", content_.c_str());
}

}