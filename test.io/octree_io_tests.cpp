// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include <json/json.h>

#include "dataset.h"
#include "file_system.h"
#include "ijson_convertible.h"
#include "octree.h"
#include "octree_io.h"
#include "point_cloud.h"
#include "point_cloud_io.h"
#include "tests.h"

namespace arc::tests {

void WriteReadAndAssertEqual(const geometry::Octree& src_octree) {
    // Write to file
    std::string file_name = utility::filesystem::GetTempDirectoryPath() + "/temp_octree.json";
    EXPECT_TRUE(io::WriteOctree(file_name, src_octree));

    // Read from file
    geometry::Octree dst_octree;
    EXPECT_TRUE(io::ReadOctree(file_name, dst_octree));
    EXPECT_TRUE(src_octree == dst_octree);
}

TEST(OctreeIO, EmptyTree) {
    geometry::Octree octree(10);
    ExpectEQ(octree.origin_, Eigen::Vector3d(0, 0, 0));
    EXPECT_EQ(octree.size_, 0);

    WriteReadAndAssertEqual(octree);
}

TEST(OctreeIO, ZeroDepth) {
    geometry::Octree octree(0, Eigen::Vector3d(-1, -1, -1), 2);
    Eigen::Vector3d point(0, 0, 0);
    Eigen::Vector3d color(0, 0.1, 0.2);
    octree.InsertPoint(point, geometry::OctreeColorLeafNode::GetInitFunction(),
                       geometry::OctreeColorLeafNode::GetUpdateFunction(color));

    WriteReadAndAssertEqual(octree);
}

TEST(OctreeIO, JsonFileIOFragment) {
    // Create octree
    geometry::PointCloud pcd;
    data::PLYPointCloud pointcloud_ply;
    io::ReadPointCloud(pointcloud_ply.GetPath(), pcd);
    size_t max_depth = 6;
    geometry::Octree octree(max_depth);
    octree.ConvertFromPointCloud(pcd, 0.01);

    WriteReadAndAssertEqual(octree);
}

TEST(OctreeIO, JsonFileIOSevenCubes) {
    // Build octree
    std::vector<Eigen::Vector3d> points{Eigen::Vector3d(0.5, 0.5, 0.5), Eigen::Vector3d(1.5, 0.5, 0.5),
                                        Eigen::Vector3d(0.5, 1.5, 0.5), Eigen::Vector3d(1.5, 1.5, 0.5),
                                        Eigen::Vector3d(0.5, 0.5, 1.5), Eigen::Vector3d(1.5, 0.5, 1.5),
                                        Eigen::Vector3d(0.5, 1.5, 1.5)};
    std::vector<Eigen::Vector3d> colors{Eigen::Vector3d(0.0, 0.0, 0.0),  Eigen::Vector3d(0.25, 0.0, 0.0),
                                        Eigen::Vector3d(0.0, 0.25, 0.0), Eigen::Vector3d(0.25, 0.25, 0.0),
                                        Eigen::Vector3d(0.0, 0.0, 0.25), Eigen::Vector3d(0.25, 0.0, 0.25),
                                        Eigen::Vector3d(0.0, 0.25, 0.25)};
    geometry::Octree octree(1, Eigen::Vector3d(0, 0, 0), 2);
    for (size_t i = 0; i < points.size(); ++i) {
        octree.InsertPoint(points[i], geometry::OctreeColorLeafNode::GetInitFunction(),
                           geometry::OctreeColorLeafNode::GetUpdateFunction(colors[i]));
    }

    WriteReadAndAssertEqual(octree);
}

}  // namespace arc
