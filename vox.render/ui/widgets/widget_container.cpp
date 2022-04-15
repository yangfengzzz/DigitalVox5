//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "widget_container.h"
#include <algorithm>

namespace vox::ui {
void WidgetContainer::remove_widget(Widget &p_widget) {
    auto found = std::find_if(widgets_.begin(), widgets_.end(), [&p_widget](std::pair<Widget *, MemoryMode> &p_pair) {
        return p_pair.first == &p_widget;
    });
    
    if (found != widgets_.end()) {
        if (found->second == MemoryMode::INTERNAL_MANAGEMENT)
            delete found->first;
        
        widgets_.erase(found);
    }
}

void WidgetContainer::remove_all_widgets() {
    std::for_each(widgets_.begin(), widgets_.end(), [](auto &pair) {
        if (pair.second == MemoryMode::INTERNAL_MANAGEMENT)
            delete pair.first;
    });
    
    widgets_.clear();
}

void WidgetContainer::consider_widget(Widget &p_widget, bool p_manage_memory) {
    widgets_.emplace_back(std::make_pair(&p_widget,
                                         p_manage_memory ? MemoryMode::INTERNAL_MANAGEMENT :
                                         MemoryMode::EXTERNAL_MANAGEMENT));
    p_widget.set_parent(this);
}

void WidgetContainer::unconsider_widget(Widget &p_widget) {
    auto found = std::find_if(widgets_.begin(), widgets_.end(), [&p_widget](std::pair<Widget *, MemoryMode> &p_pair) {
        return p_pair.first == &p_widget;
    });
    
    if (found != widgets_.end()) {
        p_widget.set_parent(nullptr);
        widgets_.erase(found);
    }
}

void WidgetContainer::collect_garbages() {
    widgets_.erase(std::remove_if(widgets_.begin(), widgets_.end(), [](std::pair<Widget *, MemoryMode> &p_item) {
        bool to_destroy = p_item.first && p_item.first->is_destroyed();
        
        if (to_destroy && p_item.second == MemoryMode::INTERNAL_MANAGEMENT)
            delete p_item.first;
        
        return to_destroy;
    }), widgets_.end());
}

void WidgetContainer::draw_widgets() {
    collect_garbages();
    
    if (reversed_draw_order_) {
        for (auto it = widgets_.crbegin(); it != widgets_.crend(); ++it)
            it->first->draw();
    } else {
        for (const auto &widget : widgets_)
            widget.first->draw();
    }
}

void WidgetContainer::reverse_draw_order(const bool reversed) {
    reversed_draw_order_ = reversed;
}

std::vector<std::pair<Widget *, MemoryMode>> &WidgetContainer::widgets() {
    return widgets_;
}

}
