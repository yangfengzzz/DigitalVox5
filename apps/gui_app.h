//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "forward_application.h"
#include "ui/canvas.h"
#include "ui/widgets/panel_transformables/panel_window.h"

namespace vox {
class GuiApp : public ForwardApplication {
public:
    void LoadScene() override;

private:
    ui::Canvas canvas_;
    ui::PanelWindow panel_;
};

}  // namespace vox
