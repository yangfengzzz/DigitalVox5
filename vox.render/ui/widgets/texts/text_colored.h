//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_COLORED_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_COLORED_H_

#include "text.h"
#include "color.h"

namespace vox::ui {
/**
 * Widget to display text on a panel that can be colored
 */
class TextColored : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     * @param p_color p_color
     */
    explicit TextColored(const std::string &p_content = "",
                         const Color &p_color = Color(1.0f, 1.0f, 1.0f, 1.0f));
    
public:
    Color color_;
    
protected:
    void draw_impl() override;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_TEXTS_TEXT_COLORED_H_ */
