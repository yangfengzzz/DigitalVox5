//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "debug_info.h"
#include "platform/configuration.h"
#include "platform/input_events.h"
#include "timer.h"

namespace vox {
class Window;

class Platform;

class Application {
public:
    Application();
    
    virtual ~Application() = default;
    
    /**
     * @brief Prepares the application for execution
     * @param platform The platform the application is being run on
     */
    virtual bool prepare(Platform &platform);
    
    /**
     * @brief Updates the application
     * @param delta_time The time since the last update
     */
    virtual void update(float delta_time);
    
    /**
     * @brief Handles cleaning up the application
     */
    virtual void finish();
    
    /**
     * @brief Handles resizing of the window
     * @param width New width of the window
     * @param height New height of the window
     */
    virtual bool resize(uint32_t width, uint32_t height);
    
    /**
     * @brief Handles input events of the window
     * @param input_event The input event object
     */
    virtual void input_event(const InputEvent &input_event);
    
    [[nodiscard]] const std::string &get_name() const;
    
    void set_name(const std::string &name);
    
    DebugInfo &get_debug_info();
    
protected:
    float fps_{0.0f};
    
    float frame_time_{0.0f};        // In ms
    
    uint32_t frame_count_{0};
    
    uint32_t last_frame_count_{0};
    
    Platform *platform_{nullptr};
    
private:
    std::string name_{};
    
    // The debug info of the app
    DebugInfo debug_info_{};
};

}        // namespace vox
