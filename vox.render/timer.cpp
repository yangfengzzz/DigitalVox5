//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/timer.h"

namespace vox {
Timer::Timer() : start_time_{Clock::now()}, previous_tick_{Clock::now()} {}

void Timer::Start() {
    if (!running_) {
        running_ = true;
        start_time_ = Clock::now();
    }
}

void Timer::Lap() {
    lapping_ = true;
    lap_time_ = Clock::now();
}

bool Timer::IsRunning() const { return running_; }

}  // namespace vox
