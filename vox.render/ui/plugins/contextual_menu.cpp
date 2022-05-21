//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "contextual_menu.h"

namespace vox::ui {
void ContextualMenu::Execute() {
    if (ImGui::BeginPopupContextItem()) {
        DrawWidgets();
        ImGui::EndPopup();
    }
}

void ContextualMenu::Close() { ImGui::CloseCurrentPopup(); }

}  // namespace vox::ui
