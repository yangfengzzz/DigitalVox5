//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "menu_bar.h"

#include "ui/widgets/visual/separator.h"
#include "ui/widgets/sliders/slider_int.h"
#include "ui/widgets/drags/drag_float.h"
#include "ui/widgets/selection/color_edit.h"
#include "ui/widgets/texts/text.h"

#include "editor_utils.h"
#include "editor_actions.h"
#include "entity_creation_menu.h"
#include "editor_settings.h"

namespace vox::editor::ui {
MenuBar::MenuBar() {
    create_file_menu();
    create_build_menu();
    create_window_menu();
    create_actors_menu();
    create_resources_menu();
    create_settings_menu();
    create_layout_menu();
    create_help_menu();
}

void MenuBar::handle_shortcuts(float delta_time) {
    
}

void MenuBar::register_panel(const std::string &name, PanelWindow &panel) {
    auto &menu_item = window_menu_->create_widget<MenuItem>(name, "", true, true);
    menu_item.value_changed_event_ += std::bind(&PanelWindow::set_opened, &panel, std::placeholders::_1);
    
    panels_.emplace(name, std::make_pair(std::ref(panel), std::ref(menu_item)));
}

void MenuBar::create_file_menu() {
    auto &file_menu = create_widget<MenuList>("File");
    file_menu.create_widget<MenuItem>("New Scene", "CTRL + N").clicked_event_ +=
    std::bind(&EditorActions::load_empty_scene, EditorActions::GetSingletonPtr());
    
    file_menu.create_widget<MenuItem>("Save Scene", "CTRL + S").clicked_event_ +=
    std::bind(&EditorActions::save_scene_changes, EditorActions::GetSingletonPtr());
    
    file_menu.create_widget<MenuItem>("Save Scene As...", "CTRL + SHIFT + S").clicked_event_ +=
    std::bind(&EditorActions::save_as, EditorActions::GetSingletonPtr());
    
    file_menu.create_widget<MenuItem>("Exit", "ALT + F4").clicked_event_ += [] {
        // EDITOR_CONTEXT(window)->SetShouldClose(true);
    };
}

void MenuBar::create_build_menu() {
    auto &build_menu = create_widget<MenuList>("Build");
    build_menu.create_widget<MenuItem>("Build game").clicked_event_ +=
    std::bind(&EditorActions::build, EditorActions::GetSingletonPtr(), false, false);
    
    build_menu.create_widget<MenuItem>("Build game and run").clicked_event_ +=
    std::bind(&EditorActions::build, EditorActions::GetSingletonPtr(), true, false);
    
    build_menu.create_widget<Separator>();
    build_menu.create_widget<MenuItem>("Temporary build").clicked_event_ +=
    std::bind(&EditorActions::build, EditorActions::GetSingletonPtr(), true, true);
}

void MenuBar::create_window_menu() {
    window_menu_ = &create_widget<MenuList>("Window");
    window_menu_->create_widget<MenuItem>("Close all").clicked_event_ += std::bind(&MenuBar::open_every_windows, this, false);
    window_menu_->create_widget<MenuItem>("Open all").clicked_event_ += std::bind(&MenuBar::open_every_windows, this, true);
    window_menu_->create_widget<Separator>();
    
    /* When the menu is opened, we update which window is marked as "Opened" or "Closed" */
    window_menu_->clicked_event_ += std::bind(&MenuBar::update_toggleable_items, this);
}

void MenuBar::create_actors_menu() {
    auto &actors_menu = create_widget<MenuList>("Actors");
    EntityCreationMenu::generate_entity_creation_menu(actors_menu);
}

void MenuBar::create_resources_menu() {
    auto &resources_menu = create_widget<MenuList>("Resources");
    resources_menu.create_widget<MenuItem>("Compile shaders").clicked_event_ +=
    std::bind(&EditorActions::compile_shaders, EditorActions::GetSingletonPtr());
    
    resources_menu.create_widget<MenuItem>("Save materials").clicked_event_ +=
    std::bind(&EditorActions::save_materials, EditorActions::GetSingletonPtr());
}

void MenuBar::create_settings_menu() {
    auto &settings_menu = create_widget<MenuList>("Settings");
    settings_menu.create_widget<MenuItem>("Spawn actors at origin", "", true, true).value_changed_event_ +=
    std::bind(&EditorActions::set_entity_spawn_at_origin, EditorActions::GetSingletonPtr(), std::placeholders::_1);
    
    settings_menu.create_widget<MenuItem>("Vertical Synchronization", "", true, true).value_changed_event_ += [this](bool value) {
        // EDITOR_CONTEXT(device)->SetVsync(p_value);
    };
    
    auto &camera_speed_menu = settings_menu.create_widget<MenuList>("Camera Speed");
    {
        camera_speed_menu.create_widget<SliderInt>(1, 50, 15, SliderOrientation::HORIZONTAL, "Scene View").value_changed_event_ +=
        std::bind(&EditorActions::set_scene_view_camera_speed, EditorActions::GetSingletonPtr(), std::placeholders::_1);
        
        camera_speed_menu.create_widget<SliderInt>(1, 50, 15, SliderOrientation::HORIZONTAL, "Asset View").value_changed_event_ +=
        std::bind(&EditorActions::set_asset_view_camera_speed, EditorActions::GetSingletonPtr(), std::placeholders::_1);
    }
    
    auto &camera_position_menu = settings_menu.create_widget<MenuList>("Reset Camera");
    {
        camera_position_menu.create_widget<MenuItem>("Scene View").clicked_event_ +=
        std::bind(&EditorActions::reset_scene_view_camera_position, EditorActions::GetSingletonPtr());
        
        camera_position_menu.create_widget<MenuItem>("Asset View").clicked_event_ +=
        std::bind(&EditorActions::reset_asset_view_camera_position, EditorActions::GetSingletonPtr());
    }
    
    auto &view_colors = settings_menu.create_widget<MenuList>("View Colors");
    {
        auto &scene_view_background = view_colors.create_widget<MenuList>("Scene View Background");
        auto &scene_view_background_picker = scene_view_background.create_widget<ColorEdit>(false, Color{0.098f, 0.098f, 0.098f});
        scene_view_background_picker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").GetCamera().SetClearColor({ color.r, color.g, color.b });
        };
        scene_view_background.create_widget<MenuItem>("Reset").clicked_event_ += [this, &scene_view_background_picker] {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").GetCamera().SetClearColor({ 0.098f, 0.098f, 0.098f });
            // sceneViewBackgroundPicker.color = { 0.098f, 0.098f, 0.098f };
        };
        
        auto &scene_view_grid = view_colors.create_widget<MenuList>("Scene View Grid");
        auto &scene_view_grid_picker = scene_view_grid.create_widget<ColorEdit>(false, Color(0.176f, 0.176f, 0.176f));
        scene_view_grid_picker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").SetGridColor({ color.r, color.g, color.b });
        };
        scene_view_grid.create_widget<MenuItem>("Reset").clicked_event_ += [this, &scene_view_grid_picker] {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").SetGridColor(OvMaths::FVector3(0.176f, 0.176f, 0.176f));
            // sceneViewGridPicker.color = OvUI::Types::Color(0.176f, 0.176f, 0.176f);
        };
        
