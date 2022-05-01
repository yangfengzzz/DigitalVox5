//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/texts/text_colored.h"
#include "logging.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class Console : public PanelWindow {
public:
    Console(const std::string &title,
            bool opened,
            const PanelWindowSettings &window_settings);
    
    /**
     * Method called when a log event occurred
     */
    void on_log_intercepted(spdlog::level::level_enum log_level,
                            const std::string &message);
    
    /**
     * Called when the scene plays. It will clear the console if the "Clear on play" settings is on
     */
    void clear_on_play();
    
    /**
     * Clear the console
     */
    void clear();
    
    /**
     * Filter logs using defined filters
     */
    void filter_logs();
    
    /**
     * Verify if a given log level is allowed by the current filter
     */
    bool is_allowed_by_filter(spdlog::level::level_enum log_level) const;
    
private:
    void set_show_info_logs(bool value);
    
    void set_show_warning_logs(bool value);
    
    void set_show_error_logs(bool value);
    
private:
    Group *log_group_{nullptr};
    std::unordered_map<TextColored *, spdlog::level::level_enum> log_text_widgets_{};
    
    bool clear_on_play_ = true;
    bool show_info_log_ = true;
    bool show_warning_log_ = true;
    bool show_error_log_ = true;
};

}
}
