//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "widget.h"

namespace vox::ui {
uint64_t Widget::widget_id_increment_ = 0;

Widget::Widget() { widget_id_ = "##" + std::to_string(widget_id_increment_++); }

void Widget::link_to(const Widget &widget) { widget_id_ = widget.widget_id_; }

void Widget::destroy() { destroyed_ = true; }

bool Widget::is_destroyed() const { return destroyed_; }

void Widget::set_parent(WidgetContainer *parent) { parent_ = parent; }

bool Widget::has_parent() const { return parent_ != nullptr; }

WidgetContainer *Widget::parent() { return parent_; }

void Widget::draw() {
    if (enabled_) {
        draw_impl();

        if (auto_execute_plugins_) execute_plugins();

        if (!line_break_) ImGui::SameLine();
    }
}

}  // namespace vox::ui
