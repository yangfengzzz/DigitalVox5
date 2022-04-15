//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_WIDGET_CONTAINER_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_WIDGET_CONTAINER_H_

#include "widget.h"

namespace vox::ui {
/**
 * Defines how the memory should be managed
 */
enum class MemoryMode {
    INTERNAL_MANAGEMENT,
    EXTERNAL_MANAGEMENT
};

/**
 * The base class for any widget container
 */
class WidgetContainer {
public:
    /**
     * Remove a widget from the container
     * @param p_widget p_widget
     */
    void remove_widget(Widget &p_widget);
    
    /**
     * Remove all widgets from the container
     */
    void remove_all_widgets();
    
    /**
     * Consider a widget
     * @param p_manage_memory p_manageMemory
     */
    void consider_widget(Widget &p_widget, bool p_manage_memory = true);
    
    /**
     * Unconsider a widget
     * @param p_widget p_widget
     */
    void unconsider_widget(Widget &p_widget);
    
    /**
     * Collect garbage by removing widgets marked as "Destroyed"
     */
    void collect_garbages();
    
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
     * @param p_args p_args
     */
    template<typename T, typename ... Args>
    T &create_widget(Args &&... p_args) {
        widgets_.emplace_back(new T(p_args...), MemoryMode::INTERNAL_MANAGEMENT);
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

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_WIDGET_CONTAINER_H_ */
