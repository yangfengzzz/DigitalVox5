//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANELS_PANEL_MENU_BAR_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANELS_PANEL_MENU_BAR_H_

#include "ui/widgets/panel.h"

namespace vox::ui {
/**
 * A simple panel that will be displayed on the top side of the canvas
 */
class PanelMenuBar : public Panel {
protected:
    void draw_impl() override;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANELS_PANEL_MENU_BAR_H_ */
