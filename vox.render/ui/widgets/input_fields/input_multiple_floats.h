//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_MULTIPLE_FLOATS_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_MULTIPLE_FLOATS_H_

#include "input_multiple_scalars.h"

namespace vox::ui {
/**
 * Input widget of multiple type float
 */
template<size_t Size>
class InputMultipleFloats : public InputMultipleScalars<float, Size> {
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
    explicit InputMultipleFloats(float p_default_value = 0.0f,
                                 float p_step = 0.1f,
                                 float p_fast_step = 0.0f,
                                 const std::string &p_label = "",
                                 const std::string &p_format = "%.3f",
                                 bool p_select_all_on_click = true) :
    InputMultipleScalars<float, Size>(ImGuiDataType_::ImGuiDataType_Float,
                                      p_default_value,
                                      p_step,
                                      p_fast_step,
                                      p_label,
                                      p_format,
                                      p_select_all_on_click) {
    }
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_MULTIPLE_FLOATS_H_ */
