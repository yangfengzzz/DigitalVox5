//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "panel.h"

namespace vox::ui {
uint64_t Panel::panel_id_increment_ = 0;

Panel::Panel() {
    panel_id_ = "##" + std::to_string(panel_id_increment_++);
}

void Panel::draw() {
    if (enabled_)
        draw_impl();
}

const std::string &Panel::panel_id() const {
    return panel_id_;
}

}
