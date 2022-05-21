//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tool_bar.h"
#include "ui/widgets/layout/spacing.h"
#include "editor_actions.h"
#include "editor_resources.h"

namespace vox::editor::ui {

Toolbar::Toolbar(const std::string &title, bool opened,
                 const PanelWindowSettings &window_settings,
                 EditorResources *resource) :
PanelWindow(title, opened, window_settings),
resource_(resource) {
    std::string icon_folder = ":Textures/Icons/";
    
    play_button_ = &CreateWidget<ButtonImage>(resource_->get_texture("Button_Play"), Vector2F{20, 20});
    pause_button_ = &CreateWidget<ButtonImage>(resource_->get_texture("Button_Pause"), Vector2F{20, 20});
    stop_button_ = &CreateWidget<ButtonImage>(resource_->get_texture("Button_Stop"), Vector2F{20, 20});
    next_button_ = &CreateWidget<ButtonImage>(resource_->get_texture("Button_Next"), Vector2F{20, 20});

    CreateWidget<Spacing>(0).line_break_ = false;
    auto &refresh_button = CreateWidget<ButtonImage>(resource_->get_texture("Button_Refresh"), Vector2F{20, 20});
    
    play_button_->line_break_ = false;
    pause_button_->line_break_ = false;
    stop_button_->line_break_ = false;
    next_button_->line_break_ = false;
    refresh_button.line_break_ = false;
    
    play_button_->clicked_event_ +=
    std::bind(&EditorActions::start_playing, EditorActions::GetSingletonPtr());
    
    pause_button_->clicked_event_ +=
    std::bind(&EditorActions::pause_game, EditorActions::GetSingletonPtr());
    
    stop_button_->clicked_event_ +=
    std::bind(&EditorActions::stop_playing, EditorActions::GetSingletonPtr());
    
    next_button_->clicked_event_ +=
    std::bind(&EditorActions::next_frame, EditorActions::GetSingletonPtr());
    
    refresh_button.clicked_event_ +=
    std::bind(&EditorActions::refresh_scripts, EditorActions::GetSingletonPtr());
    
    EditorActions::GetSingleton().editor_mode_changed_event_ += [this](EditorActions::EditorMode new_mode) {
        auto enable = [](ButtonImage *button, bool enable) {
            button->disabled_ = !enable;
            button->tint_ = enable ? Color{1.0f, 1.0f, 1.0f, 1.0f} : Color{1.0f, 1.0f, 1.0f, 0.15f};
        };
        
        switch (new_mode) {
            case EditorActions::EditorMode::EDIT:enable(play_button_, true);
                enable(pause_button_, false);
                enable(stop_button_, false);
                enable(next_button_, false);
                break;
            case EditorActions::EditorMode::PLAY:enable(play_button_, false);
                enable(pause_button_, true);
                enable(stop_button_, true);
                enable(next_button_, true);
                break;
            case EditorActions::EditorMode::PAUSE:enable(play_button_, true);
                enable(pause_button_, false);
                enable(stop_button_, true);
                enable(next_button_, true);
                break;
            case EditorActions::EditorMode::FRAME_BY_FRAME:enable(play_button_, true);
                enable(pause_button_, false);
                enable(stop_button_, true);
                enable(next_button_, true);
                break;
        }
    };
    
    EditorActions::GetSingleton().set_editor_mode(EditorActions::EditorMode::EDIT);
}

void Toolbar::draw_impl() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    
    PanelWindow::draw_impl();
    
    ImGui::PopStyleVar();
}

}
