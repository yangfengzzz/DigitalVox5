//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <filesystem>
#include <queue>
#include <unordered_map>

#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/layout/tree_node.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"

namespace vox::editor::ui {
/**
 * A panel that handle asset management
 */
class AssetBrowser : public ::vox::ui::PanelWindow {
public:
    AssetBrowser(const std::string &title,
                 bool opened,
                 const ::vox::ui::PanelWindowSettings &window_settings,
                 const std::string &engine_asset_folder = "",
                 const std::string &project_asset_folder = "",
                 const std::string &project_script_folder = "");

    /**
     * Fill the asset browser panels with widgets corresponding to elements in the asset folder
     */
    void Fill();

    /**
     * Clear the asset browser widgets
     */
    void Clear();

    /**
     * Refresh the asset browser widgets (Clear + Fill)
     */
    void Refresh();

private:
    void ParseFolder(::vox::ui::TreeNode &root,
                     const std::filesystem::directory_entry &directory,
                     bool is_engine_item,
                     bool script_folder = false);

    void ConsiderItem(::vox::ui::TreeNode *root,
                      const std::filesystem::directory_entry &entry,
                      bool is_engine_item,
                      bool auto_open = false,
                      bool script_folder = false);

public:
    static const std::string filenames_chars_;

private:
    std::string engine_asset_folder_;
    std::string project_asset_folder_;
    std::string project_script_folder_;
    ::vox::ui::Group *asset_list_;
    std::unordered_map<::vox::ui::TreeNode *, std::string> path_update_;
};

}  // namespace vox::editor::ui
