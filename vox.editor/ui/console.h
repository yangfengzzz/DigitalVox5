//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <spdlog/sinks/base_sink.h>

#include "vox.render/logging.h"
#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/texts/text_colored.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class Console : public PanelWindow {
public:
    Console(const std::string &title, bool opened, const PanelWindowSettings &window_settings);

    ~Console() override;

    /**
     * Method called when a log event occurred
     */
    void OnLogIntercepted(spdlog::level::level_enum log_level, const std::string &message);

    /**
     * Called when the scene plays. It will clear the console if the "Clear on play" settings is on
     */
    void ClearOnPlay();

    /**
     * Clear the console
     */
    void Clear();

    /**
     * Filter logs using defined filters
     */
    void FilterLogs();

    /**
     * Verify if a given log level is allowed by the current filter
     */
    [[nodiscard]] bool IsAllowedByFilter(spdlog::level::level_enum log_level) const;

private:
    void SetShowInfoLogs(bool value);

    void SetShowWarningLogs(bool value);

    void SetShowErrorLogs(bool value);

private:
    Group *log_group_{nullptr};
    std::unordered_map<TextColored *, spdlog::level::level_enum> log_text_widgets_{};
    std::shared_ptr<spdlog::sinks::base_sink<spdlog::details::null_mutex>> console_sink_{nullptr};

    bool clear_on_play_ = true;
    bool show_info_log_ = true;
    bool show_warning_log_ = true;
    bool show_error_log_ = true;
};

}  // namespace editor::ui
}  // namespace vox
