//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_selectable.h"

namespace vox::ui {
TextSelectable::TextSelectable(const std::string &p_content, bool p_selected, bool p_disabled) :
Text(p_content), selected_(p_selected), disabled_(p_disabled) {
}

void TextSelectable::draw_impl() {
    if (ImGui::Selectable((content_ + widget_id_).c_str(), &selected_,
                          disabled_ ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None)) {
        clicked_event_.invoke(selected_);
        
        if (selected_)
            selected_event_.invoke();
        else
            unselected_event_.invoke();
    }
}

}