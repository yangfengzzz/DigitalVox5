//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "input_text.h"

#include <utility>

namespace vox::ui {
InputText::InputText(std::string p_content, std::string p_label) :
DataWidget<std::string>(content_), content_(std::move(p_content)), label_(std::move(p_label)) {
}

void InputText::draw_impl() {
    std::string previous_content = content_;
    
    content_.resize(256, '\0');
    bool enter_pressed = ImGui::InputText((label_ + widget_id_).c_str(), &content_[0], 256,
                                          ImGuiInputTextFlags_EnterReturnsTrue
                                          | (select_all_on_click_ ? ImGuiInputTextFlags_AutoSelectAll : 0));
    
    if (content_ != previous_content) {
        content_changed_event_.invoke(content_);
        notify_change();
    }
    
    if (enter_pressed)
        enter_pressed_event_.invoke(content_);
}

}