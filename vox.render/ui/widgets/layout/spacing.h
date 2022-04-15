//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_SPACING_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_SPACING_H_

#include "ui/widgets/widget.h"

namespace vox::ui {
/**
 * Widget that adds a space to the panel line
 */
class Spacing : public Widget {
public:
    /**
     * Constructor
     * @param p_spaces p_spaces
     */
    explicit Spacing(uint16_t p_spaces = 1);
    
protected:
    void draw_impl() override;
    
public:
    uint16_t spaces_ = 1;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_SPACING_H_ */
