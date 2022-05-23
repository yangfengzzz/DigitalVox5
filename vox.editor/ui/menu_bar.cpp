//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/menu_bar.h"

#include "vox.editor/editor_actions.h"
#include "vox.editor/editor_settings.h"
#include "vox.editor/editor_utils.h"
#include "vox.editor/entity_creation_menu.h"
#include "vox.render/ui/widgets/drags/drag_float.h"
#include "vox.render/ui/widgets/selection/color_edit.h"
#include "vox.render/ui/widgets/sliders/slider_int.h"
#include "vox.render/ui/widgets/texts/text.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
MenuBar::MenuBar() {
    CreateFileMenu();
    CreateBuildMenu();
    CreateWindowMenu();
    CreateActorsMenu();
    CreateResourcesMenu();
    CreateSettingsMenu();
    CreateLayoutMenu();
    CreateHelpMenu();
}

void MenuBar::HandleShortcuts(float delta_time) {}

void MenuBar::RegisterPanel(const std::string &name, PanelWindow &panel) {
    auto &menu_item = window_menu_->CreateWidget<MenuItem>(name, "", true, true);
    menu_item.value_changed_event_ += std::bind(&PanelWindow::SetOpened, &panel, std::placeholders::_1);

    panels_.emplace(name, std::make_pair(std::ref(panel), std::ref(menu_item)));
}

void MenuBar::CreateFileMenu() {
    auto &file_menu = CreateWidget<MenuList>("File");
    file_menu.CreateWidget<MenuItem>("New Scene", "CTRL + N").clicked_event_ +=
            std::bind(&EditorActions::LoadEmptyScene, EditorActions::GetSingletonPtr());

    file_menu.CreateWidget<MenuItem>("Save Scene", "CTRL + S").clicked_event_ +=
            std::bind(&EditorActions::SaveSceneChanges, EditorActions::GetSingletonPtr());

    file_menu.CreateWidget<MenuItem>("Save Scene As...", "CTRL + SHIFT + S").clicked_event_ +=
            std::bind(&EditorActions::SaveAs, EditorActions::GetSingletonPtr());

    file_menu.CreateWidget<MenuItem>("Exit", "ALT + F4").clicked_event_ += [] {
        // EDITOR_CONTEXT(window)->SetShouldClose(true);
    };
}

void MenuBar::CreateBuildMenu() {
    auto &build_menu = CreateWidget<MenuList>("Build");
    build_menu.CreateWidget<MenuItem>("Build game").clicked_event_ +=
            std::bind(&EditorActions::Build, EditorActions::GetSingletonPtr(), false, false);

    build_menu.CreateWidget<MenuItem>("Build game and run").clicked_event_ +=
            std::bind(&EditorActions::Build, EditorActions::GetSingletonPtr(), true, false);

    build_menu.CreateWidget<Separator>();
    build_menu.CreateWidget<MenuItem>("Temporary build").clicked_event_ +=
            std::bind(&EditorActions::Build, EditorActions::GetSingletonPtr(), true, true);
}

void MenuBar::CreateWindowMenu() {
    window_menu_ = &CreateWidget<MenuList>("Window");
    window_menu_->CreateWidget<MenuItem>("Close all").clicked_event_ +=
            std::bind(&MenuBar::OpenEveryWindows, this, false);
    window_menu_->CreateWidget<MenuItem>("Open all").clicked_event_ +=
            std::bind(&MenuBar::OpenEveryWindows, this, true);
    window_menu_->CreateWidget<Separator>();

    /* When the menu is opened, we update which window is marked as "Opened" or "Closed" */
    window_menu_->clicked_event_ += std::bind(&MenuBar::UpdateToggleableItems, this);
}

void MenuBar::CreateActorsMenu() {
    auto &actors_menu = CreateWidget<MenuList>("Actors");
    EntityCreationMenu::GenerateEntityCreationMenu(actors_menu);
}

void MenuBar::CreateResourcesMenu() {
    auto &resources_menu = CreateWidget<MenuList>("Resources");
    resources_menu.CreateWidget<MenuItem>("Compile shaders").clicked_event_ +=
            std::bind(&EditorActions::CompileShaders, EditorActions::GetSingletonPtr());

    resources_menu.CreateWidget<MenuItem>("Save materials").clicked_event_ +=
            std::bind(&EditorActions::SaveMaterials, EditorActions::GetSingletonPtr());
}

