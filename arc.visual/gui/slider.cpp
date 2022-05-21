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

#include "gui/slider.h"

#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <utility>

#include "gui/theme.h"

namespace vox::visualization::gui {

namespace {
int g_next_slider_id = 1;
}

struct Slider::Impl {
    Slider::Type type_;
    std::string id_;
    // A double has 53-bits of integer value, which should be enough for
    // anything we want a slider for. A slider isn't really useful for
    // a range of 2^53 anyway.
    double value_ = 0.0;
    double min_value_ = -1e35;
    double max_value_ = 1e35;
    std::function<void(double)> on_value_changed_;
};

Slider::Slider(Type type) : impl_(new Slider::Impl()) {
    impl_->id_ = "##slider_" + std::to_string(g_next_slider_id++);
    impl_->type_ = type;
}

Slider::~Slider() = default;

int Slider::GetIntValue() const { return int(impl_->value_); }

double Slider::GetDoubleValue() const { return impl_->value_; }

void Slider::SetValue(double val) {
    impl_->value_ = std::max(impl_->min_value_, std::min(impl_->max_value_, val));
    if (impl_->type_ == INT) {
        impl_->value_ = std::round(impl_->value_);
    }
}

double Slider::GetMinimumValue() const { return impl_->min_value_; }

double Slider::GetMaximumValue() const { return impl_->max_value_; }

void Slider::SetLimits(double min_value, double max_value) {
    impl_->min_value_ = min_value;
    impl_->max_value_ = max_value;
    if (impl_->type_ == INT) {
        impl_->min_value_ = std::round(impl_->min_value_);
        impl_->max_value_ = std::round(impl_->max_value_);
    }
    SetValue(impl_->value_);  // make sure value is within new limits
}

void Slider::SetOnValueChanged(std::function<void(double)> on_value_changed) {
    impl_->on_value_changed_ = std::move(on_value_changed);
}

Size Slider::CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const {
    auto line_height = ImGui::GetTextLineHeight();
    auto height = line_height + 2.0 * ImGui::GetStyle().FramePadding.y;

    return {Widget::DIM_GROW, int(std::ceil(height))};
}

Widget::DrawResult Slider::Draw(const DrawContext& context) {
    auto& frame = GetFrame();
    ImGui::SetCursorScreenPos(ImVec2(float(frame.x), float(frame.y) - ImGui::GetScrollY()));

    auto new_value = impl_->value_;
    DrawImGuiPushEnabledState();
    ImGui::PushItemWidth(float(GetFrame().width));
    if (impl_->type_ == INT) {
        int i_new_value = int(new_value);
        ImGui::SliderInt(impl_->id_.c_str(), &i_new_value, int(impl_->min_value_), int(impl_->max_value_));
        new_value = double(i_new_value);
    } else {
        auto f_new_value = float(new_value);
        ImGui::SliderFloat(impl_->id_.c_str(), &f_new_value, float(impl_->min_value_), float(impl_->max_value_));
        new_value = double(f_new_value);
    }
    ImGui::PopItemWidth();
    DrawImGuiPopEnabledState();
    DrawImGuiTooltip();

    if (impl_->value_ != new_value) {
        impl_->value_ = new_value;
        if (impl_->on_value_changed_) {
            impl_->on_value_changed_(new_value);
        }
        return Widget::DrawResult::REDRAW;
    }
    return Widget::DrawResult::NONE;
}

}  // namespace vox::visualization::gui
