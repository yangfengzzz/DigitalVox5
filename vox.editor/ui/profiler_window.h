//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "profiling/profiler.h"

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/texts/text_colored.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/buttons/button_simple.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class ProfilerWindow : public PanelWindow {
public:
    /**
     * Constructor
     */
    ProfilerWindow(const std::string &title,
                   bool opened,
                   const PanelWindowSettings &window_settings,
                   float frequency);
    
    /**
     * Update profiling information
     */
    void update(float delta_time);
    
    /**
     * Enable or disable the profiler
     */
    void enable(bool value, bool disable_log = false);
    
private:
    [[nodiscard]] static Color calculate_action_color(double percentage);
    
    static std::string generate_action_string(ProfilerReport::Action &action);
    
private:
    enum class ProfilingMode {
        DEFAULT,
        CAPTURE
    };
    
    float frequency_;
    float timer_ = 0.f;
    float fps_timer_ = 0.f;
    ProfilingMode profiling_mode_ = ProfilingMode::DEFAULT;
    
    Profiler profiler_;
    
    Widget *separator_;
    ButtonSimple *capture_resume_button_;
    TextColored *fps_text_;
    TextColored *elapsed_frames_text_;
    TextColored *elapsed_time_text_;
    Columns<5> *action_list_;
};

}
}
