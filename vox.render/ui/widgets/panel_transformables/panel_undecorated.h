//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANEL_TRANSFORMABLES_PANEL_UNDECORATED_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANEL_TRANSFORMABLES_PANEL_UNDECORATED_H_

#include "ui/widgets/panel_transformable.h"

namespace vox::ui {
/**
 * A simple panel that is transformable and without decorations (No background)
 */
class PanelUndecorated : public PanelTransformable {
public:
    void draw_impl() override;
    
private:
    int collect_flags();
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_PANEL_TRANSFORMABLES_PANEL_UNDECORATED_H_ */