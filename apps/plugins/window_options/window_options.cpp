//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "window_options.h"

#include <algorithm>

#include "platform/platform.h"
#include "platform/window.h"

namespace plugins {
WindowOptions::WindowOptions() :
WindowOptionsTags("Window Options",
                  "A collection of flags to configure window used when running the application. Implementation may differ between platforms",
                  {},
                  {&window_options_group}) {
}

bool WindowOptions::is_active(const vox::CommandParser &parser) {
    return true;
}

void WindowOptions::init(const vox::CommandParser &parser) {
    vox::Window::OptionalProperties properties;
    
    if (parser.contains(&width_flag)) {
        auto width = parser.as<uint32_t>(&width_flag);
        if (width < platform_->min_window_width_) {
            LOGD("[Window Options] {} is smaller than the minimum width {}, resorting to minimum width", width, platform_->min_window_width_);
            width = platform_->min_window_width_;
        }
        properties.extent.width = width;
    }
    
    if (parser.contains(&height_flag)) {
        auto height = parser.as<uint32_t>(&height_flag);
        if (height < platform_->min_window_height_) {
            LOGD("[Window Options] {} is smaller than the minimum height {}, resorting to minimum height",
                 height,
                 platform_->min_window_height_);
            height = platform_->min_window_height_;
        }
        properties.extent.height = height;
    }
    
    if (parser.contains(&headless_flag)) {
        properties.mode = vox::Window::Mode::HEADLESS;
    } else if (parser.contains(&fullscreen_flag)) {
        properties.mode = vox::Window::Mode::FULLSCREEN;
    } else if (parser.contains(&borderless_flag)) {
        properties.mode = vox::Window::Mode::FULLSCREEN_BORDERLESS;
    }
    
    if (parser.contains(&vsync_flag)) {
        std::string value = parser.as<std::string>(&vsync_flag);
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        if (value == "on") {
            properties.vsync = vox::Window::Vsync::ON;
        } else if (value == "off") {
            properties.vsync = vox::Window::Vsync::OFF;
        }
    }
    
    platform_->set_window_properties(properties);
}
}        // namespace plugins
