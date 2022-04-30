//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "event.h"
#include "ini_file.h"

#include "ui/widgets/texts/text.h"
#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/buttons/button_simple.h"
#include "view/asset_view.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class AssetProperties : public PanelWindow {
public:
    AssetProperties(const std::string &p_title, bool p_opened,
                    const PanelWindowSettings &p_window_settings,
                    AssetView &view);
    
    /**
     * Defines the target of the asset settings editor
     * @param p_path p_path
     */
    void set_target(const std::string &p_path);
    
    /**
     * Refresh the panel to show the current target settings
     */
    void refresh();
    
    /**
     * Launch the preview of the target asset
     */
    void preview();
    
private:
    void create_header_buttons();
    
    void create_asset_selector();
    
    void create_settings();
    
    void create_info();
    
    void create_model_settings();
    
    void create_texture_settings();
    
    void apply();
    
private:
    AssetView &asset_view_;
    std::string resource_;
    
    Event<> target_changed_;
    Group *settings_{nullptr};
    Group *info_{nullptr};
    ButtonSimple *apply_button_{nullptr};
    ButtonSimple *revert_button_{nullptr};
    ButtonSimple *preview_button_{nullptr};
    ButtonSimple *reset_button_{nullptr};
    Widget *header_separator_{nullptr};
    Widget *header_line_break_{nullptr};
    Columns<2> *settings_columns_{nullptr};
    Columns<2> *info_columns_{nullptr};
    Text *asset_selector_{nullptr};
    std::unique_ptr<fs::IniFile> metadata_{nullptr};
};

}
}
