//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "project_settings.h"

#include "ui/gui_drawer.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/layout/group_collapsable.h"
#include "ui/widgets/visual/separator.h"
#include "ui/widgets/buttons/button_simple.h"

namespace vox::editor::ui {
ProjectSettings::ProjectSettings(const std::string &title, bool opened,
                                 const PanelWindowSettings &window_settings,
                                 const std::string &project_path, const std::string &project_name) :
PanelWindow(title, opened, window_settings),
project_settings_(project_path + project_name + ".project") {
    auto &save_button = CreateWidget<ButtonSimple>("Apply");
    save_button.idle_background_color_ = {0.0f, 0.5f, 0.0f};
    save_button.clicked_event_ += [this] {
        apply_project_settings();
        project_settings_.rewrite();
    };
    
    save_button.line_break_ = false;
    
    auto &reset_button = CreateWidget<ButtonSimple>("Reset");
    reset_button.idle_background_color_ = {0.5f, 0.0f, 0.0f};
    reset_button.clicked_event_ += [this] {
        reset_project_settings();
    };

    CreateWidget<Separator>();
    
    {
        /* Physics settings */
        auto &root = CreateWidget<GroupCollapsable>("Physics");
        auto &columns = root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawScalar<float>(columns, "Gravity", generate_gatherer<float>("gravity"),
                                     generate_provider<float>("gravity"), 0.1f, GuiDrawer::min_float_,
                                     GuiDrawer::max_float_);
    }
    
    {
        /* Build settings */
        auto &generation_root = CreateWidget<GroupCollapsable>("Build");
        auto &columns = generation_root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;
        
        GuiDrawer::draw_boolean(columns, "Development build", generate_gatherer<bool>("dev_build"),
                                generate_provider<bool>("dev_build"));
    }
    
    {
        /* Windowing settings */
        auto &windowing_root = CreateWidget<GroupCollapsable>("Windowing");
        auto &columns = windowing_root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawScalar<int>(columns, "Resolution X", generate_gatherer<int>("x_resolution"),
                                   generate_provider<int>("x_resolution"), 1, 0, 10000);
        GuiDrawer::DrawScalar<int>(columns, "Resolution Y", generate_gatherer<int>("y_resolution"),
                                   generate_provider<int>("y_resolution"), 1, 0, 10000);
        GuiDrawer::draw_boolean(columns, "Fullscreen", generate_gatherer<bool>("fullscreen"),
                                generate_provider<bool>("fullscreen"));
        GuiDrawer::DrawString(columns, "Executable name", generate_gatherer<std::string>("executable_name"),
                              generate_provider<std::string>("executable_name"));
    }
    
    {
        /* Rendering settings */
        auto &rendering_root = CreateWidget<GroupCollapsable>("Rendering");
        auto &columns = rendering_root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;
        
        GuiDrawer::draw_boolean(columns, "Vertical Sync.", generate_gatherer<bool>("vsync"),
                                generate_provider<bool>("vsync"));
        GuiDrawer::draw_boolean(columns, "Multi-sampling", generate_gatherer<bool>("multisampling"),
                                generate_provider<bool>("multisampling"));
        GuiDrawer::DrawScalar<int>(columns, "Samples", generate_gatherer<int>("samples"),
                                   generate_provider<int>("samples"), 1, 2, 16);
        GuiDrawer::DrawScalar<int>(columns, "OpenGL Major", generate_gatherer<int>("opengl_major"),
                                   generate_provider<int>("opengl_major"), 1, 3, 4);
        GuiDrawer::DrawScalar<int>(columns, "OpenGL Minor", generate_gatherer<int>("opengl_minor"),
                                   generate_provider<int>("opengl_minor"), 1, 0, 6);
    }
    
    {
        /* Scene Management settings */
        auto &game_root = CreateWidget<GroupCollapsable>("Scene Management");
        auto &columns = game_root.CreateWidget<Columns<2>>();
        columns.widths_[0] = 125;

        GuiDrawer::DrawDdstring(columns, "Start scene", generate_gatherer<std::string>("start_scene"),
                                generate_provider<std::string>("start_scene"), "File");
    }
}

void ProjectSettings::reset_project_settings() {
    project_settings_.remove_all();
    project_settings_.add<float>("gravity", -9.81f);
    project_settings_.add<int>("x_resolution", 1280);
    project_settings_.add<int>("y_resolution", 720);
    project_settings_.add<bool>("fullscreen", false);
    project_settings_.add<std::string>("executable_name", "Game");
    project_settings_.add<std::string>("start_scene", "Scene.scene");
    project_settings_.add<bool>("vsync", true);
    project_settings_.add<bool>("multisampling", false);
    project_settings_.add<int>("samples", 1);
    project_settings_.add<int>("vulkan_major", 0);
    project_settings_.add<int>("vulkan_minor", 1);
    project_settings_.add<bool>("dev_build", true);
}

bool ProjectSettings::is_project_settings_integrity_verified() {
    return
    project_settings_.is_key_existing("gravity") &&
    project_settings_.is_key_existing("x_resolution") &&
    project_settings_.is_key_existing("y_resolution") &&
    project_settings_.is_key_existing("fullscreen") &&
    project_settings_.is_key_existing("executable_name") &&
    project_settings_.is_key_existing("start_scene") &&
    project_settings_.is_key_existing("vsync") &&
    project_settings_.is_key_existing("multisampling") &&
    project_settings_.is_key_existing("samples") &&
    project_settings_.is_key_existing("vulkan_major") &&
    project_settings_.is_key_existing("vulkan_minor") &&
    project_settings_.is_key_existing("dev_build");
}

fs::IniFile &ProjectSettings::project_settings() {
    return project_settings_;
}

void ProjectSettings::apply_project_settings() {
    //todo
}

}
