//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "panel_undecorated.h"

namespace vox::ui {
void PanelUndecorated::draw_impl() {
    auto &style = ImGui::GetStyle();
    ImVec2 previous_padding = style.WindowPadding;
    ImVec2 previous_min_size = style.WindowMinSize;
    style.WindowPadding = {0, 0};
    style.WindowMinSize = {0, 0};

    if (ImGui::Begin(panel_id_.c_str(), nullptr, collect_flags())) {
        style.WindowPadding = previous_padding;
        style.WindowMinSize = previous_min_size;

        update();

        draw_widgets();

        ImGui::End();
    } else {
        style.WindowPadding = previous_padding;
    }
}

int PanelUndecorated::collect_flags() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;

    if (auto_size_) flags |= ImGuiWindowFlags_AlwaysAutoResize;

    return flags;
}

}  // namespace vox::ui
