// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "gui/progress_bar.h"

#include <imgui.h>

#include <cmath>

#include "gui/theme.h"
#include "gui/util.h"

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
