//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/asset_properties.h"

#include "vox.editor/editor_actions.h"
#include "vox.editor/size_converter.h"
#include "vox.render/platform/filesystem.h"
#include "vox.render/ui/gui_drawer.h"
#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/layout/group_collapsable.h"
#include "vox.render/ui/widgets/layout/new_line.h"
#include "vox.render/ui/widgets/selection/combo_box.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
AssetProperties::AssetProperties(const std::string &p_title,
                                 bool p_opened,
                                 const PanelWindowSettings &p_window_settings,
                                 AssetView &view)
    : PanelWindow(p_title, p_opened, p_window_settings), asset_view_(view) {
    target_changed_ += [this]() { SetTarget(asset_selector_->content_); };

    CreateHeaderButtons();

    header_separator_ = &CreateWidget<::vox::ui::Separator>();
    header_separator_->enabled_ = false;

    CreateAssetSelector();

    settings_ = &CreateWidget<GroupCollapsable>("Settings");
    settings_columns_ = &settings_->CreateWidget<Columns<2>>();
    settings_columns_->widths_[0] = 150;

    info_ = &CreateWidget<GroupCollapsable>("Info");
    info_columns_ = &info_->CreateWidget<Columns<2>>();
    info_columns_->widths_[0] = 150;

    settings_->enabled_ = info_->enabled_ = false;
}

void AssetProperties::SetTarget(const std::string &p_path) {
    resource_ = p_path.empty() ? p_path : EditorActions::GetSingleton().GetResourcePath(p_path);

    if (asset_selector_) {
        asset_selector_->content_ = resource_;
    }

    Refresh();
}

void AssetProperties::Refresh() {
    metadata_ = std::make_unique<::vox::fs::IniFile>(EditorActions::GetSingleton().GetRealPath(resource_) + ".meta");

    CreateSettings();
    CreateInfo();

    apply_button_->enabled_ = settings_->enabled_;
    reset_button_->enabled_ = settings_->enabled_;
    revert_button_->enabled_ = settings_->enabled_;

    switch (fs::ExtraFileType(resource_)) {
        case fs::FileType::MODEL:
        case fs::FileType::TEXTURE:
        case fs::FileType::MATERIAL:
            preview_button_->enabled_ = true;
            break;
        default:
            preview_button_->enabled_ = false;
            break;
    }

    // Enables the header separator (And the line break) if at least one button is enabled
    header_separator_->enabled_ =
            apply_button_->enabled_ || reset_button_->enabled_ || revert_button_->enabled_ || preview_button_->enabled_;
    header_line_break_->enabled_ = header_separator_->enabled_;
}

void AssetProperties::Preview() {
    const auto kFileType = fs::ExtraFileType(resource_);

    if (kFileType == fs::FileType::MODEL) {
        // if (auto resource = OVSERVICE(OvCore::ResourceManagement::ModelManager).GetResource(resource)) {
        //     _assetView.setResource(resource);
        // }
    } else if (kFileType == fs::FileType::TEXTURE) {
        // if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(resource)) {
        //     _assetView.setResource(resource);
        // }
    }

    asset_view_.Open();
}

void AssetProperties::CreateHeaderButtons() {
    apply_button_ = &CreateWidget<ButtonSimple>("Apply");
    apply_button_->idle_background_color_ = {0.0f, 0.5f, 0.0f};
    apply_button_->enabled_ = false;
    apply_button_->line_break_ = false;
    apply_button_->clicked_event_ += std::bind(&AssetProperties::Apply, this);

    revert_button_ = &CreateWidget<ButtonSimple>("Revert");
    revert_button_->idle_background_color_ = {0.7f, 0.5f, 0.0f};
    revert_button_->enabled_ = false;
    revert_button_->line_break_ = false;
    revert_button_->clicked_event_ += std::bind(&AssetProperties::SetTarget, this, resource_);

    preview_button_ = &CreateWidget<ButtonSimple>("Preview");
    preview_button_->idle_background_color_ = {0.7f, 0.5f, 0.0f};
    preview_button_->enabled_ = false;
    preview_button_->line_break_ = false;
    preview_button_->clicked_event_ += std::bind(&AssetProperties::Preview, this);

    reset_button_ = &CreateWidget<ButtonSimple>("Reset to default");
    reset_button_->idle_background_color_ = {0.5f, 0.0f, 0.0f};
    reset_button_->enabled_ = false;
    reset_button_->line_break_ = false;
    reset_button_->clicked_event_ += [this] {
        metadata_->RemoveAll();
        CreateSettings();
    };

    header_line_break_ = &CreateWidget<NewLine>();
    header_line_break_->enabled_ = false;
}

