//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "panel_menu_bar.h"

namespace vox::ui {
void PanelMenuBar::draw_impl() {
    if (!widgets_.empty() && ImGui::BeginMainMenuBar()) {
        draw_widgets();
        ImGui::EndMainMenuBar();
    }
}

}  // namespace vox::ui
