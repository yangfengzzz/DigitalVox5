//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#pragma once

#include <memory>
#include <string>
#include <vector>

#include "utils.h"
#include "vk_common.h"
#include "core/device.h"
#include "platform/application.h"
#include "platform/filesystem.h"
#include "platform/parser.h"
#include "platform/plugins/plugin.h"
#include "platform/window.h"
#include "rendering/render_context.h"

#if defined(VK_USE_PLATFORM_XLIB_KHR)
#	undef Success
#endif

namespace vox {
enum class ExitCode {
    SUCCESS = 0, /* App executed as expected */
    HELP,        /* App should show help */
    CLOSE,       /* App has been requested to close at initialization */
    FATAL_ERROR   /* App encountered an unexpected error */
};

class Platform {
public:
    Platform() = default;
    
    virtual ~Platform() = default;
    
    /**
     * @brief Initialize the platform
     * @param plugins plugins available to the platform
     * @return An exit code representing the outcome of initialization
     */
    virtual ExitCode initialize(const std::vector<Plugin *> &plugins);
    
    /**
     * @brief Handles the main loop of the platform
     * This should be overridden if a platform requires a specific main loop setup.
     * @return An exit code representing the outcome of the loop
     */
    ExitCode main_loop();
    
    /**
     * @brief Runs the application for one frame
     */
    void update();
    
    /**
     * @brief Terminates the platform and the application
     * @param code Determines how the platform should exit
     */
    virtual void terminate(ExitCode code);
    
    /**
     * @brief Requests to close the platform at the next available point
     */
    virtual void close();
    
    virtual std::unique_ptr<RenderContext> create_render_context(Device &device, VkSurfaceKHR surface,
                                                                 const std::vector<VkSurfaceFormatKHR> &surface_format_priority) const;
    
    virtual void resize(uint32_t width, uint32_t height);
    
    virtual void input_event(const InputEvent &input_event);
    
public:
    Window &get_window();
    
    [[nodiscard]] Application &get_app() const;
    
    Application &get_app();
    
    void set_app(std::unique_ptr<Application> &&active_app);
    
    bool start_app();
    
public:
    void set_focus(bool focused);
    
    void force_simulation_fps(float fps);
    
    void disable_input_processing();
    
public:
    void set_window_properties(const Window::OptionalProperties &properties);
    
    /**
     * @brief Returns the working directory of the application set by the platform
     * @returns The path to the working directory
     */
    static const std::string &get_external_storage_directory();
    
    /**
     * @brief Returns the suitable directory for temporary files from the environment variables set in the system
     * @returns The path to the temp folder on the system
     */
    static const std::string &get_temp_directory();
    
    static std::vector<std::string> &get_arguments();
    
    static void set_arguments(const std::vector<std::string> &args);
    
    static void set_external_storage_directory(const std::string &dir);
    
    static void set_temp_directory(const std::string &dir);
    
    static const uint32_t min_window_width_;
    static const uint32_t min_window_height_;
    
public:
    /**
     * @return The VkInstance extension name for the platform
     */
    virtual const char *get_surface_extension() = 0;
    
    template<class T>
    T *get_plugin() const;
    
    template<class T>
    [[nodiscard]] bool using_plugin() const;
    
    void on_post_draw(RenderContext &context);
    
protected:
    std::unique_ptr<CommandParser> parser_;
    
    std::vector<Plugin *> active_plugins_;
    
    std::unordered_map<Hook, std::vector<Plugin *>> hooks_;
    
    std::unique_ptr<Window> window_{nullptr};
    
    std::unique_ptr<Application> active_app_{nullptr};
    
    virtual std::vector<spdlog::sink_ptr> get_platform_sinks();
    
    /**
     * @brief Handles the creation of the window
     *
     * @param properties Preferred window configuration
     */
    virtual void create_window(const Window::Properties &properties) = 0;
    
    void on_update(float delta_time);
    
    void on_app_error(const std::string &app_id);
    
    void on_app_start(const std::string &app_id);
    
    void on_app_close(const std::string &app_id);
    
    void on_platform_close();
    
    Window::Properties window_properties_;              /* Source of truth for window state */
    bool fixed_simulation_fps_{false};    /* Delta time should be fixed with a fabricated value */
    float simulation_frame_time_ = 0.016f; /* A fabricated delta time */
    bool process_input_events_{true};     /* App should continue processing input events */
    bool focused_{true};                  /* App is currently in focus at an operating system level */
    bool close_requested_{false};         /* Close requested */
    
private:
    Timer timer_;
    
    std::vector<Plugin *> plugins_;
    
    /// Static so can be set via JNI code in android_platform.cpp
    static std::vector<std::string> arguments_;
    
    static std::string external_storage_directory_;
    
    static std::string temp_directory_;
};

template<class T>
bool Platform::using_plugin() const {
    return !plugins::with_tags<T>(active_plugins_).empty();
}

template<class T>
T *Platform::get_plugin() const {
    assert(using_plugin<T>() && "Plugin is not enabled but was requested");
    const auto kPlugins = plugins::with_tags<T>(active_plugins_);
    return dynamic_cast<T *>(kPlugins[0]);
}

}        // namespace vox
