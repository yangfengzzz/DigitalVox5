//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_INT_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_INT_H_

#include "input_single_scalar.h"

namespace vox::ui {
/**
 * Input widget of type int
 */
class InputInt : public InputSingleScalar<int> {
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
    explicit InputInt(int p_default_value = 0,
                      int p_step = 1,
                      int p_fast_step = 0,
                      const std::string &p_label = "",
                      const std::string &p_format = "%d",
                      bool p_select_all_on_click = true);
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_INT_H_ */
