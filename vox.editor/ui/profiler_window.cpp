//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "profiler_window.h"
#include "ui/widgets/visual/separator.h"
#include "logging.h"

namespace vox::editor::ui {
ProfilerWindow::ProfilerWindow(const std::string &title,
                               bool opened,
                               const PanelWindowSettings &window_settings,
                               float frequency) :
PanelWindow(title, opened, window_settings),
frequency_(frequency) {
    allow_horizontal_scrollbar_ = true;

    CreateWidget<Text>("Profiler state: ").line_break_ = false;
    CreateWidget<CheckBox>(false, "").value_changed_event_ += std::bind(&ProfilerWindow::enable, this, std::placeholders::_1, false);
    
    fps_text_ = &CreateWidget<TextColored>("");
    capture_resume_button_ = &CreateWidget<ButtonSimple>("Capture");
    capture_resume_button_->idle_background_color_ = {0.7f, 0.5f, 0.f};
    capture_resume_button_->clicked_event_ += [this] {
        profiling_mode_ = profiling_mode_ == ProfilingMode::CAPTURE ? ProfilingMode::DEFAULT : ProfilingMode::CAPTURE;
        capture_resume_button_->label_ = profiling_mode_ == ProfilingMode::CAPTURE ? "Resume" : "Capture";
    };
    elapsed_frames_text_ = &CreateWidget<TextColored>("", Color(1.f, 0.8f, 0.01f, 1));
    elapsed_time_text_ = &CreateWidget<TextColored>("", Color(1.f, 0.8f, 0.01f, 1));
    separator_ = &CreateWidget<::vox::ui::Separator>();
    action_list_ = &CreateWidget<Columns<5>>();
    action_list_->widths_ = {300.f, 100.f, 100.f, 100.f, 200.f};
    
    enable(false, true);
}

void ProfilerWindow::update(float delta_time) {
    timer_ += delta_time;
    fps_timer_ += delta_time;
    
    while (fps_timer_ >= 0.07f) {
        fps_text_->content_ = "FPS: " + std::to_string(static_cast<int>(1.0f / delta_time));
        fps_timer_ -= 0.07f;
    }
    
    if (Profiler::is_enabled()) {
        Profiler::update(delta_time);
        
        while (timer_ >= frequency_) {
            if (profiling_mode_ == ProfilingMode::DEFAULT) {
                ProfilerReport report = profiler_.generate_report();
                profiler_.clear_history();
                action_list_->RemoveAllWidgets();
                
                elapsed_frames_text_->content_ = "Elapsed frames: " + std::to_string(report.elapsed_frames);
                elapsed_time_text_->content_ = "Elapsed time: " + std::to_string(report.elapsed_time);

                action_list_->CreateWidget<Text>("Action");
                action_list_->CreateWidget<Text>("Total duration");
                action_list_->CreateWidget<Text>("Frame duration");
                action_list_->CreateWidget<Text>("Frame load");
                action_list_->CreateWidget<Text>("Total calls");
                
                for (auto &action : report.actions) {
                    auto color = calculate_action_color(action.percentage);
                    action_list_->CreateWidget<TextColored>(action.name, color);
                    action_list_->CreateWidget<TextColored>(std::to_string(action.duration) + "s", color);
                    action_list_->CreateWidget<TextColored>(std::to_string(action.duration / action.calls) + "s",
                                                            color);
                    action_list_->CreateWidget<TextColored>(std::to_string(action.percentage) + "%%", color);
                    action_list_->CreateWidget<TextColored>(std::to_string(action.calls) + " calls", color);
                }
            }
            
            timer_ -= frequency_;
        }
    }
}

void ProfilerWindow::enable(bool value, bool disable_log) {
    if (value) {
        if (!disable_log)
            LOGI("Profiling started!")
            vox::Profiler::enable();
    } else {
        if (!disable_log)
            LOGI("Profiling stop!")
            vox::Profiler::disable();
        profiler_.clear_history();
        action_list_->RemoveAllWidgets();
    }
    
    capture_resume_button_->enabled_ = value;
    elapsed_frames_text_->enabled_ = value;
    elapsed_time_text_->enabled_ = value;
    separator_->enabled_ = value;
}

Color ProfilerWindow::calculate_action_color(double percentage) {
    if (percentage <= 25.0f) return {0.0f, 1.0f, 0.0f, 1.0f};
    else if (percentage <= 50.0f) return {1.0f, 1.0f, 0.0f, 1.0f};
    else if (percentage <= 75.0f) return {1.0f, 0.6f, 0.0f, 1.0f};
    else return {1.0f, 0.0f, 0.0f, 1.0f};
}

std::string ProfilerWindow::generate_action_string(ProfilerReport::Action &action) {
    std::string result;
    
    result += "[" + action.name + "]";
    result += std::to_string(action.duration) + "s (total) | ";
    result += std::to_string(action.duration / action.calls) + "s (per call) | ";
    result += std::to_string(action.percentage) + "%% | ";
    result += std::to_string(action.calls) + " calls";
    
    return result;
}

}