void AssetProperties::CreateAssetSelector() {
    auto &columns = CreateWidget<Columns<2>>();
    columns.widths_[0] = 150;
    // assetSelector = &OvCore::Helpers::GUIDrawer::DrawAsset(columns, "Target", resource, &targetChanged);
}

void AssetProperties::CreateSettings() {
    settings_columns_->RemoveAllWidgets();

    const auto kFileType = fs::ExtraFileType(resource_);

    settings_->enabled_ = true;

    if (kFileType == fs::FileType::MODEL) {
        CreateModelSettings();
    } else if (kFileType == fs::FileType::TEXTURE) {
        CreateTextureSettings();
    } else {
        settings_->enabled_ = false;
    }
}

void AssetProperties::CreateInfo() {
    const auto kRealPath = EditorActions::GetSingleton().GetRealPath(resource_);

    info_columns_->RemoveAllWidgets();

    if (std::filesystem::exists(kRealPath)) {
        info_->enabled_ = true;

        GuiDrawer::CreateTitle(*info_columns_, "Path");
        info_columns_->CreateWidget<Text>(kRealPath);

        GuiDrawer::CreateTitle(*info_columns_, "Size");
        const auto [kSize, kUnit] = SizeConverter::ConvertToOptimalUnit(
                static_cast<float>(std::filesystem::file_size(kRealPath)), SizeConverter::SizeUnit::BYTE);
        info_columns_->CreateWidget<Text>(std::to_string(kSize) + " " + SizeConverter::UnitToString(kUnit));

        GuiDrawer::CreateTitle(*info_columns_, "Metadata");
        info_columns_->CreateWidget<Text>(std::filesystem::exists(kRealPath + ".meta") ? "Yes" : "No");
    } else {
        info_->enabled_ = false;
    }
}

#define MODEL_FLAG_ENTRY(setting)                                                         \
    GuiDrawer::DrawBoolean(                                                               \
            *settings_columns_, setting, [&]() { return metadata_->Get<bool>(setting); }, \
            [&](bool value) { metadata_->Set<bool>(setting, value); })

void AssetProperties::CreateModelSettings() {
    metadata_->Add("CALC_TANGENT_SPACE", true);
    metadata_->Add("JOIN_IDENTICAL_VERTICES", true);
    metadata_->Add("MAKE_LEFT_HANDED", false);
    metadata_->Add("TRIANGULATE", true);
    metadata_->Add("REMOVE_COMPONENT", false);
    metadata_->Add("GEN_NORMALS", false);
    metadata_->Add("GEN_SMOOTH_NORMALS", true);
    metadata_->Add("SPLIT_LARGE_MESHES", false);
    metadata_->Add("PRE_TRANSFORVERTICES", true);
    metadata_->Add("LIMIT_BONE_WEIGHTS", false);
    metadata_->Add("VALIDATE_DATA_STRUCTURE", false);
    metadata_->Add("IMPROVE_CACHE_LOCALITY", true);
    metadata_->Add("REMOVE_REDUNDANT_MATERIALS", false);
    metadata_->Add("FIX_INFACING_NORMALS", false);
    metadata_->Add("SORT_BY_PTYPE", false);
    metadata_->Add("FIND_DEGENERATES", false);
    metadata_->Add("FIND_INVALID_DATA", true);
    metadata_->Add("GEN_UV_COORDS", true);
    metadata_->Add("TRANSFORUV_COORDS", false);
    metadata_->Add("FIND_INSTANCES", true);
    metadata_->Add("OPTIMIZE_MESHES", true);
    metadata_->Add("OPTIMIZE_GRAPH", true);
    metadata_->Add("FLIP_UVS", false);
    metadata_->Add("FLIP_WINDING_ORDER", false);
    metadata_->Add("SPLIT_BY_BONE_COUNT", false);
    metadata_->Add("DEBONE", true);
    metadata_->Add("GLOBAL_SCALE", true);
    metadata_->Add("EMBED_TEXTURES", false);
    metadata_->Add("FORCE_GEN_NORMALS", false);
    metadata_->Add("DROP_NORMALS", false);
    metadata_->Add("GEN_BOUNDING_BOXES", false);

    MODEL_FLAG_ENTRY("CALC_TANGENT_SPACE");
    MODEL_FLAG_ENTRY("JOIN_IDENTICAL_VERTICES");
    MODEL_FLAG_ENTRY("MAKE_LEFT_HANDED");
    MODEL_FLAG_ENTRY("TRIANGULATE");
    MODEL_FLAG_ENTRY("REMOVE_COMPONENT");
    MODEL_FLAG_ENTRY("GEN_NORMALS");
    MODEL_FLAG_ENTRY("GEN_SMOOTH_NORMALS");
    MODEL_FLAG_ENTRY("SPLIT_LARGE_MESHES");
    MODEL_FLAG_ENTRY("PRE_TRANSFORVERTICES");
    MODEL_FLAG_ENTRY("LIMIT_BONE_WEIGHTS");
    MODEL_FLAG_ENTRY("VALIDATE_DATA_STRUCTURE");
    MODEL_FLAG_ENTRY("IMPROVE_CACHE_LOCALITY");
    MODEL_FLAG_ENTRY("REMOVE_REDUNDANT_MATERIALS");
    MODEL_FLAG_ENTRY("FIX_INFACING_NORMALS");
    MODEL_FLAG_ENTRY("SORT_BY_PTYPE");
    MODEL_FLAG_ENTRY("FIND_DEGENERATES");
    MODEL_FLAG_ENTRY("FIND_INVALID_DATA");
    MODEL_FLAG_ENTRY("GEN_UV_COORDS");
    MODEL_FLAG_ENTRY("TRANSFORUV_COORDS");
    MODEL_FLAG_ENTRY("FIND_INSTANCES");
    MODEL_FLAG_ENTRY("OPTIMIZE_MESHES");
    MODEL_FLAG_ENTRY("OPTIMIZE_GRAPH");
    MODEL_FLAG_ENTRY("FLIP_UVS");
    MODEL_FLAG_ENTRY("FLIP_WINDING_ORDER");
    MODEL_FLAG_ENTRY("SPLIT_BY_BONE_COUNT");
    MODEL_FLAG_ENTRY("DEBONE");
    MODEL_FLAG_ENTRY("GLOBAL_SCALE");
    MODEL_FLAG_ENTRY("EMBED_TEXTURES");
    MODEL_FLAG_ENTRY("FORCE_GEN_NORMALS");
    MODEL_FLAG_ENTRY("DROP_NORMALS");
    MODEL_FLAG_ENTRY("GEN_BOUNDING_BOXES");
};

