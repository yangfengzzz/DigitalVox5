//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/common.h"

namespace vox::force {
class TimeManager {
private:
    Real time;
    static TimeManager* current;
    Real h;

public:
    TimeManager();
    ~TimeManager();

    // Singleton
    static TimeManager* getCurrent();
    static void setCurrent(TimeManager* tm);
    static bool hasCurrent();

    Real getTime();
    void setTime(Real t);
    Real getTimeStepSize();
    void setTimeStepSize(Real tss);
};
}  // namespace vox::force