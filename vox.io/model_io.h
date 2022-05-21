//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <string>

namespace vox {
namespace visualization {
namespace rendering {
struct TriangleMeshModel;
}
}  // namespace visualization

namespace io {

struct ReadTriangleModelOptions {
    /// Print progress to stdout about loading progress.
    /// Also see \p update_progress if you want to have your own progress
    /// indicators or to be able to cancel loading.
    bool print_progress = false;
    /// Callback to invoke as reading is progressing, parameter is percentage
    /// completion (0.-100.) return true indicates to continue loading, false
    /// means to try to stop loading and cleanup
    std::function<bool(double)> update_progress;
};

bool ReadTriangleModel(const std::string& filename,
                       visualization::rendering::TriangleMeshModel& model,
                       ReadTriangleModelOptions params = {});

}  // namespace io
}  // namespace vox