        auto &asset_view_background = view_colors.create_widget<MenuList>("Asset View Background");
        auto &asset_view_background_picker = asset_view_background.create_widget<ColorEdit>(false, Color{0.098f, 0.098f, 0.098f});
        asset_view_background_picker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").GetCamera().SetClearColor({ color.r, color.g, color.b });
        };
        asset_view_background.create_widget<MenuItem>("Reset").clicked_event_ += [this, &asset_view_background_picker] {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").GetCamera().SetClearColor({ 0.098f, 0.098f, 0.098f });
            // assetViewBackgroundPicker.color = { 0.098f, 0.098f, 0.098f };
        };
        
        auto &asset_view_grid = view_colors.create_widget<MenuList>("Asset View Grid");
        auto &asset_view_grid_picker = asset_view_grid.create_widget<ColorEdit>(false, Color(0.176f, 0.176f, 0.176f));
        asset_view_grid_picker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").SetGridColor({ color.r, color.g, color.b });
        };
        asset_view_grid.create_widget<MenuItem>("Reset").clicked_event_ += [this, &asset_view_grid_picker] {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").SetGridColor(OvMaths::FVector3(0.176f, 0.176f, 0.176f));
            // assetViewGridPicker.color = OvUI::Types::Color(0.176f, 0.176f, 0.176f);
        };
    }
    
    auto &scene_view_billboard_scale_menu = settings_menu.create_widget<MenuList>("3D Icons Scales");
    {
        auto &light_billboard_scale_slider =
        scene_view_billboard_scale_menu
            .create_widget<SliderInt>(0, 100, static_cast<int>(EditorSettings::light_billboard_scale_ * 100.0f),
                                      SliderOrientation::HORIZONTAL, "Lights");
        light_billboard_scale_slider.value_changed_event_ += [this](int value) {
            EditorSettings::light_billboard_scale_ = value / 100.0f;
        };
        light_billboard_scale_slider.format_ = "%d %%";
    }
    
    auto &snapping_menu = settings_menu.create_widget<MenuList>("Snapping");
    {
        snapping_menu.create_widget<DragFloat>(0.001f, 999999.0f, EditorSettings::translation_snap_unit_, 0.05f,
                                               "Translation Unit").value_changed_event_ += [this](float value) {
            EditorSettings::translation_snap_unit_ = value;
        };
        snapping_menu.create_widget<DragFloat>(0.001f, 999999.0f, EditorSettings::rotation_snap_unit_, 1.0f,
                                               "Rotation Unit").value_changed_event_ += [this](float value) {
            EditorSettings::rotation_snap_unit_ = value;
        };
        snapping_menu.create_widget<DragFloat>(0.001f, 999999.0f, EditorSettings::scaling_snap_unit_, 0.05f,
                                               "Scaling Unit").value_changed_event_ += [this](float value) {
            EditorSettings::scaling_snap_unit_ = value;
        };
    }
    
    auto &debugging_menu = settings_menu.create_widget<MenuList>("Debugging");
    {
        debugging_menu.create_widget<MenuItem>("Show geometry bounds", "", true,
                                               EditorSettings::show_geometry_bounds_).value_changed_event_ += [this](bool value) {
            EditorSettings::show_geometry_bounds_ = value;
        };
        debugging_menu.create_widget<MenuItem>("Show lights bounds", "", true,
                                               EditorSettings::show_light_bounds_).value_changed_event_ += [this](bool value) {
            EditorSettings::show_light_bounds_ = value;
        };
        auto &sub_menu = debugging_menu.create_widget<MenuList>("Frustum culling visualizer...");
        sub_menu.create_widget<MenuItem>("For geometry", "", true,
                                         EditorSettings::show_geometry_frustum_culling_in_scene_view_).value_changed_event_ +=
        [this](bool value) {
            EditorSettings::show_geometry_frustum_culling_in_scene_view_ = value;
        };
        sub_menu.create_widget<MenuItem>("For lights", "", true,
                                         EditorSettings::show_light_frustum_culling_in_scene_view_).value_changed_event_ +=
        [this](bool value) {
            EditorSettings::show_light_frustum_culling_in_scene_view_ = value;
        };
    }
}

