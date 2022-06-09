//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.cloth/cloth_controller.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class ClothInspector : public PanelWindow {
public:
    ClothInspector(const std::string &p_new_name, bool opened, const PanelWindowSettings &window_settings,
                   cloth::ClothController& controller);
};

}  // namespace editor::ui
}  // namespace vox
