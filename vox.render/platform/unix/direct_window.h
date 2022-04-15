//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <termios.h>
#include <unistd.h>

#include "vk_common.h"
#include "platform/platform.h"
#include "platform/window.h"

namespace vox {
/**
 * @brief Direct2Display window
 */
class DirectWindow : public Window {
public:
    DirectWindow(Platform *platform, const Window::Properties &properties);
    
    virtual ~DirectWindow();
    
    virtual VkSurfaceKHR create_surface(Instance &instance) override;
    
    virtual VkSurfaceKHR create_surface(VkInstance instance, VkPhysicalDevice physical_device) override;
    
    virtual bool should_close() override;
    
    virtual void process_events() override;
    
    virtual void close() override;
    
    float get_dpi_factor() const override;
    
private:
    void poll_terminal();
    
    uint32_t find_compatible_plane(VkPhysicalDevice phys_dev, VkDisplayKHR display,
                                   const std::vector<VkDisplayPlanePropertiesKHR> &plane_properties);
    
private:
    mutable bool keep_running = true;
    Platform *platform = nullptr;
    float dpi;
    int tty_fd;
    struct termios termio;
    struct termios termio_prev;
    KeyCode key_down = KeyCode::Unknown;
};

}        // namespace vox
