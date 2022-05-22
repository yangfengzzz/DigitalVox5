//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.geometry/line_set.h"
#include "vox.geometry/triangle_mesh.h"
#include "vox.geometry/voxel_grid.h"
//#include "utility/DrawGeometry.h"
#include "tests.h"

using namespace vox;
using namespace tests;

TEST(VoxelGrid, Bounds) {
    auto voxel_grid = std::make_shared<geometry::VoxelGrid>();
    voxel_grid->origin_ = Eigen::Vector3d(0, 0, 0);
    voxel_grid->voxel_size_ = 5;
    voxel_grid->AddVoxel(geometry::Voxel(Eigen::Vector3i(1, 0, 0)));
    voxel_grid->AddVoxel(geometry::Voxel(Eigen::Vector3i(0, 2, 0)));
    voxel_grid->AddVoxel(geometry::Voxel(Eigen::Vector3i(0, 0, 3)));
    ExpectEQ(voxel_grid->GetMinBound(), Eigen::Vector3d(0, 0, 0));
    ExpectEQ(voxel_grid->GetMaxBound(), Eigen::Vector3d(10, 15, 20));
}

TEST(VoxelGrid, GetVoxel) {
    auto voxel_grid = std::make_shared<geometry::VoxelGrid>();
    voxel_grid->origin_ = Eigen::Vector3d(0, 0, 0);
    voxel_grid->voxel_size_ = 5;
    ExpectEQ(voxel_grid->GetVoxel(Eigen::Vector3d(0, 0, 0)), Eigen::Vector3i(0, 0, 0));
    ExpectEQ(voxel_grid->GetVoxel(Eigen::Vector3d(0, 1, 0)), Eigen::Vector3i(0, 0, 0));
    // Test near boundary voxel_size_ == 5
    ExpectEQ(voxel_grid->GetVoxel(Eigen::Vector3d(0, 4.9, 0)), Eigen::Vector3i(0, 0, 0));
    ExpectEQ(voxel_grid->GetVoxel(Eigen::Vector3d(0, 5, 0)), Eigen::Vector3i(0, 1, 0));
    ExpectEQ(voxel_grid->GetVoxel(Eigen::Vector3d(0, 5.1, 0)), Eigen::Vector3i(0, 1, 0));
}

TEST(VoxelGrid, Visualization) {
    auto voxel_grid = std::make_shared<geometry::VoxelGrid>();
    voxel_grid->origin_ = Eigen::Vector3d(0, 0, 0);
    voxel_grid->voxel_size_ = 5;
    voxel_grid->AddVoxel(geometry::Voxel(Eigen::Vector3i(0, 0, 0), Eigen::Vector3d(0.9, 0, 0)));
    voxel_grid->AddVoxel(geometry::Voxel(Eigen::Vector3i(0, 1, 0), Eigen::Vector3d(0.9, 0.9, 0)));

    // Uncomment the line below for visualization test
    // visualization::DrawGeometries({voxel_grid});
}
