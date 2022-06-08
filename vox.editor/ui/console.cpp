//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/console.h"

#include "vox.editor/editor_actions.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/layout/spacing.h"
#include "vox.render/ui/widgets/selection/check_box.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
namespace {
template <typename Mutex>
class ConsoleSink : public spdlog::sinks::base_sink<Mutex> {
public:
    explicit ConsoleSink(Console *console) : console_(console) {}

    void OnDestroy() { console_ = nullptr; }

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override {
        if (console_) {
            // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
            // msg.raw contains preformatted log

            // If needed (very likely but not mandatory), the sink formats the message before sending it to its final
            // destination:
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
            console_->OnLogIntercepted(msg.level, fmt::to_string(formatted));
        }
    }

    void flush_() override {}

private:
    Console *console_;
};

}  // namespace

Console::Console(const std::string &title, bool opened, const PanelWindowSettings &window_settings)
    : PanelWindow(title, opened, window_settings) {
    console_sink_ = std::make_shared<ConsoleSink<spdlog::details::null_mutex>>(this);
    spdlog::default_logger()->sinks().push_back(console_sink_);

    allow_horizontal_scrollbar_ = true;

    auto &clear_button = CreateWidget<ButtonSimple>("Clear");
    clear_button.size_ = {50.f, 0.f};
    clear_button.idle_background_color_ = {0.5f, 0.f, 0.f};
    clear_button.clicked_event_ += std::bind(&Console::Clear, this);
    clear_button.line_break_ = false;

    auto &clear_on_play = CreateWidget<CheckBox>(clear_on_play_, "Auto clear on play");

    CreateWidget<Spacing>(5).line_break_ = false;

    auto &enable_info = CreateWidget<CheckBox>(true, "Info");
    auto &enable_warning = CreateWidget<CheckBox>(true, "Warning");
    auto &enable_error = CreateWidget<CheckBox>(true, "Error");

    clear_on_play.line_break_ = false;
    enable_info.line_break_ = false;
    enable_warning.line_break_ = false;
    enable_error.line_break_ = true;

    clear_on_play.value_changed_event_ += [this](bool value) { clear_on_play_ = value; };
    enable_info.value_changed_event_ += std::bind(&Console::SetShowInfoLogs, this, std::placeholders::_1);
    enable_warning.value_changed_event_ += std::bind(&Console::SetShowWarningLogs, this, std::placeholders::_1);
    enable_error.value_changed_event_ += std::bind(&Console::SetShowErrorLogs, this, std::placeholders::_1);

    CreateWidget<Separator>();

    log_group_ = &CreateWidget<Group>();
    log_group_->ReverseDrawOrder();

//    EditorActions::GetSingleton().play_event_ += std::bind(&Console::ClearOnPlay, this);
}

Console::~Console() {
    log_text_widgets_.clear();
    static_cast<ConsoleSink<spdlog::details::null_mutex> *>(console_sink_.get())->OnDestroy();
    console_sink_.reset();
}

void Console::OnLogIntercepted(spdlog::level::level_enum log_level, const std::string &message) {
    Color log_color;
    switch (log_level) {
        case spdlog::level::info:
            log_color = {0.f, 1.f, 1.f, 1.f};
            break;
        case spdlog::level::warn:
            log_color = {1.f, 1.f, 0.f, 1.f};
            break;
        case spdlog::level::err:
            log_color = {1.f, 0.f, 0.f, 1.f};
            break;
        default:
            log_color = {1.f, 1.f, 1.f, 1.f};
            break;
    }
    auto &console_item = log_group_->CreateWidget<TextColored>(message, log_color);
    console_item.enabled_ = IsAllowedByFilter(log_level);
    log_text_widgets_[&console_item] = log_level;
}

void Console::ClearOnPlay() {
    if (clear_on_play_) Clear();
}

void Console::Clear() {
    log_text_widgets_.clear();
    log_group_->RemoveAllWidgets();
}

void Console::FilterLogs() {
    for (const auto &[kWidget, kLogLevel] : log_text_widgets_) kWidget->enabled_ = IsAllowedByFilter(kLogLevel);
}

bool Console::IsAllowedByFilter(spdlog::level::level_enum log_level) const {
    switch (log_level) {
        case spdlog::level::info:
            return show_info_log_;
        case spdlog::level::warn:
            return show_warning_log_;
        case spdlog::level::err:
            return show_error_log_;
        default:
            return false;
    }

    return false;
}

void Console::SetShowInfoLogs(bool value) {
    show_info_log_ = value;
    FilterLogs();
}

void Console::SetShowWarningLogs(bool value) {
    show_warning_log_ = value;
    FilterLogs();
}

void Console::SetShowErrorLogs(bool value) {
    show_error_log_ = value;
    FilterLogs();
}

}  // namespace vox::editor::ui
