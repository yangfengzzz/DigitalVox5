//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "input_int.h"

namespace vox::ui {
InputInt::InputInt(int default_value, int step, int fast_step,
                   const std::string &label, const std::string &format, bool select_all_on_click)
: InputSingleScalar<int>(ImGuiDataType_::ImGuiDataType_S32, default_value,
                         step, fast_step, label, format, select_all_on_click) {
}

}
