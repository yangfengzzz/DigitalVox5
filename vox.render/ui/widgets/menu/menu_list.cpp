//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "menu_list.h"

#include <utility>

namespace vox::ui {
MenuList::MenuList(std::string p_name, bool p_locked) :
name_(std::move(p_name)), locked_(p_locked) {
}

void MenuList::draw_impl() {
    if (ImGui::BeginMenu(name_.c_str(), !locked_)) {
        if (!opened_) {
            clicked_event_.invoke();
            opened_ = true;
        }
        
        draw_widgets();
        ImGui::EndMenu();
    } else {
        opened_ = false;
    }
}

}
