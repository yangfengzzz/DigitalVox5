//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "check_box.h"

#include <utility>

namespace vox::ui {
CheckBox::CheckBox(bool p_value, std::string p_label) :
DataWidget<bool>(value_), value_(p_value), label_(std::move(p_label)) {
}

void CheckBox::draw_impl() {
    bool previous_value = value_;
    
    ImGui::Checkbox((label_ + widget_id_).c_str(), &value_);
    
    if (value_ != previous_value) {
        value_changed_event_.invoke(value_);
        notify_change();
    }
}

}
