//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "screenshot.h"

#include <chrono>
#include <iomanip>

#include "rendering/render_context.h"

namespace plugins {
Screenshot::Screenshot() :
ScreenshotTags("Screenshot",
               "Save a screenshot of a specific frame",
               {vox::Hook::ON_UPDATE, vox::Hook::ON_APP_START, vox::Hook::POST_DRAW},
               {&screenshot_flag, &screenshot_output_flag}) {
}

bool Screenshot::is_active(const vox::CommandParser &parser) {
    return parser.contains(&screenshot_flag);
}

void Screenshot::init(const vox::CommandParser &parser) {
    if (parser.contains(&screenshot_flag)) {
        frame_number = parser.as<uint32_t>(&screenshot_flag);
        
        if (parser.contains(&screenshot_output_flag)) {
            output_path = parser.as<std::string>(&screenshot_output_flag);
            output_path_set = true;
        }
    }
}

void Screenshot::on_update(float delta_time) {
    current_frame++;
}

void Screenshot::on_app_start(const std::string &name) {
    current_app_name = name;
    current_frame = 0;
}

void Screenshot::on_post_draw(vox::RenderContext &context) {
    if (current_frame == frame_number) {
        if (!output_path_set) {
            // Create generic image path. <app name>-<current timestamp>.png
            auto timestamp = std::chrono::system_clock::now();
            std::time_t now_tt = std::chrono::system_clock::to_time_t(timestamp);
            std::tm tm = *std::localtime(&now_tt);
            
            char buffer[30];
            strftime(buffer, sizeof(buffer), "%G-%m-%d---%H-%M-%S", &tm);
            
            std::stringstream stream;
            stream << current_app_name << "-" << buffer;
            
            output_path = stream.str();
        }

        Screenshot(context, output_path);
    }
}
}        // namespace plugins
