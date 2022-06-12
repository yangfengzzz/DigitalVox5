//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/time_manager.h"

namespace vox::compute {

TimeManager* TimeManager::current_ = nullptr;

TimeManager::TimeManager() {
    time_ = 0;
    h_ = static_cast<Real>(0.005);
}

TimeManager::~TimeManager() { current_ = nullptr; }

TimeManager* TimeManager::GetCurrent() {
    if (current_ == nullptr) {
        current_ = new TimeManager();
    }
    return current_;
}

void TimeManager::SetCurrent(TimeManager* tm) { current_ = tm; }

bool TimeManager::HasCurrent() { return (current_ != nullptr); }

Real TimeManager::GetTime() const { return time_; }

void TimeManager::SetTime(Real t) { time_ = t; }

Real TimeManager::GetTimeStepSize() const { return h_; }

void TimeManager::SetTimeStepSize(Real tss) { h_ = tss; }

}  // namespace vox::compute
