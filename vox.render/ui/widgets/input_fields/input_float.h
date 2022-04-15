//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_FLOAT_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_FLOAT_H_

#include "input_single_scalar.h"

namespace vox::ui {
/**
 * Input widget of type float
 */
class InputFloat : public InputSingleScalar<float> {
public:
    /**
     * Constructor
     * @param p_default_value p_defaultValue
     * @param p_step p_step
     * @param p_fast_step p_fastStep
     * @param p_label p_label
     * @param p_format p_format
     * @param p_select_all_on_click p_selectAllOnClick
     */
    explicit InputFloat(float p_default_value = 0.0f,
                        float p_step = 0.1f,
                        float p_fast_step = 0.0f,
                        const std::string &p_label = "",
                        const std::string &p_format = "%.3f",
                        bool p_select_all_on_click = true);
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_FLOAT_H_ */
