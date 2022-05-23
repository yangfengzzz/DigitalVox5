//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/project_settings.h"

#include "vox.render/ui/gui_drawer.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/layout/columns.h"
#include "vox.render/ui/widgets/layout/group_collapsable.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
ProjectSettings::ProjectSettings(const std::string &title,
                                 bool opened,
                                 const PanelWindowSettings &window_settings,
                                 const std::string &project_path,
                                 const std::string &project_name)
    : PanelWindow(title, opened, window_settings), project_settings_(project_path + project_name + ".project") {
    auto &save_button = CreateWidget<ButtonSimple>("Apply");
    save_button.idle_background_color_ = {0.0f, 0.5f, 0.0f};
    save_button.clicked_event_ += [this] {
        ApplyProjectSettings();
        project_settings_.Rewrite();
    };

    save_button.line_break_ = false;

    auto &reset_button = CreateWidget<ButtonSimple>("Reset");
    reset_button.idle_background_color_ = {0.5f, 0.0f, 0.0f};
    reset_button.clicked_event_ += [this] { ResetProjectSettings(); };

    CreateWidget<Separator>();

    {
        /* Physics settings */
        auto &root = CreateWidget<GroupCollapsable>("Physics");
        auto &columns = root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawScalar<float>(columns, "Gravity", GenerateGatherer<float>("gravity"),
                                     GenerateProvider<float>("gravity"), 0.1f, GuiDrawer::min_float_,
                                     GuiDrawer::max_float_);
    }

    {
        /* Build settings */
        auto &generation_root = CreateWidget<GroupCollapsable>("Build");
        auto &columns = generation_root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawBoolean(columns, "Development build", GenerateGatherer<bool>("dev_build"),
                               GenerateProvider<bool>("dev_build"));
    }

    {
        /* Windowing settings */
        auto &windowing_root = CreateWidget<GroupCollapsable>("Windowing");
        auto &columns = windowing_root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawScalar<int>(columns, "Resolution X", GenerateGatherer<int>("x_resolution"),
                                   GenerateProvider<int>("x_resolution"), 1, 0, 10000);
        GuiDrawer::DrawScalar<int>(columns, "Resolution Y", GenerateGatherer<int>("y_resolution"),
                                   GenerateProvider<int>("y_resolution"), 1, 0, 10000);
        GuiDrawer::DrawBoolean(columns, "Fullscreen", GenerateGatherer<bool>("fullscreen"),
                               GenerateProvider<bool>("fullscreen"));
        GuiDrawer::DrawString(columns, "Executable name", GenerateGatherer<std::string>("executable_name"),
                              GenerateProvider<std::string>("executable_name"));
    }

    {
        /* Rendering settings */
        auto &rendering_root = CreateWidget<GroupCollapsable>("Rendering");
        auto &columns = rendering_root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawBoolean(columns, "Vertical Sync.", GenerateGatherer<bool>("vsync"),
                               GenerateProvider<bool>("vsync"));
        GuiDrawer::DrawBoolean(columns, "Multi-sampling", GenerateGatherer<bool>("multisampling"),
                               GenerateProvider<bool>("multisampling"));
        GuiDrawer::DrawScalar<int>(columns, "Samples", GenerateGatherer<int>("samples"),
                                   GenerateProvider<int>("samples"), 1, 2, 16);
        GuiDrawer::DrawScalar<int>(columns, "OpenGL Major", GenerateGatherer<int>("opengl_major"),
                                   GenerateProvider<int>("opengl_major"), 1, 3, 4);
        GuiDrawer::DrawScalar<int>(columns, "OpenGL Minor", GenerateGatherer<int>("opengl_minor"),
                                   GenerateProvider<int>("opengl_minor"), 1, 0, 6);
    }

    {
        /* Scene Management settings */
        auto &game_root = CreateWidget<GroupCollapsable>("Scene Management");
        auto &columns = game_root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawDdstring(columns, "Start scene", GenerateGatherer<std::string>("start_scene"),
                                GenerateProvider<std::string>("start_scene"), "File");
    }
}

void ProjectSettings::ResetProjectSettings() {
    project_settings_.RemoveAll();
    project_settings_.Add<float>("gravity", -9.81f);
    project_settings_.Add<int>("x_resolution", 1280);
    project_settings_.Add<int>("y_resolution", 720);
    project_settings_.Add<bool>("fullscreen", false);
    project_settings_.Add<std::string>("executable_name", "Game");
    project_settings_.Add<std::string>("start_scene", "Scene.scene");
    project_settings_.Add<bool>("vsync", true);
    project_settings_.Add<bool>("multisampling", false);
    project_settings_.Add<int>("samples", 1);
    project_settings_.Add<int>("vulkan_major", 0);
    project_settings_.Add<int>("vulkan_minor", 1);
    project_settings_.Add<bool>("dev_build", true);
}

bool ProjectSettings::IsProjectSettingsIntegrityVerified() {
    return project_settings_.IsKeyExisting("gravity") && project_settings_.IsKeyExisting("x_resolution") &&
           project_settings_.IsKeyExisting("y_resolution") && project_settings_.IsKeyExisting("fullscreen") &&
           project_settings_.IsKeyExisting("executable_name") && project_settings_.IsKeyExisting("start_scene") &&
           project_settings_.IsKeyExisting("vsync") && project_settings_.IsKeyExisting("multisampling") &&
           project_settings_.IsKeyExisting("samples") && project_settings_.IsKeyExisting("vulkan_major") &&
           project_settings_.IsKeyExisting("vulkan_minor") && project_settings_.IsKeyExisting("dev_build");
}

fs::IniFile &ProjectSettings::ProjectSettingFiles() { return project_settings_; }

void ProjectSettings::ApplyProjectSettings() {
    // todo
}

}  // namespace vox::editor::ui
