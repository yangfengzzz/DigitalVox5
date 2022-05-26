//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.visual/gui/text_edit.h"

#include <imgui.h>

#include <cmath>
#include <string>
#include <utility>

#include "vox.visual/gui/theme.h"
#include "vox.visual/gui/util.h"

namespace vox::visualization::gui {

namespace {
int g_next_text_edit_id = 1;

// See 3rdparty/imgui/misc/imgui_stdlib.cpp
int InputTextCallback(ImGuiInputTextCallbackData *data) {
    if (data && data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        auto *s = reinterpret_cast<std::string *>(data->UserData);
        s->resize(data->BufTextLen);
        data->Buf = const_cast<char *>(s->c_str());
    }
    return 0;
}

}  // namespace

struct TextEdit::Impl {
    std::string id_;
    std::string text_;
    std::string placeholder_;
    std::function<void(const char *)> on_text_changed_;
    std::function<void(const char *)> on_value_changed_;
};

TextEdit::TextEdit() : impl_(new TextEdit::Impl()) {
    impl_->id_ = "##textedit_" + std::to_string(g_next_text_edit_id++);
    impl_->text_.reserve(1);
}

TextEdit::~TextEdit() = default;

const char *TextEdit::GetText() const { return impl_->text_.c_str(); }

void TextEdit::SetText(const char *text) { impl_->text_ = text; }

const char *TextEdit::GetPlaceholderText() const { return impl_->placeholder_.c_str(); }

void TextEdit::SetPlaceholderText(const char *text) { impl_->placeholder_ = text; }

void TextEdit::SetOnTextChanged(std::function<void(const char *)> on_text_changed) {
    impl_->on_text_changed_ = std::move(on_text_changed);
}

void TextEdit::SetOnValueChanged(std::function<void(const char *)> on_value_changed) {
    impl_->on_value_changed_ = std::move(on_value_changed);
}

bool TextEdit::ValidateNewText(const char *text) { return true; }

Size TextEdit::CalcPreferredSize(const LayoutContext &context, const Constraints &constraints) const {
    auto em = std::ceil(ImGui::GetTextLineHeight());
    auto padding = ImGui::GetStyle().FramePadding;
    return {constraints.width, int(std::ceil(em + 2.0f * padding.y))};
}

Widget::DrawResult TextEdit::Draw(const DrawContext &context) {
    auto &frame = GetFrame();
    ImGui::SetCursorScreenPos(ImVec2(float(frame.x), float(frame.y) + ImGui::GetScrollY()));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,
                        0.0);  // macOS doesn't round text editing

    ImGui::PushStyleColor(ImGuiCol_FrameBg, colorToImgui(context.theme.text_edit_background_color));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colorToImgui(context.theme.text_edit_background_color));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, colorToImgui(context.theme.text_edit_background_color));

    int text_flags = ImGuiInputTextFlags_CallbackResize;
    if (!IsEnabled()) {
        text_flags = ImGuiInputTextFlags_ReadOnly;
    }
    auto result = Widget::DrawResult::NONE;
    DrawImGuiPushEnabledState();
    ImGui::PushItemWidth(float(GetFrame().width));
    if (ImGui::InputTextWithHint(impl_->id_.c_str(), impl_->placeholder_.c_str(),
                                 const_cast<char *>(impl_->text_.c_str()), impl_->text_.capacity() + 1, text_flags,
                                 InputTextCallback, &impl_->text_)) {
        if (impl_->on_text_changed_) {
            impl_->on_text_changed_(impl_->text_.c_str());
        }
        result = Widget::DrawResult::REDRAW;
    }
    ImGui::PopItemWidth();
    DrawImGuiPopEnabledState();
    DrawImGuiTooltip();

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (ValidateNewText(impl_->text_.c_str())) {
            if (impl_->on_value_changed_) {
                impl_->on_value_changed_(impl_->text_.c_str());
            }
        }
        // ValidateNewText() may have updated text (even if returned true)
        result = Widget::DrawResult::REDRAW;
    }

    return result;
}

}  // namespace vox::visualization::gui
