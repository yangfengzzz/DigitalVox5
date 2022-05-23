//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/menu/menu_item.h"
#include "vox.render/ui/widgets/menu/menu_list.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/panels/panel_menu_bar.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class MenuBar : public PanelMenuBar {
    using PanelMap =
            std::unordered_map<std::string,
                               std::pair<std::reference_wrapper<PanelWindow>, std::reference_wrapper<MenuItem>>>;

public:
    /**
     * Constructor
     */
    MenuBar();

    /**
     * Check inputs for menubar shortcuts
     */
    void HandleShortcuts(float delta_time);

    /**
     * Register a panel to the menu bar window menu
     */
    void RegisterPanel(const std::string &name, PanelWindow &panel);

private:
    void CreateFileMenu();

    void CreateBuildMenu();

    void CreateWindowMenu();

    void CreateActorsMenu();

    void CreateResourcesMenu();

    void CreateSettingsMenu();

    void CreateLayoutMenu();

    void CreateHelpMenu();

    void UpdateToggleableItems();

    void OpenEveryWindows(bool state);

private:
    PanelMap panels_;

    MenuList *window_menu_{nullptr};
};

}  // namespace editor::ui
}  // namespace vox
