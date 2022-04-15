//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_SMALL_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_SMALL_H_

#include "button.h"
#include "vector2.h"
#include "color.h"

namespace vox::ui {
/**
 * Small button widget
 */
class ButtonSmall : public Button {
public:
    /**
     * Constructor
     */
    explicit ButtonSmall(std::string p_label = "");
    
protected:
    void draw_impl() override;
    
public:
    std::string label_;
    
    Color idle_background_color_;
    Color hovered_background_color_;
    Color clicked_background_color_;
    
    Color text_color_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_SMALL_H_ */
