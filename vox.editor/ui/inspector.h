//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/layout/group_collapsable.h"
#include "ui/widgets/input_fields/input_text.h"
#include "ui/widgets/visual/separator.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/buttons/button.h"
#include "ui/widgets/selection/combo_box.h"

#include "hierarchy.h"
#include "asset_browser.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class Inspector : public PanelWindow {
public:
    Inspector(const std::string &p_new_name,
              bool opened,
              const PanelWindowSettings &window_settings);
    
    /**
     * Destructor
     */
    ~Inspector() override;
    
    /**
     * Focus the given entity
     */
    void focus_entity(Entity *target);
    
    /**
     * Unfocus the currently targeted entity
     */
    void un_focus();
    
    /**
     * Unfocus the currently targeted entity without removing listeners attached to this entity
     */
    void soft_un_focus();
    
    /**
     * Returns the currently selected entity
     */
    [[nodiscard]] Entity *target_entity() const;
    
    /**
     * Create the entity inspector for the given entity
     */
    void create_entity_inspector(Entity *target);
    
    /**
     * Draw the given component in inspector
     */
    void draw_component(Component *component);
    
    /**
     * Draw the given behaviour in inspector
     */
    void draw_behaviour(Behaviour *behaviour);
    
    /**
     * Refresh the inspector
     */
    void refresh();
    
private:
    Entity *target_entity_{nullptr};
    Group *entity_info_;
    Group *inspector_header_;
    ComboBox *component_selector_widget_{};
    InputText *script_selector_widget_{};
    
    uint64_t component_added_listener_ = 0;
    uint64_t component_removed_listener_ = 0;
    uint64_t behaviour_added_listener_ = 0;
    uint64_t behaviour_removed_listener_ = 0;
    uint64_t destroyed_listener_ = 0;
};

}
}
