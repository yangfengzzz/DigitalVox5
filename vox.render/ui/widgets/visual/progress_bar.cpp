//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "progress_bar.h"

#include <utility>
#include "ui/widgets/converter.h"

namespace vox::ui {
ProgressBar::ProgressBar(float p_fraction, const Vector2F &p_size, std::string p_overlay) :
fraction_(p_fraction), size_(p_size), overlay_(std::move(p_overlay)) {
}

void ProgressBar::draw_impl() {
    ImGui::ProgressBar(fraction_, Converter::to_imVec2(size_), !overlay_.empty() ? overlay_.c_str() : nullptr);
}

}
