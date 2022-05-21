//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "logging.h"
#include "point_cloud.h"
#include "qhull.h"
#include "tetra_mesh.h"

namespace vox::geometry {

std::tuple<std::shared_ptr<TetraMesh>, std::vector<size_t>> TetraMesh::CreateFromPointCloud(
        const PointCloud& point_cloud) {
    if (point_cloud.points_.size() < 4) {
        LOGE("Not enough points to create a tetrahedral mesh.")
    }
    return Qhull::ComputeDelaunayTetrahedralization(point_cloud.points_);
}

}  // namespace vox::geometry
