//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_ARROW_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_ARROW_H_

#include "button.h"

namespace vox::ui {
/**
 * Button widget with an arrow image
 */
class ButtonArrow : public Button {
public:
    /**
     * Create the button
     */
    explicit ButtonArrow(ImGuiDir p_direction = ImGuiDir_None);
    
protected:
    void draw_impl() override;
    
public:
    ImGuiDir direction_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_ARROW_H_ */
