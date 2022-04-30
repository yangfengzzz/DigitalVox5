//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_map>

#include "ui/menu_bar.h"
#include "ui/widgets/panel.h"
#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/canvas.h"

namespace vox {
using namespace ui;

namespace editor::ui {
/**
 * Handle the creation and storage of UI panels
 */
class PanelsManager {
public:
    /**
     * Constructor
     * @param canvas canvas
     */
    explicit PanelsManager(Canvas &canvas);
    
    /**
     * Create a panel
     * @param id id
     * @param args args
     */
    template<typename T, typename... Args>
    void create_panel(const std::string &id, Args &&... args) {
        if constexpr (std::is_base_of<PanelWindow, T>::value) {
            panels_.emplace(id, std::make_unique<T>(id, std::forward<Args>(args)...));
            T &instance = *static_cast<T *>(panels_.at(id).get());
            get_panel_as<MenuBar>("Menu Bar").register_panel(instance.name_, instance);
        } else {
            panels_.emplace(id, std::make_unique<T>(std::forward<Args>(args)...));
        }
        canvas_.add_panel(*panels_.at(id));
    }
    
    /**
     * Returns the panel identified by the given id casted in the given type
     * @param id id
     */
    template<typename T>
    T &get_panel_as(const std::string &id) {
        return *static_cast<T *>(panels_[id].get());
    }
    
private:
    std::unordered_map<std::string, std::unique_ptr<Panel>> panels_;
    Canvas &canvas_;
};

}
}
