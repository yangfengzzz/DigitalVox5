//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "button_colored.h"

#include <utility>
#include "ui/widgets/converter.h"

namespace vox::ui {
ButtonColored::ButtonColored(std::string label, const Color &color,
                             const Vector2F &size, bool enable_alpha) :
label_(std::move(label)), color_(color), size_(size), enableAlpha_(enable_alpha) {
}

void ButtonColored::draw_impl() {
    ImVec4 imColor = Converter::to_imVec4(color_);
    
    if (ImGui::ColorButton((label_ + widget_id_).c_str(), imColor,
                           !enableAlpha_ ? ImGuiColorEditFlags_NoAlpha : 0,
                           Converter::to_imVec2(size_)))
        clicked_event_.invoke();
    
    color_ = Converter::to_color(imColor);
}

}
