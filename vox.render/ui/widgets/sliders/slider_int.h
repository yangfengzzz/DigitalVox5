//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_INT_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_INT_H_

#include "slider_single_scalar.h"

namespace vox::ui {
/**
 * Slider widget of type int
 */
class SliderInt : public SliderSingleScalar<int> {
public:
    /**
     * Constructor
     * @param p_min p_min
     * @param p_max p_max
     * @param p_value p_value
     * @param p_orientation p_orientation
     * @param p_label p_label
     * @param p_format p_format
     */
    explicit SliderInt(int p_min = 0,
                       int p_max = 100,
                       int p_value = 50,
                       SliderOrientation p_orientation = SliderOrientation::HORIZONTAL,
                       const std::string &p_label = "",
                       const std::string &p_format = "%d");
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_INT_H_ */
