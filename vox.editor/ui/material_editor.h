//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/material.h"
#include "vox.render/ui/widgets/layout/columns.h"
#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/texts/text.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class MaterialEditor : public PanelWindow {
public:
    MaterialEditor(const std::string &title, bool opened, const PanelWindowSettings &window_settings);

    /**
     * Refresh the material editor
     */
    void Refresh();

    /**
     * Defines the target material of the material editor
     */
    void SetTarget(const std::shared_ptr<Material> &new_target);

    /**
     * Returns the target of the material editor
     */
    [[nodiscard]] std::shared_ptr<Material> GetTarget() const;

    /**
     * Remove the target of the material editor (Clear the material editor)
     */
    void RemoveTarget();

    /**
     * Launch the preview of the currently targeted material
     */
    void Preview();

    /**
     * Reset material
     */
    void Reset();

private:
    void OnMaterialDropped();

    void OnShaderDropped();

    void CreateHeaderButtons();

    void CreateMaterialSelector();

    void CreateShaderSelector();

    void CreateMaterialSettings();

    void CreateShaderSettings();

    void GenerateShaderSettingsContent();

    void GenerateMaterialSettingsContent();

private:
    std::shared_ptr<Material> target_{nullptr};
    ShaderSource *shader_{nullptr};

    Text *target_material_text_{nullptr};
    Text *shader_text_{nullptr};

    Event<> material_dropped_event_;
    Event<> shader_dropped_event_;

    Group *settings_{nullptr};
    Group *material_settings_{nullptr};
    Group *shader_settings_{nullptr};

    Columns<2> *shader_settings_columns_{nullptr};
    Columns<2> *material_settings_columns_{nullptr};
};

}  // namespace editor::ui
}  // namespace vox