void AssetProperties::CreateTextureSettings() {
    // _metadata->add("MIN_FILTER",
    // static_cast<int>(OvRendering::Settings::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR));
    // _metadata->add("MAG_FILTER", static_cast<int>(OvRendering::Settings::ETextureFilteringMode::LINEAR));
    metadata_->Add("ENABLE_MIPMAPPING", true);

    std::map<int, std::string> filtering_modes{{0x2600, "NEAREST"},
                                               {0x2601, "LINEAR"},
                                               {0x2700, "NEAREST_MIPMAP_NEAREST"},
                                               {0x2703, "LINEAR_MIPMAP_LINEAR"},
                                               {0x2701, "LINEAR_MIPMAP_NEAREST"},
                                               {0x2702, "NEAREST_MIPMAP_LINEAR"}};

    GuiDrawer::CreateTitle(*settings_columns_, "MIN_FILTER");
    auto &min_filter = settings_columns_->CreateWidget<ComboBox>(metadata_->Get<int>("MIN_FILTER"));
    min_filter.choices_ = filtering_modes;
    min_filter.value_changed_event_ += [this](int p_choice) { metadata_->Set("MIN_FILTER", p_choice); };

    GuiDrawer::CreateTitle(*settings_columns_, "MAG_FILTER");
    auto &mag_filter = settings_columns_->CreateWidget<ComboBox>(metadata_->Get<int>("MAG_FILTER"));
    mag_filter.choices_ = filtering_modes;
    mag_filter.value_changed_event_ += [this](int p_choice) { metadata_->Set("MAG_FILTER", p_choice); };

    GuiDrawer::DrawBoolean(
            *settings_columns_, "ENABLE_MIPMAPPING", [&]() { return metadata_->Get<bool>("ENABLE_MIPMAPPING"); },
            [&](bool value) { metadata_->Set<bool>("ENABLE_MIPMAPPING", value); });
}

void AssetProperties::Apply() {
    metadata_->Rewrite();

    const auto kResourcePath = EditorActions::GetSingleton().GetResourcePath(resource_);
    const auto kFileType = fs::ExtraFileType(resource_);

    //    if (fileType == fs::FileType::MODEL) {
    //        auto &modelManager = OVSERVICE(OvCore::ResourceManagement::ModelManager);
    //        if (modelManager.IsResourceRegistered(resourcePath)) {
    //            modelManager.AResourceManager::ReloadResource(resourcePath);
    //        }
    //    } else if (fileType == fs::FileType::TEXTURE) {
    //        auto &textureManager = OVSERVICE(OvCore::ResourceManagement::TextureManager);
    //        if (textureManager.IsResourceRegistered(resourcePath)) {
    //            textureManager.AResourceManager::ReloadResource(resourcePath);
    //        }
    //    }

    Refresh();
}

}  // namespace vox::editor::ui
