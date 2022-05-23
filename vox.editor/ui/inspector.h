//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.editor/ui/asset_browser.h"
#include "vox.editor/ui/hierarchy.h"
#include "vox.render/ui/widgets/buttons/button.h"
#include "vox.render/ui/widgets/input_fields/input_text.h"
#include "vox.render/ui/widgets/layout/group_collapsable.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/selection/check_box.h"
#include "vox.render/ui/widgets/selection/combo_box.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class Inspector : public PanelWindow {
public:
    Inspector(const std::string &p_new_name, bool opened, const PanelWindowSettings &window_settings);

    /**
     * Destructor
     */
    ~Inspector() override;

    /**
     * Focus the given entity
     */
    void FocusEntity(Entity *target);

    /**
     * Unfocus the currently targeted entity
     */
    void UnFocus();

    /**
     * Unfocus the currently targeted entity without removing listeners attached to this entity
     */
    void SoftUnFocus();

    /**
     * Returns the currently selected entity
     */
    [[nodiscard]] Entity *TargetEntity() const;

    /**
     * Create the entity inspector for the given entity
     */
    void CreateEntityInspector(Entity *target);

    /**
     * Draw the given component in inspector
     */
    void DrawComponent(Component *component);

    /**
     * Draw the given behaviour in inspector
     */
    void DrawBehaviour(Behaviour *behaviour);

    /**
     * Refresh the inspector
     */
    void Refresh();

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

}  // namespace editor::ui
}  // namespace vox
