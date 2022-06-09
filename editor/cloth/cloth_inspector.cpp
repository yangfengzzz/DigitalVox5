//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor/cloth/cloth_inspector.h"

#include "vox.render/ui/widgets/layout/spacing.h"

namespace vox::editor::ui {
namespace {
class ClothUI : public vox::ui::Widget {
    void DrawImpl() override { ImGui::Text("hahha"); }
};

}  // namespace

ClothInspector::ClothInspector(const std::string &title, bool opened, const PanelWindowSettings &window_settings)
    : PanelWindow(title, opened, window_settings) {
    CreateWidget<ClothUI>();
}

}  // namespace vox::editor::ui
