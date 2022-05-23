//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/buttons/button_image.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"

namespace vox {
using namespace ui;

namespace editor {
class EditorResources;

namespace ui {
class Toolbar : public PanelWindow {
public:
    Toolbar(const std::string &new_mode,
            bool opened,
            const PanelWindowSettings &window_settings,
            EditorResources *resource);

    /**
     * Custom implementation of the draw method
     */
    void DrawImpl() override;

private:
    EditorResources *resource_{nullptr};
    ButtonImage *play_button_{nullptr};
    ButtonImage *pause_button_{nullptr};
    ButtonImage *stop_button_{nullptr};
    ButtonImage *next_button_{nullptr};
};

}  // namespace ui
}  // namespace editor
}  // namespace vox
