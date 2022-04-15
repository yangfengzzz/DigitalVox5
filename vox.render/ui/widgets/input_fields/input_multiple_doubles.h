//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_MULTIPLE_DOUBLES_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_MULTIPLE_DOUBLES_H_

#include "input_multiple_scalars.h"

namespace vox::ui {
/**
 * Input widget of multiple type double
 */
template<size_t Size>
class InputMultipleDoubles : public InputMultipleScalars<double, Size> {
public:
    /**
     * Constructor
     * @param p_default_value
     * @param p_step
     * @param p_fast_step
     * @param p_label
     * @param p_format
     * @param p_select_all_on_click
     */
    explicit InputMultipleDoubles(double p_default_value = 0.0,
                                  double p_step = 0.1,
                                  double p_fast_step = 0.0,
                                  const std::string &p_label = "",
                                  const std::string &p_format = "%.6f",
                                  bool p_select_all_on_click = true) :
    InputMultipleScalars<double, Size>(ImGuiDataType_::ImGuiDataType_Double,
                                       p_default_value,
                                       p_step,
                                       p_fast_step,
                                       p_label,
                                       p_format,
                                       p_select_all_on_click) {
    }
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_MULTIPLE_DOUBLES_H_ */
