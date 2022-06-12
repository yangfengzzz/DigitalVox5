//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.compute/common.h"

namespace vox::compute {
class TimeManager {
private:
    Real time_;
    static TimeManager* current_;
    Real h_;

public:
    TimeManager();
    ~TimeManager();

    // Singleton
    static TimeManager* GetCurrent();
    static void SetCurrent(TimeManager* tm);
    static bool HasCurrent();

    [[nodiscard]] Real GetTime() const;
    void SetTime(Real t);
    [[nodiscard]] Real GetTimeStepSize() const;
    void SetTimeStepSize(Real tss);
};
}  // namespace vox::compute
