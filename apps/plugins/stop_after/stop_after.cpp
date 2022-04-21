//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "stop_after.h"

namespace plugins {
StopAfter::StopAfter() :
StopAfterTags("Stop After X",
              "A collection of flags to stop the running application after a set period.",
              {vox::Hook::ON_UPDATE}, {&stop_after_frame_flag}) {
}

bool StopAfter::is_active(const vox::CommandParser &parser) {
    return parser.contains(&stop_after_frame_flag);
}

void StopAfter::init(const vox::CommandParser &parser) {
    remaining_frames = parser.as<uint32_t>(&stop_after_frame_flag);
}

void StopAfter::on_update(float delta_time) {
    remaining_frames--;
    
    if (remaining_frames <= 0) {
        platform_->close();
    }
}
}        // namespace plugins
