//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui/list_view.h"

#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <utility>

#include "gui/theme.h"
#include "gui/util.h"

namespace vox::visualization::gui {

namespace {
const int NO_SELECTION = -1;
int g_next_list_box_id = 1;
const int g_min_visible_items = 3;
}  // namespace

struct ListView::Impl {
    std::string imgui_id_;
    std::vector<std::string> items_;
    int selected_index_ = NO_SELECTION;
    int max_visible_item_ = -1;
    std::function<void(const char *, bool)> on_value_changed_;
};

ListView::ListView() : impl_(new ListView::Impl()) {
    impl_->imgui_id_ = "##listview_" + std::to_string(g_next_list_box_id++);
}

ListView::~ListView() = default;

void ListView::SetItems(const std::vector<std::string> &items) {
    impl_->items_ = items;
    impl_->selected_index_ = NO_SELECTION;
}

int ListView::GetSelectedIndex() const { return impl_->selected_index_; }

const char *ListView::GetSelectedValue() const {
    if (impl_->selected_index_ < 0 || impl_->selected_index_ >= int(impl_->items_.size())) {
        return "";
    } else {
        return impl_->items_[impl_->selected_index_].c_str();
    }
}

void ListView::SetMaxVisibleItems(int num) {
    if (num > 0) {
        impl_->max_visible_item_ = std::max(g_min_visible_items, num);
    } else {
        // unlimited, will make height be DIM_GROW
        impl_->max_visible_item_ = -1;
    }
}
void ListView::SetSelectedIndex(int index) { impl_->selected_index_ = std::min(int(impl_->items_.size() - 1), index); }

void ListView::SetOnValueChanged(std::function<void(const char *, bool)> on_value_changed) {
    impl_->on_value_changed_ = std::move(on_value_changed);
}

Size ListView::CalcPreferredSize(const LayoutContext &context, const Constraints &constraints) const {
    auto padding = ImGui::GetStyle().FramePadding;
    auto fh = ImGui::GetFrameHeight();
    auto *font = ImGui::GetFont();
    ImVec2 size(0, 0);

    for (auto &item : impl_->items_) {
        auto item_size =
                font->CalcTextSizeA(float(context.theme.font_size), float(constraints.width), 0.0, item.c_str());
        size.x = std::max(size.x, item_size.x);
        size.y += fh;
    }
    auto h = Widget::DIM_GROW;
    if (impl_->max_visible_item_ > 0) {
        // make sure show at least g_min_visible_items items, and
        // at most max_visible_item_ items.
        h = std::max((int)impl_->items_.size(), g_min_visible_items);
        h = std::min(h, impl_->max_visible_item_);
        h = int(std::ceil((float)h * fh));
    }
    return Size{int(std::ceil(size.x + 2.0f * padding.x)), h};
}

Size ListView::CalcMinimumSize(const LayoutContext &context) const {
    return {0, g_min_visible_items * context.theme.font_size};
}

Widget::DrawResult ListView::Draw(const DrawContext &context) {
    auto &frame = GetFrame();
    ImGui::SetCursorScreenPos(ImVec2(float(frame.x), float(frame.y) - ImGui::GetScrollY()));
    ImGui::PushItemWidth(float(frame.width));

    ImGui::PushStyleColor(ImGuiCol_FrameBg, colorToImgui(context.theme.list_background_color));
    ImGui::PushStyleColor(ImGuiCol_Header,  // selection color
                          colorToImgui(context.theme.list_selected_color));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,  // hover color
                          colorToImgui(vox::Color(0, 0, 0, 0)));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,  // click-hold color
                          colorToImgui(context.theme.list_selected_color));

    int height_in_items = int(std::floor(frame.height / ImGui::GetFrameHeight()));

    auto result = Widget::DrawResult::NONE;
    auto new_selected_idx = impl_->selected_index_;
    bool is_double_click = false;
    DrawImGuiPushEnabledState();
    if (ImGui::ListBoxHeader(impl_->imgui_id_.c_str(), int(impl_->items_.size()), height_in_items)) {
        for (size_t i = 0; i < impl_->items_.size(); ++i) {
            bool is_selected = (int(i) == impl_->selected_index_);
            // ImGUI's list wants to hover over items, which is not done by
            // any major OS, is pretty unnecessary (you can see the cursor
            // right over the row), and acts really weird. Worse, the hover
            // is drawn instead of the selection color. So to get rid of it
            // we need hover to be the selected color iff this item is
            // selected, otherwise we want it to be transparent.
            if (is_selected) {
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colorToImgui(context.theme.list_selected_color));
            } else {
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colorToImgui(vox::Color(0, 0, 0, 0)));
            }
            if (ImGui::Selectable(impl_->items_[i].c_str(), &is_selected, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (is_selected) {
                    new_selected_idx = int(i);
                }
                // Dear ImGUI seems to have a bug where it registers a
                // double-click as long as you haven't moved the mouse,
                // no matter how long the time between clicks was.
                if (ImGui::IsMouseDoubleClicked(0)) {
                    is_double_click = true;
                }
            }
            ImGui::PopStyleColor();
        }
        ImGui::ListBoxFooter();

        if (new_selected_idx != impl_->selected_index_ || is_double_click) {
            impl_->selected_index_ = new_selected_idx;
            if (impl_->on_value_changed_) {
                impl_->on_value_changed_(GetSelectedValue(), is_double_click);
            }
            result = Widget::DrawResult::REDRAW;
        }
    }
    DrawImGuiPopEnabledState();

    ImGui::PopStyleColor(4);

    ImGui::PopItemWidth();
    return result;
}

}  // namespace vox::visualization::gui
