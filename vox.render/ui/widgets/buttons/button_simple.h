//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_SIMPLE_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_SIMPLE_H_

#include "button.h"
#include "vector2.h"
#include "color.h"

namespace vox::ui {
/**
 * Simple button widget
 */
class ButtonSimple : public Button {
public:
    /**
     * Constructor
     * @param p_label p_label
     * @param p_size p_size
     * @param p_disabled p_disabled
     */
    explicit ButtonSimple(std::string p_label = "",
                          const Vector2F &p_size = Vector2F(0.f, 0.f),
                          bool p_disabled = false);
    
protected:
    void draw_impl() override;
    
public:
    std::string label_;
    Vector2F size_;
    bool disabled_ = false;
    
    Color idle_background_color_;
    Color hovered_background_color_;
    Color clicked_background_color_;
    
    Color text_color_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_SIMPLE_H_ */
