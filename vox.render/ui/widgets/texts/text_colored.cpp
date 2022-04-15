//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_colored.h"
#include "ui/widgets/converter.h"

namespace vox::ui {
TextColored::TextColored(const std::string &p_content,
                         const Color &p_color) :
Text(p_content), color_(p_color) {
}

void TextColored::draw_impl() {
    ImGui::TextColored(Converter::to_imVec4(color_), "%s", content_.c_str());
}

}