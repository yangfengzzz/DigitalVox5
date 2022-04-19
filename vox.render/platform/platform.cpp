//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#include "platform.h"

#include <algorithm>
#include <vector>

#include <spdlog/async_logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "logging.h"
//#include "force_close/force_close.h"
#include "platform/filesystem.h"
#include "platform/parsers/CLI11.h"
#include "platform/plugins/plugin.h"

namespace vox {
const uint32_t Platform::min_window_width_ = 420;
const uint32_t Platform::min_window_height_ = 320;

std::vector<std::string> Platform::arguments_ = {};

std::string Platform::external_storage_directory_;

std::string Platform::temp_directory_;

ExitCode Platform::initialize(const std::vector<Plugin *> &plugins = {}) {
    auto sinks = get_platform_sinks();
    
    auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
    
#ifdef VKB_DEBUG
    logger->set_level(spdlog::level::debug);
#else
    logger->set_level(spdlog::level::info);
#endif
    
    logger->set_pattern(LOGGER_FORMAT);
    spdlog::set_default_logger(logger);
    
    LOGI("Logger initialized")
    
    parser_ = std::make_unique<Cli11CommandParser>("vulkan_samples",
                                                   "\n\tVulkan Samples\n\n\t\tA collection of samples to demonstrate the Vulkan best practice.\n",
                                                   arguments_);
    
    // Process command line arguments
    if (!parser_->parse(associate_plugins(plugins))) {
        return ExitCode::HELP;
    }
    
    // Subscribe plugins to requested hooks and store activated plugins
    for (auto *plugin : plugins) {
        if (plugin->activate_plugin(this, *parser_)) {
            auto &plugin_hooks = plugin->get_hooks();
            for (auto hook : plugin_hooks) {
                auto it = hooks_.find(hook);
                
                if (it == hooks_.end()) {
                    auto r = hooks_.emplace(hook, std::vector<Plugin *>{});
                    
                    if (r.second) {
                        it = r.first;
                    }
                }
                
                it->second.emplace_back(plugin);
            }
            
            active_plugins_.emplace_back(plugin);
        }
    }
    
    // Platform has been closed by a plugin initialization phase
    if (close_requested_) {
        return ExitCode::CLOSE;
    }
    
    create_window(window_properties_);
    
    if (!window_) {
        LOGE("Window creation failed!")
        return ExitCode::FATAL_ERROR;
    }
    
    return ExitCode::SUCCESS;
}

ExitCode Platform::main_loop() {
    // Load the requested app
    if (!start_app()) {
        LOGE("Failed to load requested application")
        return ExitCode::FATAL_ERROR;
    }
    
    // Compensate for load times of the app by rendering the first frame pre-emptively
    timer_.tick<Timer::Seconds>();
    active_app_->update(0.01667f);
    
    while (!window_->should_close() && !close_requested_) {
        try {
            update();
            
            window_->process_events();
        }
        catch (std::exception &e) {
            LOGE("Error Message: {}", e.what())
            LOGE("Failed when running application {}", active_app_->get_name())
            
            on_app_error(active_app_->get_name());
            return ExitCode::FATAL_ERROR;
        }
    }
    
    return ExitCode::SUCCESS;
}

void Platform::update() {
    auto delta_time = static_cast<float>(timer_.tick<Timer::Seconds>());
    
    if (focused_) {
        on_update(delta_time);
        
        if (fixed_simulation_fps_) {
            delta_time = simulation_frame_time_;
        }
        
        active_app_->update(delta_time);
    }
}

std::unique_ptr<RenderContext> Platform::create_render_context(Device &device, VkSurfaceKHR surface,
                                                               const std::vector<VkSurfaceFormatKHR> &surface_format_priority) const {
    assert(!surface_format_priority.empty() &&
           "Surface format priority list must contain at least one preferred surface format");
    
    auto extent = window_->get_extent();
    auto context = std::make_unique<RenderContext>(device, surface, extent.width, extent.height);
    
    context->set_surface_format_priority(surface_format_priority);
    
    context->request_image_format(surface_format_priority[0].format);
    
    context->set_present_mode_priority({
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_FIFO_KHR,
        VK_PRESENT_MODE_IMMEDIATE_KHR,
    });
    
    switch (window_properties_.vsync) {
        case Window::Vsync::ON:context->request_present_mode(VK_PRESENT_MODE_FIFO_KHR);
            break;
        case Window::Vsync::OFF:
        default:context->request_present_mode(VK_PRESENT_MODE_MAILBOX_KHR);
            break;
    }
    
    return context;
}

void Platform::terminate(ExitCode code) {
    if (code == ExitCode::HELP) {
        auto help = parser_->help();
        for (auto &line : help) {
            LOGI(line)
        }
    }
    
    if (active_app_) {
        std::string id = active_app_->get_name();
        
        on_app_close(id);
        
        active_app_->finish();
    }
    
    active_app_.reset();
    window_.reset();
    
    spdlog::drop_all();
    
    on_platform_close();
    
    // Halt on all unsuccessful exit codes unless ForceClose is in use
    //    if (code != ExitCode::Success && !using_plugin<::plugins::ForceClose>()) {
    //#ifndef ANDROID
    //        std::cout << "Press any key to continue";
    //        std::cin.get();
    //#endif
    //    }
}

void Platform::close() {
    if (window_) {
        window_->close();
    }
    
    // Fallback incase a window is not yet in use
    close_requested_ = true;
}

void Platform::force_simulation_fps(float fps) {
    fixed_simulation_fps_ = true;
    simulation_frame_time_ = 1 / fps;
}

void Platform::disable_input_processing() {
    process_input_events_ = false;
}

void Platform::set_focus(bool focused) {
    focused_ = focused;
}

void Platform::set_window_properties(const Window::OptionalProperties &properties) {
    window_properties_.title = properties.title.has_value() ? properties.title.value() : window_properties_.title;
    window_properties_.mode = properties.mode.has_value() ? properties.mode.value() : window_properties_.mode;
    window_properties_.resizable = properties.resizable.has_value() ? properties.resizable.value()
    : window_properties_.resizable;
    window_properties_.vsync = properties.vsync.has_value() ? properties.vsync.value() : window_properties_.vsync;
    window_properties_.extent.width = properties.extent.width.has_value() ? properties.extent.width.value()
    : window_properties_.extent.width;
    window_properties_.extent.height = properties.extent.height.has_value() ? properties.extent.height.value()
    : window_properties_.extent.height;
}

const std::string &Platform::get_external_storage_directory() {
    return external_storage_directory_;
}

const std::string &Platform::get_temp_directory() {
    return temp_directory_;
}

Application &Platform::get_app() {
    assert(active_app_ && "Application is not valid");
    return *active_app_;
}

Application &Platform::get_app() const {
    assert(active_app_ && "Application is not valid");
    return *active_app_;
}

Window &Platform::get_window() {
    return *window_;
}

std::vector<std::string> &Platform::get_arguments() {
    return Platform::arguments_;
}

void Platform::set_arguments(const std::vector<std::string> &args) {
    arguments_ = args;
}

void Platform::set_external_storage_directory(const std::string &dir) {
    external_storage_directory_ = dir;
}

void Platform::set_temp_directory(const std::string &dir) {
    temp_directory_ = dir;
}

std::vector<spdlog::sink_ptr> Platform::get_platform_sinks() {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    return sinks;
}

void Platform::set_app(std::unique_ptr<Application> &&new_app) {
    if (active_app_) {
        auto execution_time = timer_.stop();
        LOGI("Closing App (Runtime: {:.1f})", execution_time)
        
        auto app_id = active_app_->get_name();
        
        active_app_->finish();
    }
    active_app_ = std::move(new_app);
}

bool Platform::start_app() {
    active_app_->set_name("");
    
    if (!active_app_) {
        LOGE("Failed to create a valid vulkan app.")
        return false;
    }
    
    if (!active_app_->prepare(*this)) {
        LOGE("Failed to prepare vulkan app.")
        return false;
    }
    
    on_app_start("");
    
    return true;
}

void Platform::input_event(const InputEvent &input_event) {
    if (process_input_events_ && active_app_) {
        active_app_->input_event(input_event);
    }
    
    if (input_event.get_source() == EventSource::KEYBOARD) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
        
        if (key_event.get_code() == KeyCode::BACK ||
            key_event.get_code() == KeyCode::ESCAPE) {
            close();
        }
    }
}

