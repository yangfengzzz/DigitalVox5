//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "menu_item.h"

#include <utility>

namespace vox::ui {
MenuItem::MenuItem(std::string p_name, std::string p_shortcut,
                   bool p_checkable, bool p_checked) :
DataWidget(selected_), name_(std::move(p_name)), shortcut_(std::move(p_shortcut)),
checkable_(p_checkable), checked_(p_checked) {
}

void MenuItem::draw_impl() {
    bool previous_value = checked_;
    
    if (ImGui::MenuItem((name_ + widget_id_).c_str(), shortcut_.c_str(),
                        checkable_ ? &checked_ : nullptr, enabled_))
        clicked_event_.invoke();
    
    if (checked_ != previous_value) {
        value_changed_event_.invoke(checked_);
        notify_change();
    }
}

}
