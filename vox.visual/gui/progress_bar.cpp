//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.visual/gui/progress_bar.h"

#include <imgui.h>

#include <cmath>

#include "vox.visual/gui/theme.h"
#include "vox.visual/gui/util.h"

namespace vox::visualization::gui {

struct ProgressBar::Impl {
    float value_ = 0.0f;
};

ProgressBar::ProgressBar() : impl_(new ProgressBar::Impl()) {}

ProgressBar::~ProgressBar() = default;

/// ProgressBar values ranges from 0.0 (incomplete) to 1.0 (complete)
void ProgressBar::SetValue(float value) { impl_->value_ = value; }

float ProgressBar::GetValue() const { return impl_->value_; }

Size ProgressBar::CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const {
    return {constraints.width, int(std::ceil(0.25 * context.theme.font_size))};
}

Widget::DrawResult ProgressBar::Draw(const DrawContext& context) {
    auto& frame = GetFrame();
    auto fg = context.theme.border_color;
    auto color = colorToImguiRGBA(fg);
    float rounding = frame.height / 2.0f;
    ImGui::GetWindowDrawList()->AddRect(ImVec2(float(frame.x), float(frame.y)),
                                        ImVec2(float(frame.GetRight()), float(frame.GetBottom())), color, rounding);
    float x = float(frame.x) + float(frame.width) * impl_->value_;
    x = std::max(x, float(frame.x + rounding));
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(float(frame.x), float(frame.y)),
                                              ImVec2(float(x), float(frame.GetBottom())), color, frame.height / 2.0f);
    return DrawResult::NONE;
}

}  // namespace vox::visualization::gui
