//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "input_double.h"

namespace vox::ui {
InputDouble::InputDouble(double p_default_value, double p_step, double p_fast_step,
                         const std::string &p_label, const std::string &p_format,
                         bool p_select_all_on_click) :
InputSingleScalar<double>(ImGuiDataType_::ImGuiDataType_Double,
                          p_default_value, p_step, p_fast_step,
                          p_label, p_format, p_select_all_on_click) {
}

}
