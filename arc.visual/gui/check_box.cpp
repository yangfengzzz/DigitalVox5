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

#include "gui/check_box.h"

#include <imgui.h>

#include <cmath>
#include <string>
#include <utility>

#include "gui/theme.h"
#include "gui/util.h"

namespace vox::visualization::gui {

namespace {
int g_next_checkbox_id = 1;
}

struct Checkbox::Impl {
    std::string name_;
    std::string id_;
    bool is_checked_ = false;
    std::function<void(bool)> on_checked_;
};

Checkbox::Checkbox(const char* name) : impl_(new Checkbox::Impl()) {
    impl_->name_ = name;
    impl_->id_ = impl_->name_ + "##checkbox_" + std::to_string(g_next_checkbox_id++);
}

Checkbox::~Checkbox() = default;

bool Checkbox::IsChecked() const { return impl_->is_checked_; }

void Checkbox::SetChecked(bool checked) { impl_->is_checked_ = checked; }

void Checkbox::SetOnChecked(std::function<void(bool)> on_checked) { impl_->on_checked_ = std::move(on_checked); }

Size Checkbox::CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const {
    auto em = ImGui::GetTextLineHeight();
    auto padding = ImGui::GetStyle().FramePadding;
    auto text_size =
            ImGui::GetFont()->CalcTextSizeA(float(context.theme.font_size), 10000, 10000, impl_->name_.c_str());
    int height = int(std::ceil(em + 2.0f * padding.y));
    auto checkbox_width = height + padding.x;
    return {int(checkbox_width + std::ceil(text_size.x + 2.0f * padding.x)), height};
}

Widget::DrawResult Checkbox::Draw(const DrawContext& context) {
    auto& frame = GetFrame();
    ImGui::SetCursorScreenPos(ImVec2(float(frame.x), float(frame.y) - ImGui::GetScrollY()));
    auto result = Widget::DrawResult::NONE;

    // ImGUI doesn't offer styling specific to checkboxes other than the
    // color of the checkmark, so we need to adjust the colors ourselves.
    if (impl_->is_checked_) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, colorToImgui(context.theme.checkbox_background_on_color));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colorToImgui(context.theme.checkbox_background_hover_on_color));
    } else {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, colorToImgui(context.theme.checkbox_background_off_color));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colorToImgui(context.theme.checkbox_background_hover_off_color));
    }

    DrawImGuiPushEnabledState();
    ImGui::PushItemWidth(float(GetFrame().width));
    if (ImGui::Checkbox(impl_->id_.c_str(), &impl_->is_checked_)) {
        if (impl_->on_checked_) {
            impl_->on_checked_(impl_->is_checked_);
        }
        result = Widget::DrawResult::REDRAW;
    }
    ImGui::PopItemWidth();
    DrawImGuiPopEnabledState();
    DrawImGuiTooltip();

    ImGui::PopStyleColor(2);

    return result;
}

}  // namespace vox::visualization::gui
