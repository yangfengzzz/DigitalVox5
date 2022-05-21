//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "widget_container.h"

#include <algorithm>

namespace vox::ui {
void WidgetContainer::remove_widget(Widget &widget) {
    auto found = std::find_if(widgets_.begin(), widgets_.end(),
                              [&widget](std::pair<Widget *, MemoryMode> &pair) { return pair.first == &widget; });

    if (found != widgets_.end()) {
        if (found->second == MemoryMode::INTERNAL_MANAGEMENT) delete found->first;

        widgets_.erase(found);
    }
}

void WidgetContainer::remove_all_widgets() {
    std::for_each(widgets_.begin(), widgets_.end(), [](auto &pair) {
        if (pair.second == MemoryMode::INTERNAL_MANAGEMENT) delete pair.first;
    });

    widgets_.clear();
}

void WidgetContainer::consider_widget(Widget &widget, bool manage_memory) {
    widgets_.emplace_back(
            std::make_pair(&widget, manage_memory ? MemoryMode::INTERNAL_MANAGEMENT : MemoryMode::EXTERNAL_MANAGEMENT));
    widget.set_parent(this);
}

void WidgetContainer::unconsider_widget(Widget &widget) {
    auto found = std::find_if(widgets_.begin(), widgets_.end(),
                              [&widget](std::pair<Widget *, MemoryMode> &pair) { return pair.first == &widget; });

    if (found != widgets_.end()) {
        widget.set_parent(nullptr);
        widgets_.erase(found);
    }
}

void WidgetContainer::collect_garbage() {
    widgets_.erase(std::remove_if(widgets_.begin(), widgets_.end(),
                                  [](std::pair<Widget *, MemoryMode> &item) {
                                      bool to_destroy = item.first && item.first->is_destroyed();

                                      if (to_destroy && item.second == MemoryMode::INTERNAL_MANAGEMENT)
                                          delete item.first;

                                      return to_destroy;
                                  }),
                   widgets_.end());
}

void WidgetContainer::draw_widgets() {
    collect_garbage();

    if (reversed_draw_order_) {
        for (auto it = widgets_.crbegin(); it != widgets_.crend(); ++it) it->first->draw();
    } else {
        for (const auto &widget : widgets_) widget.first->draw();
    }
}

void WidgetContainer::reverse_draw_order(const bool reversed) { reversed_draw_order_ = reversed; }

std::vector<std::pair<Widget *, MemoryMode>> &WidgetContainer::widgets() { return widgets_; }

}  // namespace vox::ui
