//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ui/widgets/panels/panel_menu_bar.h"
#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/menu/menu_item.h"
#include "ui/widgets/menu/menu_list.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class MenuBar : public PanelMenuBar {
    using PanelMap = std::unordered_map<std::string, std::pair<std::reference_wrapper<PanelWindow>, std::reference_wrapper<MenuItem>>>;
    
public:
    /**
     * Constructor
     */
    MenuBar();
    
    /**
     * Check inputs for menubar shortcuts
     */
    void handle_shortcuts(float delta_time);
    
    /**
     * Register a panel to the menu bar window menu
     */
    void register_panel(const std::string &name, PanelWindow &panel);
    
private:
    void create_file_menu();
    
    void create_build_menu();
    
    void create_window_menu();
    
    void create_actors_menu();
    
    void create_resources_menu();
    
    void create_settings_menu();
    
    void create_layout_menu();
    
    void create_help_menu();
    
    void update_toggleable_items();
    
    void open_every_windows(bool state);
    
private:
    PanelMap panels_;
    
    MenuList *window_menu_{nullptr};
};

}
}
