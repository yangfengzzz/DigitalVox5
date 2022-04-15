//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_FLOAT_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_FLOAT_H_

#include "drag_single_scalar.h"

namespace vox::ui {
/**
 * Drag widget of type double
 */
class DragFloat : public DragSingleScalar<float> {
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
    explicit DragFloat(float p_min = 0.0,
                       float p_max = 1.0,
                       float p_value = 0.5,
                       float p_speed = 0.1f,
                       const std::string &p_label = "",
                       const std::string &p_format = "%.3f");
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_DRAGS_DRAG_FLOAT_H_ */
