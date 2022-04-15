//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_MULTIPLE_DOUBLES_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_MULTIPLE_DOUBLES_H_

#include "ui/widgets/sliders/slider_multiple_scalars.h"

namespace vox::ui {
/**
 * Slider widget of multiple type double
 */
template<size_t Size>
class SliderMultipleDoubles : public SliderMultipleScalars<double, Size> {
public:
    /**
     * Constructor
     * @param p_min
     * @param p_max
     * @param p_value
     * @param p_label
     * @param p_format
     */
    explicit SliderMultipleDoubles(double p_min = 0.0,
                                   double p_max = 1.0,
                                   double p_value = 0.5,
                                   const std::string &p_label = "",
                                   const std::string &p_format = "%.6f") :
    SliderMultipleScalars<double, Size>(ImGuiDataType_::ImGuiDataType_Double,
                                        p_min, p_max, p_value, p_label, p_format) {
    }
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_MULTIPLE_DOUBLES_H_ */
