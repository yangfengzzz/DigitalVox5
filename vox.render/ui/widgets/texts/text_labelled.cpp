//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_labelled.h"

#include <utility>

namespace vox::ui {
TextLabelled::TextLabelled(const std::string &p_content, std::string p_label) :
Text(p_content), label_(std::move(p_label)) {
}

void TextLabelled::draw_impl() {
    ImGui::LabelText((label_ + widget_id_).c_str(), "%s", content_.c_str());
}

}