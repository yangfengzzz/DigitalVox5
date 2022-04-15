//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "input_float.h"

namespace vox::ui {
InputFloat::InputFloat(float p_default_value, float p_step, float p_fast_step,
                       const std::string &p_label, const std::string &p_format,
                       bool p_select_all_on_click)
: InputSingleScalar<float>(ImGuiDataType_::ImGuiDataType_Float,
                           p_default_value, p_step, p_fast_step,
                           p_label, p_format, p_select_all_on_click) {
}

}
