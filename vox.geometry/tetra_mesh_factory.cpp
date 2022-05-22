//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/logging.h"
#include "vox.geometry/point_cloud.h"
#include "vox.geometry/qhull.h"
#include "vox.geometry/tetra_mesh.h"

namespace vox::geometry {

std::tuple<std::shared_ptr<TetraMesh>, std::vector<size_t>> TetraMesh::CreateFromPointCloud(
        const PointCloud& point_cloud) {
    if (point_cloud.points_.size() < 4) {
        LOGE("Not enough points to create a tetrahedral mesh.")
    }
    return Qhull::ComputeDelaunayTetrahedralization(point_cloud.points_);
}

}  // namespace vox::geometry
