//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "radio_button.h"

#include <utility>

namespace vox::ui {
RadioButton::RadioButton(bool selected, std::string label) :
DataWidget<bool>(selected_), label_(std::move(label)) {
    if (selected)
        select();
}

void RadioButton::select() {
    selected_ = true;
    clicked_event_.invoke(radio_id_);
}

bool RadioButton::is_selected() const {
    return selected_;
}

bool RadioButton::radio_id() const {
    return radio_id_;
}

void RadioButton::draw_impl() {
    if (ImGui::RadioButton((label_ + widget_id_).c_str(), selected_)) {
        clicked_event_.invoke(radio_id_);
        notify_change();
    }
}

}
