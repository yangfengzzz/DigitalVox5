//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/texts/text.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/columns.h"
#include "material/material.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class MaterialEditor : public PanelWindow {
public:
    MaterialEditor(const std::string &title,
                   bool opened,
                   const PanelWindowSettings &window_settings);
    
    /**
     * Refresh the material editor
     */
    void refresh();
    
    /**
     * Defines the target material of the material editor
     */
    void set_target(const std::shared_ptr<Material> &new_target);
    
    /**
     * Returns the target of the material editor
     */
    [[nodiscard]] std::shared_ptr<Material> get_target() const;
    
    /**
     * Remove the target of the material editor (Clear the material editor)
     */
    void remove_target();
    
    /**
     * Launch the preview of the currently targeted material
     */
    void preview();
    
    /**
     * Reset material
     */
    void reset();
    
private:
    void on_material_dropped();
    
    void on_shader_dropped();
    
    void create_header_buttons();
    
    void create_material_selector();
    
    void create_shader_selector();
    
    void create_material_settings();
    
    void create_shader_settings();
    
    void generate_shader_settings_content();
    
    void generate_material_settings_content();
    
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

}
}
