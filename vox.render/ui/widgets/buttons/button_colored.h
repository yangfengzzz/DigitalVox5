//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_COLORED_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_COLORED_H_

#include "button.h"
#include "vector2.h"
#include "color.h"

namespace vox::ui {
/**
 * Button widget of a single color (Color palette element)
 */
class ButtonColored : public Button {
public:
    /**
     * Constructor
     * @param p_label p_label
     * @param p_color p_color
     * @param p_size p_size
     * @param p_enable_alpha p_enableAlpha
     */
    explicit ButtonColored(std::string p_label = "", const Color &p_color = {},
                           const Vector2F &p_size = Vector2F(0.f, 0.f), bool p_enable_alpha = true);
    
protected:
    void draw_impl() override;
    
public:
    std::string label_;
    Color color_;
    Vector2F size_;
    bool enableAlpha_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_BUTTONS_BUTTON_COLORED_H_ */
