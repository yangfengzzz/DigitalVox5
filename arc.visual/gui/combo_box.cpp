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

#include "gui/combo_box.h"

#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <utility>

#include "gui/theme.h"
#include "gui/util.h"

namespace vox::visualization::gui {

namespace {
int g_next_combobox_id = 1;

int CalcItemHeight(const Theme& theme) {
    auto em = ImGui::GetTextLineHeight();
    auto padding = ImGui::GetStyle().FramePadding.y;
    return int(std::ceil(em + 2.0 * padding));
}

}  // namespace
struct Combobox::Impl {
    std::string imgui_id_;
    std::vector<std::string> items_;
    int current_index_ = 0;
    std::function<void(const char*, int)> on_value_changed_;
};

Combobox::Combobox() : impl_(new Combobox::Impl()) {
    impl_->imgui_id_ = "##combobox_" + std::to_string(g_next_combobox_id++);
}

Combobox::Combobox(const std::vector<const char*>& items) : Combobox() {
    for (auto& item : items) {
        AddItem(item);
    }
}

Combobox::~Combobox() = default;

void Combobox::ClearItems() {
    impl_->items_.clear();
    impl_->current_index_ = 0;
}

int Combobox::AddItem(const char* name) {
    impl_->items_.emplace_back(name);
    return int(impl_->items_.size()) - 1;
}

void Combobox::ChangeItem(int index, const char* new_name) { impl_->items_[index] = new_name; }

void Combobox::ChangeItem(const char* orig_name, const char* new_name) {
    for (auto& item : impl_->items_) {
        if (item == orig_name) {
            item = new_name;
            break;
        }
    }
}

void Combobox::RemoveItem(const char* name) {
    for (size_t i = 0; i < impl_->items_.size(); ++i) {
        if (impl_->items_[i] == name) {
            RemoveItem(int(i));
            break;
        }
    }
}

void Combobox::RemoveItem(int index) {
    if (index >= 0 && index < int(impl_->items_.size())) {
        impl_->items_.erase(impl_->items_.begin() + index);
        if (impl_->current_index_ >= int(impl_->items_.size())) {
            impl_->current_index_ = int(impl_->items_.size()) - 1;
        }
    }
}

int Combobox::GetNumberOfItems() const { return static_cast<int>(impl_->items_.size()); }

const char* Combobox::GetItem(int index) const { return impl_->items_[index].c_str(); }

int Combobox::GetSelectedIndex() const { return impl_->current_index_; }

const char* Combobox::GetSelectedValue() const {
    if (impl_->current_index_ >= 0 && impl_->current_index_ < int(impl_->items_.size())) {
        return impl_->items_[impl_->current_index_].c_str();
    } else {
        return "";
    }
}

void Combobox::SetSelectedIndex(int index) {
    if (index >= 0 && index < int(impl_->items_.size())) {
        impl_->current_index_ = index;
    }
}

bool Combobox::SetSelectedValue(const char* value) {
    std::string svalue = value;
    for (size_t i = 0; i < impl_->items_.size(); ++i) {
        if (impl_->items_[i] == svalue) {
            SetSelectedIndex(int(i));
            return true;
        }
    }
    return false;
}

void Combobox::SetOnValueChanged(std::function<void(const char*, int)> on_value_changed) {
    impl_->on_value_changed_ = std::move(on_value_changed);
}

Size Combobox::CalcPreferredSize(const LayoutContext& context, const Constraints& constraints) const {
    auto button_width = ImGui::GetFrameHeight();  // button is square
    auto padding = ImGui::GetStyle().FramePadding;
    int width = 0;
    for (auto& item : impl_->items_) {
        auto size = ImGui::GetFont()->CalcTextSizeA(float(context.theme.font_size), float(constraints.width), 10000.0f,
                                                    item.c_str());
        width = std::max(width, int(std::ceil(size.x)));
    }
    return {width + int(std::round(button_width + 2.0 * padding.x)), CalcItemHeight(context.theme)};
}

Combobox::DrawResult Combobox::Draw(const DrawContext& context) {
    bool value_changed = false;
    bool was_open = ImGui::IsPopupOpen(impl_->imgui_id_.c_str());
    bool did_open = false;

    auto& frame = GetFrame();
    ImGui::SetCursorScreenPos(ImVec2(float(frame.x), float(frame.y) - ImGui::GetScrollY()));

    ImGui::PushStyleColor(ImGuiCol_Button, colorToImgui(context.theme.combobox_arrow_background_color));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorToImgui(context.theme.combobox_arrow_background_color));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorToImgui(context.theme.combobox_arrow_background_color));

    DrawImGuiPushEnabledState();
    ImGui::PushItemWidth(float(frame.width));
    if (ImGui::BeginCombo(impl_->imgui_id_.c_str(), GetSelectedValue())) {
        if (!was_open) {
            did_open = true;
        }
        for (size_t i = 0; i < impl_->items_.size(); ++i) {
            bool isSelected = false;
            if (ImGui::Selectable(impl_->items_[i].c_str(), &isSelected, 0)) {
                impl_->current_index_ = int(i);
                value_changed = true;
                if (impl_->on_value_changed_) {
                    impl_->on_value_changed_(GetSelectedValue(), int(i));
                }
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();  // keyboard focus
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    DrawImGuiPopEnabledState();

    ImGui::PopStyleColor(3);

    return ((value_changed || did_open) ? Widget::DrawResult::REDRAW : Widget::DrawResult::NONE);
}

}  // namespace vox::visualization::gui
