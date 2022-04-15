//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "timer.h"

namespace vox {
Timer::Timer() :
start_time_{Clock::now()},
previous_tick_{Clock::now()} {
}

void Timer::start() {
    if (!running_) {
        running_ = true;
        start_time_ = Clock::now();
    }
}

void Timer::lap() {
    lapping_ = true;
    lap_time_ = Clock::now();
}

bool Timer::is_running() const {
    return running_;
}

}        // namespace vox
