//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/discregrid/timing.h"

namespace vox::compute::discregrid {

std::unordered_map<int, AverageTime> Timing::m_averageTimes;
std::stack<TimingHelper> Timing::m_timingStack;
bool Timing::m_dontPrintTimes = false;
unsigned int Timing::m_startCounter = 0;
unsigned int Timing::m_stopCounter = 0;
unsigned int Timing::m_id_counter = 0;

}  // namespace vox::compute::discregrid
