//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "dummy.h"

#include "ui/widgets/converter.h"

namespace vox::ui {
Dummy::Dummy(const Vector2F &size) : size_(size) {}

void Dummy::draw_impl() { ImGui::Dummy(Converter::to_imVec2(size_)); }

}  // namespace vox::ui
