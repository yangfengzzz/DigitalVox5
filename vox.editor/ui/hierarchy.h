//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/scene_forward.h"
#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/layout/tree_node.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class Hierarchy : public PanelWindow {
public:
    Hierarchy(const std::string &title, bool opened, const PanelWindowSettings &window_settings);

    /**
     * Clear hierarchy nodes
     */
    void Clear();

    /**
     * Unselect every widgets
     */
    void UnselectEntitiesWidgets();

    /**
     * Select the widget corresponding to the given entity
     */
    void SelectEntityByInstance(Entity *entity);

    /**
     * Select the widget
     */
    void SelectEntityByWidget(TreeNode &widget);

    /**
     * Attach the given entity linked widget to its parent widget
     */
    void AttachEntityToParent(Entity *entity);

    /**
     * Detach the given entity linked widget from its parent widget
     */
    void DetachFromParent(Entity *entity);

    /**
     * Delete the widget referencing the given entity
     */
    void DeleteEntityByInstance(Entity *entity);

    /**
     * Add a widget referencing the given entity
     */
    void AddEntityByInstance(Entity *entity);

public:
    Event<Entity *&> entity_selected_event_;
    Event<Entity *&> entity_unselected_event_;

private:
    TreeNode *scene_root_;

    std::unordered_map<Entity *, TreeNode *> widget_entity_link_;
};

}  // namespace editor::ui
}  // namespace vox
