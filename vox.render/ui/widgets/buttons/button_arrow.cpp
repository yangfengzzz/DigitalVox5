//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "button_arrow.h"

namespace vox::ui {
ButtonArrow::ButtonArrow(ImGuiDir p_direction) :
direction_(p_direction) {
}

void ButtonArrow::draw_impl() {
    if (ImGui::ArrowButton(widget_id_.c_str(), direction_))
        clicked_event_.invoke();
}

}
