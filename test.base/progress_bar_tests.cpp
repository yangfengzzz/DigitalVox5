//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <chrono>
#include <thread>

#include "tests.h"
#include "vox.base/parallel.h"
#include "vox.base/progress_bar.h"

namespace vox::tests {

TEST(ProgressBar, ProgressBar) {
    int iterations = 1000;
    utility::ProgressBar progress_bar(iterations, "ProgressBar test: ", true);

    for (int i = 0; i < iterations; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        ++progress_bar;
    }
    EXPECT_EQ(iterations, static_cast<int>(progress_bar.GetCurrentCount()));
}

TEST(ProgressBar, OMPProgressBar) {
    int iterations = 1000;
    utility::OMPProgressBar progress_bar(iterations, "OMPProgressBar test: ", true);

#pragma omp parallel for schedule(static) num_threads(utility::EstimateMaxThreads())
    for (int i = 0; i < iterations; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ++progress_bar;
    }
    EXPECT_TRUE(static_cast<int>(progress_bar.GetCurrentCount()) >= iterations);
}

}  // namespace vox::tests
