//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "scene_forward.h"
#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/tree_node.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class Hierarchy : public PanelWindow {
public:
    Hierarchy(const std::string &title,
              bool opened,
              const PanelWindowSettings &window_settings);
    
    /**
     * Clear hierarchy nodes
     */
    void clear();
    
    /**
     * Unselect every widgets
     */
    void unselect_entities_widgets();
    
    /**
     * Select the widget corresponding to the given entity
     */
    void select_entity_by_instance(Entity *entity);
    
    /**
     * Select the widget
     */
    void select_entity_by_widget(TreeNode &widget);
    
    /**
     * Attach the given entity linked widget to its parent widget
     */
    void attach_entity_to_parent(Entity *entity);
    
    /**
     * Detach the given entity linked widget from its parent widget
     */
    void detach_from_parent(Entity *entity);
    
    /**
     * Delete the widget referencing the given entity
     */
    void delete_entity_by_instance(Entity *entity);
    
    /**
     * Add a widget referencing the given entity
     */
    void add_entity_by_instance(Entity *entity);
    
public:
    Event<Entity *&> entity_selected_event_;
    Event<Entity *&> entity_unselected_event_;
    
private:
    TreeNode *scene_root_;
    
    std::unordered_map<Entity *, TreeNode *> widget_entity_link_;
};

}
}