void MenuBar::create_layout_menu() {
    auto &layout_menu = create_widget<MenuList>("Layout");
    layout_menu.create_widget<MenuItem>("Reset").clicked_event_ +=
    std::bind(&EditorActions::reset_layout, EditorActions::GetSingletonPtr());
}

void MenuBar::create_help_menu() {
    auto &help_menu = create_widget<MenuList>("Help");
    help_menu.create_widget<MenuItem>("GitHub").clicked_event_ += [] {
        open_url("https://github.com/ArcheGraphics");
    };
    help_menu.create_widget<MenuItem>("Tutorials").clicked_event_ += [] {
        open_url("https://arche.graphics/docs/intro");
    };
    help_menu.create_widget<MenuItem>("Scripting API").clicked_event_ += [] {
        open_url("https://arche.graphics/docs/intro");
    };
    help_menu.create_widget<Separator>();
    help_menu.create_widget<MenuItem>("Bug Report").clicked_event_ += [] {
        open_url("https://github.com/ArcheGraphics/Arche-cpp/issues");
    };
    help_menu.create_widget<MenuItem>("Feature Request").clicked_event_ += [] {
        open_url("https://github.com/ArcheGraphics/Arche-cpp/issues");
    };
    help_menu.create_widget<Separator>();
    help_menu.create_widget<MenuItem>("Arche.js").clicked_event_ += [] {
        open_url("https://github.com/ArcheGraphics/Arche.js");
    };
    help_menu.create_widget<::vox::ui::Text>("Version: 0.0.1");
}

void MenuBar::update_toggleable_items() {
    for (auto &[name, panel] : panels_)
        panel.second.get().checked_ = panel.first.get().is_opened();
}

void MenuBar::open_every_windows(bool state) {
    for (auto &[name, panel] : panels_)
        panel.first.get().set_opened(state);
}

}
