//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <numeric>
#include <unordered_map>

#include "vox.base/helper.h"
#include "vox.base/logging.h"
#include "vox.geometry/intersection_test.h"
#include "vox.geometry/point_cloud.h"
#include "vox.geometry/triangle_mesh.h"
#include "vox.geometry/voxel_grid.h"

namespace vox::geometry {

std::shared_ptr<VoxelGrid> VoxelGrid::CreateDense(const Eigen::Vector3d &origin,
                                                  const Eigen::Vector3d &color,
                                                  double voxel_size,
                                                  double width,
                                                  double height,
                                                  double depth) {
    auto output = std::make_shared<VoxelGrid>();
    int num_w = int(std::round(width / voxel_size));
    int num_h = int(std::round(height / voxel_size));
    int num_d = int(std::round(depth / voxel_size));
    output->origin_ = origin;
    output->voxel_size_ = voxel_size;
    for (int widx = 0; widx < num_w; widx++) {
        for (int hidx = 0; hidx < num_h; hidx++) {
            for (int didx = 0; didx < num_d; didx++) {
                Eigen::Vector3i grid_index(widx, hidx, didx);
                output->AddVoxel(geometry::Voxel(grid_index, color));
            }
        }
    }
    return output;
}

std::shared_ptr<VoxelGrid> VoxelGrid::CreateFromPointCloudWithinBounds(const PointCloud &input,
                                                                       double voxel_size,
                                                                       const Eigen::Vector3d &min_bound,
                                                                       const Eigen::Vector3d &max_bound) {
    auto output = std::make_shared<VoxelGrid>();
    if (voxel_size <= 0.0) {
        LOGE("voxel_size <= 0.")
    }

    if (voxel_size * std::numeric_limits<int>::max() < (max_bound - min_bound).maxCoeff()) {
        LOGE("voxel_size is too small.")
    }
    output->voxel_size_ = voxel_size;
    output->origin_ = min_bound;
    std::unordered_map<Eigen::Vector3i, AvgColorVoxel, utility::hash_eigen<Eigen::Vector3i>> voxelindex_to_accpoint;
    Eigen::Vector3d ref_coord;
    Eigen::Vector3i voxel_index;
    bool has_colors = input.HasColors();
    for (int i = 0; i < (int)input.points_.size(); i++) {
        ref_coord = (input.points_[i] - min_bound) / voxel_size;
        voxel_index << int(floor(ref_coord(0))), int(floor(ref_coord(1))), int(floor(ref_coord(2)));
        if (has_colors) {
            voxelindex_to_accpoint[voxel_index].Add(voxel_index, input.colors_[i]);
        } else {
            voxelindex_to_accpoint[voxel_index].Add(voxel_index);
        }
    }
    for (const auto &accpoint : voxelindex_to_accpoint) {
        const Eigen::Vector3i &grid_index = accpoint.second.GetVoxelIndex();
        const Eigen::Vector3d &color = has_colors ? accpoint.second.GetAverageColor() : Eigen::Vector3d(0, 0, 0);
        output->AddVoxel(geometry::Voxel(grid_index, color));
    }
    LOGD("Pointcloud is voxelized from {:d} points to {:d} voxels.", (int)input.points_.size(),
         (int)output->voxels_.size())
    return output;
}

std::shared_ptr<VoxelGrid> VoxelGrid::CreateFromPointCloud(const PointCloud &input, double voxel_size) {
    Eigen::Vector3d voxel_size3(voxel_size, voxel_size, voxel_size);
    Eigen::Vector3d min_bound = input.GetMinBound() - voxel_size3 * 0.5;
    Eigen::Vector3d max_bound = input.GetMaxBound() + voxel_size3 * 0.5;
    return CreateFromPointCloudWithinBounds(input, voxel_size, min_bound, max_bound);
}

std::shared_ptr<VoxelGrid> VoxelGrid::CreateFromTriangleMeshWithinBounds(const TriangleMesh &input,
                                                                         double voxel_size,
                                                                         const Eigen::Vector3d &min_bound,
                                                                         const Eigen::Vector3d &max_bound) {
    auto output = std::make_shared<VoxelGrid>();
    if (voxel_size <= 0.0) {
        LOGE("voxel_size <= 0.")
    }

    if (voxel_size * std::numeric_limits<int>::max() < (max_bound - min_bound).maxCoeff()) {
        LOGE("voxel_size is too small.")
    }
    output->voxel_size_ = voxel_size;
    output->origin_ = min_bound;

    Eigen::Vector3d grid_size = max_bound - min_bound;
    int num_w = int(std::round(grid_size(0) / voxel_size));
    int num_h = int(std::round(grid_size(1) / voxel_size));
    int num_d = int(std::round(grid_size(2) / voxel_size));
    const Eigen::Vector3d box_half_size(voxel_size / 2, voxel_size / 2, voxel_size / 2);
    for (int widx = 0; widx < num_w; widx++) {
        for (int hidx = 0; hidx < num_h; hidx++) {
            for (int didx = 0; didx < num_d; didx++) {
                const Eigen::Vector3d box_center = min_bound + Eigen::Vector3d(widx, hidx, didx) * voxel_size;
                for (const Eigen::Vector3i &tria : input.triangles_) {
                    const Eigen::Vector3d &v0 = input.vertices_[tria(0)];
                    const Eigen::Vector3d &v1 = input.vertices_[tria(1)];
                    const Eigen::Vector3d &v2 = input.vertices_[tria(2)];
                    if (IntersectionTest::TriangleAABB(box_center, box_half_size, v0, v1, v2)) {
                        Eigen::Vector3i grid_index(widx, hidx, didx);
                        output->AddVoxel(geometry::Voxel(grid_index));
                        break;
                    }
                }
            }
        }
    }

    return output;
}

std::shared_ptr<VoxelGrid> VoxelGrid::CreateFromTriangleMesh(const TriangleMesh &input, double voxel_size) {
    Eigen::Vector3d voxel_size3(voxel_size, voxel_size, voxel_size);
    Eigen::Vector3d min_bound = input.GetMinBound() - voxel_size3 * 0.5;
    Eigen::Vector3d max_bound = input.GetMaxBound() + voxel_size3 * 0.5;
    return CreateFromTriangleMeshWithinBounds(input, voxel_size, min_bound, max_bound);
}

}  // namespace vox::geometry
