//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "fps_logger.h"

namespace plugins {
FpsLogger::FpsLogger() :
FpsLoggerTags("FPS Logger",
              "Enable FPS logging.",
              {vox::Hook::ON_UPDATE, vox::Hook::ON_APP_START}, {&fps_flag}) {
}

bool FpsLogger::is_active(const vox::CommandParser &parser) {
    return parser.contains(&fps_flag);
}

void FpsLogger::init(const vox::CommandParser &parser) {
}

void FpsLogger::on_update(float delta_time) {
    if (!timer.is_running()) {
        timer.start();
    }
    
    auto elapsed_time = static_cast<float>(timer.elapsed<vox::Timer::Seconds>());
    
    frame_count++;
    
    if (elapsed_time > 0.5f) {
        auto fps = (frame_count - last_frame_count) / elapsed_time;
        
        LOGI("FPS: {:.1f}", fps);
        
        last_frame_count = frame_count;
        timer.lap();
    }
};
}        // namespace plugins
