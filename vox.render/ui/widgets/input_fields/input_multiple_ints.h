//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_MULTIPLE_INTS_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_MULTIPLE_INTS_H_

#include "input_multiple_scalars.h"

namespace vox::ui {
/**
 * Input widget of multiple type int
 */
template<size_t Size>
class InputMultipleInts : public InputMultipleScalars<int, Size> {
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
    explicit InputMultipleInts(int p_default_value = 0,
                               int p_step = 1,
                               int p_fast_step = 0,
                               const std::string &p_label = "",
                               const std::string &p_format = "%d",
                               bool p_select_all_on_click = true) :
    InputMultipleScalars<int, Size>(ImGuiDataType_::ImGuiDataType_S32, p_default_value,
                                    p_step, p_fast_step, p_label, p_format, p_select_all_on_click) {
    }
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_INPUT_FIELDS_INPUT_MULTIPLE_INTS_H_ */
