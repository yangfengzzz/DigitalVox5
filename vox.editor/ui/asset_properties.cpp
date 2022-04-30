//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_properties.h"
#include "editor_actions.h"
#include "filesystem.h"

#include "ui/widgets/visual/separator.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/group_collapsable.h"
#include "ui/widgets/layout/new_line.h"
#include "ui/widgets/selection/combo_box.h"
#include "ui/gui_drawer.h"

#include "size_converter.h"

namespace vox {
namespace editor {
namespace ui {
AssetProperties::AssetProperties(const std::string &p_title, bool p_opened,
                                 const PanelWindowSettings &p_window_settings,
                                 AssetView &view) :
PanelWindow(p_title, p_opened, p_window_settings),
asset_view_(view) {
    target_changed_ += [this]() {
        set_target(asset_selector_->content_);
    };
    
    create_header_buttons();
    
    header_separator_ = &create_widget<::vox::ui::Separator>();
    header_separator_->enabled_ = false;
    
    create_asset_selector();
    
    settings_ = &create_widget<GroupCollapsable>("Settings");
    settings_columns_ = &settings_->create_widget<Columns<2>>();
    settings_columns_->widths_[0] = 150;
    
    info_ = &create_widget<GroupCollapsable>("Info");
    info_columns_ = &info_->create_widget<Columns<2>>();
    info_columns_->widths_[0] = 150;
    
    settings_->enabled_ = info_->enabled_ = false;
}

void AssetProperties::set_target(const std::string &p_path) {
    resource_ = p_path == "" ? p_path : EditorActions::get_singleton().get_resource_path(p_path);
    
    if (asset_selector_) {
        asset_selector_->content_ = resource_;
    }
    
    refresh();
}

void AssetProperties::refresh() {
    metadata_ = std::make_unique<::vox::fs::IniFile>(EditorActions::get_singleton().get_real_path(resource_) + ".meta");
    
    create_settings();
    create_info();
    
    apply_button_->enabled_ = settings_->enabled_;
    reset_button_->enabled_ = settings_->enabled_;
    revert_button_->enabled_ = settings_->enabled_;
    
    switch (fs::extra_file_type(resource_)) {
        case fs::FileType::MODEL:
        case fs::FileType::TEXTURE:
        case fs::FileType::MATERIAL:preview_button_->enabled_ = true;
            break;
        default:preview_button_->enabled_ = false;
            break;
    }
    
    // Enables the header separator (And the line break) if at least one button is enabled
    header_separator_->enabled_ =
    apply_button_->enabled_ || reset_button_->enabled_ || revert_button_->enabled_ || preview_button_->enabled_;
    header_line_break_->enabled_ = header_separator_->enabled_;
}

void AssetProperties::preview() {
    const auto kFileType = fs::extra_file_type(resource_);
    
    if (kFileType == fs::FileType::MODEL) {
        // if (auto resource = OVSERVICE(OvCore::ResourceManagement::ModelManager).GetResource(resource)) {
        //     _assetView.setResource(resource);
        // }
    } else if (kFileType == fs::FileType::TEXTURE) {
        // if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(resource)) {
        //     _assetView.setResource(resource);
        // }
    }
    
    asset_view_.open();
}

void AssetProperties::create_header_buttons() {
    apply_button_ = &create_widget<ButtonSimple>("Apply");
    apply_button_->idle_background_color_ = {0.0f, 0.5f, 0.0f};
    apply_button_->enabled_ = false;
    apply_button_->line_break_ = false;
    apply_button_->clicked_event_ += std::bind(&AssetProperties::apply, this);
    
    revert_button_ = &create_widget<ButtonSimple>("Revert");
    revert_button_->idle_background_color_ = {0.7f, 0.5f, 0.0f};
    revert_button_->enabled_ = false;
    revert_button_->line_break_ = false;
    revert_button_->clicked_event_ += std::bind(&AssetProperties::set_target, this, resource_);
    
    preview_button_ = &create_widget<ButtonSimple>("Preview");
    preview_button_->idle_background_color_ = {0.7f, 0.5f, 0.0f};
    preview_button_->enabled_ = false;
    preview_button_->line_break_ = false;
    preview_button_->clicked_event_ += std::bind(&AssetProperties::preview, this);
    
    reset_button_ = &create_widget<ButtonSimple>("Reset to default");
    reset_button_->idle_background_color_ = {0.5f, 0.0f, 0.0f};
    reset_button_->enabled_ = false;
    reset_button_->line_break_ = false;
    reset_button_->clicked_event_ += [this] {
        metadata_->remove_all();
        create_settings();
    };
    
    header_line_break_ = &create_widget<NewLine>();
    header_line_break_->enabled_ = false;
}

void AssetProperties::create_asset_selector() {
    auto &columns = create_widget<Columns<2>>();
    columns.widths_[0] = 150;
    // assetSelector = &OvCore::Helpers::GUIDrawer::DrawAsset(columns, "Target", resource, &targetChanged);
}

void AssetProperties::create_settings() {
    settings_columns_->remove_all_widgets();
    
    const auto kFileType = fs::extra_file_type(resource_);
    
    settings_->enabled_ = true;
    
    if (kFileType == fs::FileType::MODEL) {
        create_model_settings();
    } else if (kFileType == fs::FileType::TEXTURE) {
        create_texture_settings();
    } else {
        settings_->enabled_ = false;
    }
}

void AssetProperties::create_info() {
    const auto kRealPath = EditorActions::get_singleton().get_real_path(resource_);
    
    info_columns_->remove_all_widgets();
    
    if (std::filesystem::exists(kRealPath)) {
        info_->enabled_ = true;
        
        GuiDrawer::create_title(*info_columns_, "Path");
        info_columns_->create_widget<Text>(kRealPath);
        
        GuiDrawer::create_title(*info_columns_, "Size");
        const auto [kSize, kUnit] = SizeConverter::convert_to_optimal_unit(static_cast<float>(std::filesystem::file_size(kRealPath)),
                                                                           SizeConverter::SizeUnit::BYTE);
        info_columns_->create_widget<Text>(std::to_string(kSize) + " " + SizeConverter::unit_to_string(kUnit));
        
        GuiDrawer::create_title(*info_columns_, "Metadata");
        info_columns_->create_widget<Text>(std::filesystem::exists(kRealPath + ".meta") ? "Yes" : "No");
    } else {
        info_->enabled_ = false;
    }
}

#define MODEL_FLAG_ENTRY(setting) GuiDrawer::draw_boolean(*settings_columns_, setting, [&]() { \
return metadata_->get<bool>(setting); }, [&](bool value) { metadata_->set<bool>(setting, value); })

void AssetProperties::create_model_settings() {
    metadata_->add("CALC_TANGENT_SPACE", true);
    metadata_->add("JOIN_IDENTICAL_VERTICES", true);
    metadata_->add("MAKE_LEFT_HANDED", false);
    metadata_->add("TRIANGULATE", true);
    metadata_->add("REMOVE_COMPONENT", false);
    metadata_->add("GEN_NORMALS", false);
    metadata_->add("GEN_SMOOTH_NORMALS", true);
    metadata_->add("SPLIT_LARGE_MESHES", false);
    metadata_->add("PRE_TRANSFORVERTICES", true);
    metadata_->add("LIMIT_BONE_WEIGHTS", false);
    metadata_->add("VALIDATE_DATA_STRUCTURE", false);
    metadata_->add("IMPROVE_CACHE_LOCALITY", true);
    metadata_->add("REMOVE_REDUNDANT_MATERIALS", false);
    metadata_->add("FIX_INFACING_NORMALS", false);
    metadata_->add("SORT_BY_PTYPE", false);
    metadata_->add("FIND_DEGENERATES", false);
    metadata_->add("FIND_INVALID_DATA", true);
    metadata_->add("GEN_UV_COORDS", true);
    metadata_->add("TRANSFORUV_COORDS", false);
    metadata_->add("FIND_INSTANCES", true);
    metadata_->add("OPTIMIZE_MESHES", true);
    metadata_->add("OPTIMIZE_GRAPH", true);
    metadata_->add("FLIP_UVS", false);
    metadata_->add("FLIP_WINDING_ORDER", false);
    metadata_->add("SPLIT_BY_BONE_COUNT", false);
    metadata_->add("DEBONE", true);
    metadata_->add("GLOBAL_SCALE", true);
    metadata_->add("EMBED_TEXTURES", false);
    metadata_->add("FORCE_GEN_NORMALS", false);
    metadata_->add("DROP_NORMALS", false);
    metadata_->add("GEN_BOUNDING_BOXES", false);
    
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

void AssetProperties::create_texture_settings() {
    // _metadata->add("MIN_FILTER", static_cast<int>(OvRendering::Settings::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR));
    // _metadata->add("MAG_FILTER", static_cast<int>(OvRendering::Settings::ETextureFilteringMode::LINEAR));
    metadata_->add("ENABLE_MIPMAPPING", true);
    
    std::map<int, std::string> filtering_modes
    {
        {0x2600, "NEAREST"},
        {0x2601, "LINEAR"},
        {0x2700, "NEAREST_MIPMAP_NEAREST"},
        {0x2703, "LINEAR_MIPMAP_LINEAR"},
        {0x2701, "LINEAR_MIPMAP_NEAREST"},
        {0x2702, "NEAREST_MIPMAP_LINEAR"}
    };
    
    GuiDrawer::create_title(*settings_columns_, "MIN_FILTER");
    auto &min_filter = settings_columns_->create_widget<ComboBox>(metadata_->get<int>("MIN_FILTER"));
    min_filter.choices_ = filtering_modes;
    min_filter.value_changed_event_ += [this](int p_choice) {
        metadata_->set("MIN_FILTER", p_choice);
    };
    
    GuiDrawer::create_title(*settings_columns_, "MAG_FILTER");
    auto &mag_filter = settings_columns_->create_widget<ComboBox>(metadata_->get<int>("MAG_FILTER"));
    mag_filter.choices_ = filtering_modes;
    mag_filter.value_changed_event_ += [this](int p_choice) {
        metadata_->set("MAG_FILTER", p_choice);
    };
    
    GuiDrawer::draw_boolean(*settings_columns_, "ENABLE_MIPMAPPING", [&]() {
        return metadata_->get<bool>("ENABLE_MIPMAPPING");
    }, [&](bool value) {
        metadata_->set<bool>("ENABLE_MIPMAPPING", value);
    });
}

void AssetProperties::apply() {
    metadata_->rewrite();
    
    const auto kResourcePath = EditorActions::get_singleton().get_resource_path(resource_);
    const auto kFileType = fs::extra_file_type(resource_);
    
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
    
    refresh();
}

}
}
}
