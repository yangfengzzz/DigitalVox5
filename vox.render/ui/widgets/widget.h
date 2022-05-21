//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <imgui.h>

#include <string>

#include "ui/drawable.h"
#include "ui/plugins/data_dispatcher.h"
#include "ui/plugins/pluginable.h"

namespace vox::ui {
class WidgetContainer;

/**
 * A AWidget is the base class for any widget of UI.
 * It is basically a visual element that can be placed into a panel.
 * It is drawable and can receive plugins
 */
class Widget : public Drawable, public Pluginable {
public:
    /**
     * Constructor
     */
    Widget();

    /**
     * Draw the widget on the panel
     */
    void draw() override;

    /**
     * Link the widget to another one by making its id identical to the given widget
     */
    void link_to(const Widget &widget);

    /**
     * Mark the widget as destroyed. It will be removed from the owner panel
     * by the garbage collector
     */
    void destroy();

    /**
     * Returns true if the widget is marked as destroyed
     */
    [[nodiscard]] bool is_destroyed() const;

    /**
     * Defines the parent of this widget
     */
    void set_parent(WidgetContainer *parent);

    /**
     * Returns true if the widget has a parent
     */
    [[nodiscard]] bool has_parent() const;

    /**
     * Returns the parent of the widget or nullptr if there is no parent
     */
    WidgetContainer *parent();

protected:
    virtual void draw_impl() = 0;

public:
    bool enabled_ = true;
    bool line_break_ = true;

protected:
    WidgetContainer *parent_{nullptr};
    std::string widget_id_ = "?";
    bool auto_execute_plugins_ = true;

private:
    static uint64_t widget_id_increment_;
    bool destroyed_ = false;
};

}  // namespace vox::ui
