//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "widget.h"

namespace vox::ui {
/**
 * Defines how the memory should be managed
 */
enum class MemoryMode { INTERNAL_MANAGEMENT, EXTERNAL_MANAGEMENT };

/**
 * The base class for any widget container
 */
class WidgetContainer {
public:
    /**
     * Remove a widget from the container
     */
    void remove_widget(Widget &widget);

    /**
     * Remove all widgets from the container
     */
    void remove_all_widgets();

    /**
     * Consider a widget
     */
    void consider_widget(Widget &widget, bool manage_memory = true);

    /**
     * Unconsider a widget
     */
    void unconsider_widget(Widget &widget);

    /**
     * Collect garbage by removing widgets marked as "Destroyed"
     */
    void collect_garbage();

    /**
     * Draw every widgets
     */
    void draw_widgets();

    /**
     * Allow the user to reverse the draw order of this widget container
     */
    void reverse_draw_order(bool reversed = true);

    /**
     * Create a widget
     */
    template <typename T, typename... Args>
    T &create_widget(Args &&...args) {
        widgets_.emplace_back(new T(args...), MemoryMode::INTERNAL_MANAGEMENT);
        T &instance = *reinterpret_cast<T *>(widgets_.back().first);
        instance.set_parent(this);
        return instance;
    }

    /**
     * Returns the widgets and their memory management mode
     */
    std::vector<std::pair<Widget *, MemoryMode>> &widgets();

protected:
    std::vector<std::pair<Widget *, MemoryMode>> widgets_{};
    bool reversed_draw_order_ = false;
};

}  // namespace vox::ui
