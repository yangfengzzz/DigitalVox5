//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_MULTIPLE_INTS_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_MULTIPLE_INTS_H_

#include "ui/widgets/sliders/slider_multiple_scalars.h"

namespace vox::ui {
/**
 * Slider widget of multiple type int
 */
template<size_t Size>
class SliderMultipleInts : public SliderMultipleScalars<int, Size> {
public:
    /**
     * Constructor
     * @param p_min
     * @param p_max
     * @param p_value
     * @param p_label
     * @param p_format
     */
    explicit SliderMultipleInts(int p_min = 0,
                                int p_max = 100,
                                int p_value = 50,
                                const std::string &p_label = "",
                                const std::string &p_format = "%d") :
    SliderMultipleScalars<int, Size>(ImGuiDataType_::ImGuiDataType_S32,
                                     p_min, p_max, p_value, p_label, p_format) {
    }
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_SLIDERS_SLIDER_MULTIPLE_INTS_H_ */
