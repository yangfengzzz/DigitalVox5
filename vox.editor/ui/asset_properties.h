//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.editor/ini_file.h"
#include "vox.editor/view/asset_view.h"
#include "vox.render/event.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/layout/columns.h"
#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/texts/text.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class AssetProperties : public PanelWindow {
public:
    AssetProperties(const std::string &p_title,
                    bool p_opened,
                    const PanelWindowSettings &p_window_settings,
                    AssetView &view);

    /**
     * Defines the target of the asset settings editor
     * @param p_path p_path
     */
    void SetTarget(const std::string &p_path);

    /**
     * Refresh the panel to show the current target settings
     */
    void Refresh();

    /**
     * Launch the preview of the target asset
     */
    void Preview();

private:
    void CreateHeaderButtons();

    void CreateAssetSelector();

    void CreateSettings();

    void CreateInfo();

    void CreateModelSettings();

    void CreateTextureSettings();

    void Apply();

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

}  // namespace editor::ui
}  // namespace vox
