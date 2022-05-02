//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>

#include "event.h"

#include "ui/drawable.h"
#include "ui/widgets/panel.h"

namespace vox::ui {
/**
 * A Canvas represents the whole frame available for UI drawing
 */
class Canvas : public Drawable {
public:
    /**
     * Adds a panel to the canvas
     */
    void add_panel(Panel &panel);
    
    /**
     * Removes a panel from the canvas
     */
    void remove_panel(Panel &panel);
    
    /**
     * Removes every panels from the canvas
     */
    void remove_all_panels();
    
    /**
     * Makes the canvas a dockspace (Dockable panels will be able to attach themselves to the canvas)
     */
    void make_dock_space(bool state);
    
    /**
     * Returns true if the canvas is a dockspace
     */
    [[nodiscard]] bool is_dock_space() const;
    
    /**
     * Draw the canvas (Draw every panels)
     */
    void draw() override;
    
private:
    std::vector<std::reference_wrapper<Panel>> panels_;
    bool is_dockspace_ = false;
};

}
