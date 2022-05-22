//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <string>
#include <utility>

#include "vox.base/progress_bar.h"

namespace vox::utility {

/// Progress reporting through update_progress(double percent) function.
/// If you have a set number of items to process (or bytes to load),
/// CountingProgressReporter will convert that to percentages (you still have to
/// specify how many items you have, of course)
class CountingProgressReporter {
public:
    explicit CountingProgressReporter(std::function<bool(double)> f) { update_progress_ = std::move(f); }
    void SetTotal(int64_t total) { total_ = total; }
    bool Update(int64_t count) {
        if (!update_progress_) return true;
        last_count_ = count;
        double percent = 0;
        if (total_ > 0) {
            if (count < total_) {
                percent = count * 100.0 / total_;
            } else {
                percent = 100.0;
            }
        }
        return CallUpdate(percent);
    }
    void Finish() { CallUpdate(100); }
    // for compatibility with ProgressBar
    void operator++() { Update(last_count_ + 1); }

private:
    bool CallUpdate(double percent) {
        if (update_progress_) {
            return update_progress_(percent);
        }
        return true;
    }
    std::function<bool(double)> update_progress_;
    int64_t total_ = -1;
    int64_t last_count_ = -1;
};

/// update_progress(double percent) functor for ProgressBar
struct ConsoleProgressUpdater {
    explicit ConsoleProgressUpdater(const std::string &progress_info, bool active = false)
        : progress_bar_(100, progress_info, active) {}
    bool operator()(double pct) {
        while (last_pct < pct) {
            ++last_pct;
            ++progress_bar;
        }
        return true;
    }

private:
    utility::ProgressBar progress_bar{};
    int last_pct = 0;
};

}  // namespace vox::utility
