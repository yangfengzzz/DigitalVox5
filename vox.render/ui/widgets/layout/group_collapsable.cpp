//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "group_collapsable.h"

#include <utility>

namespace vox::ui {
GroupCollapsable::GroupCollapsable(std::string name) :
name_(std::move(name)) {
}

void GroupCollapsable::draw_impl() {
    bool previously_opened = opened_;
    
    if (ImGui::CollapsingHeader(name_.c_str(), closable_ ? &opened_ : nullptr))
        Group::draw_impl();
    
    if (opened_ != previously_opened) {
        if (opened_)
            open_event_.invoke();
        else
            close_event_.invoke();
    }
}

}