void Platform::resize(uint32_t win_width, uint32_t win_height,
                      uint32_t fb_width, uint32_t fb_height) {
    auto extent = Window::Extent{std::max<uint32_t>(win_width, min_window_width_),
        std::max<uint32_t>(win_height, min_window_height_)};
    if (window_) {
        window_->resize(extent);
        if (active_app_) {
            active_app_->resize(win_width, win_height, fb_width, fb_height);
        }
    }
}

#define HOOK(enum, func)                \
    static auto res = hooks_.find(enum); \
    if (res != hooks_.end())             \
    {                                   \
        for (auto plugin : res->second) \
        {                               \
            plugin->func;               \
        }                               \
    }

void Platform::on_post_draw(RenderContext &context) {
    HOOK(Hook::POST_DRAW, on_post_draw(context))
}

void Platform::on_app_error(const std::string &app_id) {
    HOOK(Hook::ON_APP_ERROR, on_app_error(app_id))
}

void Platform::on_update(float delta_time) {
    HOOK(Hook::ON_UPDATE, on_update(delta_time))
}

void Platform::on_app_start(const std::string &app_id) {
    HOOK(Hook::ON_APP_START, on_app_start(app_id))
}

void Platform::on_app_close(const std::string &app_id) {
    HOOK(Hook::ON_APP_CLOSE, on_app_close(app_id))
}

void Platform::on_platform_close() {
    HOOK(Hook::ON_PLATFORM_CLOSE, on_platform_close())
}

#undef HOOK

}        // namespace vox
