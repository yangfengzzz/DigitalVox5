//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_DUMMY_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_DUMMY_H_

#include "vector2.h"

#include "ui/widgets/widget.h"

namespace vox::ui {
/**
 * Dummy widget that takes the given size as space in the panel
 */
class Dummy : public Widget {
public:
    /**
     * Constructor
     * @param p_size p_size
     */
    explicit Dummy(const Vector2F &p_size = {0.0f, 0.0f});
    
protected:
    void draw_impl() override;
    
public:
    Vector2F size_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_LAYOUT_DUMMY_H_ */
