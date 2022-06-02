//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/timing.h"

namespace vox::force {
std::unordered_map<int, AverageTime> Timing::m_average_times;
std::stack<TimingHelper> Timing::m_timing_stack;
bool Timing::m_dont_print_times = false;
unsigned int Timing::m_start_counter = 0;
unsigned int Timing::m_stop_counter = 0;
}  // namespace vox::force
