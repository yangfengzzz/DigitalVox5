//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui/vector_edit.h"

#include <imgui.h>

#include <utility>

#include "gui/theme.h"
#include "gui/util.h"

namespace vox::visualization::gui {

namespace {
int g_next_vector_edit_id = 1;
}

struct VectorEdit::Impl {
    std::string id_;
    vox::Vector3F value_;
    bool is_unit_vector_ = false;
    std::function<void(const vox::Vector3F&)> on_changed_;
};

VectorEdit::VectorEdit() : impl_(new VectorEdit::Impl()) {
    impl_->id_ = "##vectoredit_" + std::to_string(g_next_vector_edit_id++);
}

VectorEdit::~VectorEdit() = default;

vox::Vector3F VectorEdit::GetValue() const { return impl_->value_; }

void VectorEdit::SetValue(const vox::Vector3F& val) {
    if (impl_->is_unit_vector_) {
        impl_->value_ = val.normalized();
    } else {
        impl_->value_ = val;
    }
}

void VectorEdit::SetOnValueChanged(std::function<void(const vox::Vector3F&)> on_changed) {
    impl_->on_changed_ = std::move(on_changed);
}

Size VectorEdit::CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const {
    auto em = std::ceil(ImGui::GetTextLineHeight());
    auto padding = ImGui::GetStyle().FramePadding;
    return {Widget::DIM_GROW, int(std::ceil(em + 2.0f * padding.y))};
}

Widget::DrawResult VectorEdit::Draw(const DrawContext& context) {
    auto& frame = GetFrame();
    ImGui::SetCursorScreenPos(ImVec2(float(frame.x), float(frame.y) - ImGui::GetScrollY()));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,
                        0.0);  // macOS doesn't round text editing

    ImGui::PushStyleColor(ImGuiCol_FrameBg, colorToImgui(context.theme.text_edit_background_color));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colorToImgui(context.theme.text_edit_background_color));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, colorToImgui(context.theme.text_edit_background_color));

    auto result = Widget::DrawResult::NONE;
    DrawImGuiPushEnabledState();
    ImGui::PushItemWidth(float(GetFrame().width));
    if (ImGui::InputFloat3(impl_->id_.c_str(), &impl_->value_.x)) {
        result = Widget::DrawResult::REDRAW;
    }
    ImGui::PopItemWidth();
    DrawImGuiPopEnabledState();
    DrawImGuiTooltip();

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (impl_->on_changed_) {
            impl_->on_changed_(impl_->value_);
        }
        result = Widget::DrawResult::REDRAW;
    }

    return result;
}

}  // namespace vox::visualization::gui
