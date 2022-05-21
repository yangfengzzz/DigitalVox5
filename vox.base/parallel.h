//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox::utility {

/// Estimate the maximum number of threads to be used in a parallel region.
int EstimateMaxThreads();

/// Returns true if in an parallel section.
bool InParallel();

}  // namespace vox
