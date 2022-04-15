//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "new_line.h"

namespace vox::ui {
void NewLine::draw_impl() {
    ImGui::NewLine();
}

}