void MenuBar::CreateSettingsMenu() {
    auto &settings_menu = CreateWidget<MenuList>("Settings");
    settings_menu.CreateWidget<MenuItem>("Spawn actors at origin", "", true, true).value_changed_event_ +=
            std::bind(&EditorActions::SetEntitySpawnAtOrigin, EditorActions::GetSingletonPtr(), std::placeholders::_1);

    settings_menu.CreateWidget<MenuItem>("Vertical Synchronization", "", true, true).value_changed_event_ +=
            [this](bool value) {
                // EDITOR_CONTEXT(device)->SetVsync(p_value);
            };

    auto &camera_speed_menu = settings_menu.CreateWidget<MenuList>("Camera Speed");
    {
        camera_speed_menu.CreateWidget<SliderInt>(1, 50, 15, SliderOrientation::HORIZONTAL, "Scene View")
                .value_changed_event_ += std::bind(&EditorActions::SetSceneViewCameraSpeed,
                                                   EditorActions::GetSingletonPtr(), std::placeholders::_1);

        camera_speed_menu.CreateWidget<SliderInt>(1, 50, 15, SliderOrientation::HORIZONTAL, "Asset View")
                .value_changed_event_ += std::bind(&EditorActions::SetAssetViewCameraSpeed,
                                                   EditorActions::GetSingletonPtr(), std::placeholders::_1);
    }

    auto &camera_position_menu = settings_menu.CreateWidget<MenuList>("Reset Camera");
    {
        camera_position_menu.CreateWidget<MenuItem>("Scene View").clicked_event_ +=
                std::bind(&EditorActions::ResetSceneViewCameraPosition, EditorActions::GetSingletonPtr());

        camera_position_menu.CreateWidget<MenuItem>("Asset View").clicked_event_ +=
                std::bind(&EditorActions::ResetAssetViewCameraPosition, EditorActions::GetSingletonPtr());
    }

    auto &view_colors = settings_menu.CreateWidget<MenuList>("View Colors");
    {
        auto &scene_view_background = view_colors.CreateWidget<MenuList>("Scene View Background");
        auto &scene_view_background_picker =
                scene_view_background.CreateWidget<ColorEdit>(false, Color{0.098f, 0.098f, 0.098f});
        scene_view_background_picker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").GetCamera().SetClearColor({ color.r, color.g, color.b });
        };
        scene_view_background.CreateWidget<MenuItem>("Reset").clicked_event_ += [this, &scene_view_background_picker] {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").GetCamera().SetClearColor({ 0.098f, 0.098f, 0.098f });
            // sceneViewBackgroundPicker.color = { 0.098f, 0.098f, 0.098f };
        };

        auto &scene_view_grid = view_colors.CreateWidget<MenuList>("Scene View Grid");
        auto &scene_view_grid_picker = scene_view_grid.CreateWidget<ColorEdit>(false, Color(0.176f, 0.176f, 0.176f));
        scene_view_grid_picker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").SetGridColor({ color.r, color.g, color.b });
        };
        scene_view_grid.CreateWidget<MenuItem>("Reset").clicked_event_ += [this, &scene_view_grid_picker] {
            // EDITOR_PANEL(Panels::SceneView, "Scene View").SetGridColor(OvMaths::FVector3(0.176f, 0.176f, 0.176f));
            // sceneViewGridPicker.color = OvUI::Types::Color(0.176f, 0.176f, 0.176f);
        };

        auto &asset_view_background = view_colors.CreateWidget<MenuList>("Asset View Background");
        auto &asset_view_background_picker =
                asset_view_background.CreateWidget<ColorEdit>(false, Color{0.098f, 0.098f, 0.098f});
        asset_view_background_picker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").GetCamera().SetClearColor({ color.r, color.g, color.b });
        };
        asset_view_background.CreateWidget<MenuItem>("Reset").clicked_event_ += [this, &asset_view_background_picker] {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").GetCamera().SetClearColor({ 0.098f, 0.098f, 0.098f });
            // assetViewBackgroundPicker.color = { 0.098f, 0.098f, 0.098f };
        };

        auto &asset_view_grid = view_colors.CreateWidget<MenuList>("Asset View Grid");
        auto &asset_view_grid_picker = asset_view_grid.CreateWidget<ColorEdit>(false, Color(0.176f, 0.176f, 0.176f));
        asset_view_grid_picker.color_changed_event_ += [this](const auto &color) {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").SetGridColor({ color.r, color.g, color.b });
        };
        asset_view_grid.CreateWidget<MenuItem>("Reset").clicked_event_ += [this, &asset_view_grid_picker] {
            // EDITOR_PANEL(Panels::AssetView, "Asset View").SetGridColor(OvMaths::FVector3(0.176f, 0.176f, 0.176f));
            // assetViewGridPicker.color = OvUI::Types::Color(0.176f, 0.176f, 0.176f);
        };
    }

    auto &scene_view_billboard_scale_menu = settings_menu.CreateWidget<MenuList>("3D Icons Scales");
    {
        auto &light_billboard_scale_slider = scene_view_billboard_scale_menu.CreateWidget<SliderInt>(
                0, 100, static_cast<int>(EditorSettings::light_billboard_scale_ * 100.0f),
                SliderOrientation::HORIZONTAL, "Lights");
        light_billboard_scale_slider.value_changed_event_ +=
                [this](int value) { EditorSettings::light_billboard_scale_ = value / 100.0f; };
        light_billboard_scale_slider.format_ = "%d %%";
    }

    auto &snapping_menu = settings_menu.CreateWidget<MenuList>("Snapping");
    {
        snapping_menu
                .CreateWidget<DragFloat>(0.001f, 999999.0f, EditorSettings::translation_snap_unit_, 0.05f,
                                         "Translation Unit")
                .value_changed_event_ += [this](float value) { EditorSettings::translation_snap_unit_ = value; };
        snapping_menu
                .CreateWidget<DragFloat>(0.001f, 999999.0f, EditorSettings::rotation_snap_unit_, 1.0f, "Rotation Unit")
                .value_changed_event_ += [this](float value) { EditorSettings::rotation_snap_unit_ = value; };
        snapping_menu
                .CreateWidget<DragFloat>(0.001f, 999999.0f, EditorSettings::scaling_snap_unit_, 0.05f, "Scaling Unit")
                .value_changed_event_ += [this](float value) { EditorSettings::scaling_snap_unit_ = value; };
    }

    auto &debugging_menu = settings_menu.CreateWidget<MenuList>("Debugging");
    {
        debugging_menu.CreateWidget<MenuItem>("Show geometry bounds", "", true, EditorSettings::show_geometry_bounds_)
                .value_changed_event_ += [this](bool value) { EditorSettings::show_geometry_bounds_ = value; };
        debugging_menu.CreateWidget<MenuItem>("Show lights bounds", "", true, EditorSettings::show_light_bounds_)
                .value_changed_event_ += [this](bool value) { EditorSettings::show_light_bounds_ = value; };
        auto &sub_menu = debugging_menu.CreateWidget<MenuList>("Frustum culling visualizer...");
        sub_menu.CreateWidget<MenuItem>("For geometry", "", true,
                                        EditorSettings::show_geometry_frustum_culling_in_scene_view_)
                .value_changed_event_ +=
                [this](bool value) { EditorSettings::show_geometry_frustum_culling_in_scene_view_ = value; };
        sub_menu.CreateWidget<MenuItem>("For lights", "", true,
                                        EditorSettings::show_light_frustum_culling_in_scene_view_)
                .value_changed_event_ +=
                [this](bool value) { EditorSettings::show_light_frustum_culling_in_scene_view_ = value; };
    }
}

