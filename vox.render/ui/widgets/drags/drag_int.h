//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_INT_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_INT_H_

#include "drag_single_scalar.h"

namespace vox::ui {
/**
 * Drag widget of type int
 */
class DragInt : public DragSingleScalar<int> {
public:
    /**
     * Constructor
     * @param p_min p_min
     * @param p_max p_max
     * @param p_value p_value
     * @param p_speed p_speed
     * @param p_label p_label
     * @param p_format p_format
     */
    explicit DragInt(int p_min = 0,
                     int p_max = 100,
                     int p_value = 50,
                     float p_speed = 0.1f,
                     const std::string &p_label = "",
                     const std::string &p_format = "%d");
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_INT_H_ */
