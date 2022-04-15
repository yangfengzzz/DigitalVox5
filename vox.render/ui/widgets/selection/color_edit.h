//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_SELECTION_COLOR_EDIT_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_SELECTION_COLOR_EDIT_H_

#include "ui/widgets/data_widget.h"
#include "event.h"
#include "color.h"

namespace vox::ui {
/**
 * Widget that can open a color picker on click
 */
class ColorEdit : public DataWidget<Color> {
public:
    /**
     * Constructor
     * @param p_enable_alpha p_enableAlpha
     * @param p_default_color p_defaultColor
     */
    explicit ColorEdit(bool p_enable_alpha = false, const Color &p_default_color = {});
    
protected:
    void draw_impl() override;
    
public:
    bool enable_alpha_;
    Color color_;
    Event<Color &> color_changed_event_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_SELECTION_COLOR_EDIT_H_ */