void MenuBar::CreateLayoutMenu() {
    auto &layout_menu = CreateWidget<MenuList>("Layout");
    layout_menu.CreateWidget<MenuItem>("Reset").clicked_event_ +=
            std::bind(&EditorActions::ResetLayout, EditorActions::GetSingletonPtr());
}

void MenuBar::CreateHelpMenu() {
    auto &help_menu = CreateWidget<MenuList>("Help");
    help_menu.CreateWidget<MenuItem>("GitHub").clicked_event_ += [] { OpenUrl("https://github.com/ArcheGraphics"); };
    help_menu.CreateWidget<MenuItem>("Tutorials").clicked_event_ +=
            [] { OpenUrl("https://arche.graphics/docs/intro"); };
    help_menu.CreateWidget<MenuItem>("Scripting API").clicked_event_ +=
            [] { OpenUrl("https://arche.graphics/docs/intro"); };
    help_menu.CreateWidget<Separator>();
    help_menu.CreateWidget<MenuItem>("Bug Report").clicked_event_ +=
            [] { OpenUrl("https://github.com/ArcheGraphics/Arche-cpp/issues"); };
    help_menu.CreateWidget<MenuItem>("Feature Request").clicked_event_ +=
            [] { OpenUrl("https://github.com/ArcheGraphics/Arche-cpp/issues"); };
    help_menu.CreateWidget<Separator>();
    help_menu.CreateWidget<MenuItem>("Arche.js").clicked_event_ +=
            [] { OpenUrl("https://github.com/ArcheGraphics/Arche.js"); };
    help_menu.CreateWidget<::vox::ui::Text>("Version: 0.0.1");
}

void MenuBar::UpdateToggleableItems() {
    for (auto &[name, panel] : panels_) panel.second.get().checked_ = panel.first.get().IsOpened();
}

void MenuBar::OpenEveryWindows(bool state) {
    for (auto &[name, panel] : panels_) panel.first.get().SetOpened(state);
}

}  // namespace vox::editor::ui
