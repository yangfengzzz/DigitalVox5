//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tests.h"
#include "triangle_mesh_io.h"

namespace vox::tests {

TEST(TriangleMeshIO, DISABLED_CreateMeshFromFile) { NotImplemented(); }

TEST(TriangleMeshIO, DISABLED_ReadTriangleMesh) { NotImplemented(); }

TEST(TriangleMeshIO, DISABLED_WriteTriangleMesh) { NotImplemented(); }

TEST(TriangleMeshIO, DISABLED_ReadTriangleMeshFromPLY) { NotImplemented(); }

TEST(TriangleMeshIO, DISABLED_WriteTriangleMeshToPLY) { NotImplemented(); }

TEST(TriangleMeshIO, AddTrianglesByEarClippingNonconvexPoly) {
    // This test checks if a bug in AddTrianglesByEarClipping() is fixed.

    auto poly = std::make_shared<geometry::TriangleMesh>();
    poly->vertices_.resize(5);

    //  1      3
    //  |\    /|
    //  | \2 / |    y
    //  |  \/  |    |
    //  |______|    /-- x
    //  0      4   z
    Eigen::Vector3d v1(0., 0., 0.);
    Eigen::Vector3d v2(0., 2., 0.);
    Eigen::Vector3d v3(1., 1., 0.);
    Eigen::Vector3d v4(2., 2., 0.);
    Eigen::Vector3d v5(2., 0., 0.);
    poly->vertices_[0] = v1;
    poly->vertices_[1] = v2;
    poly->vertices_[2] = v3;
    poly->vertices_[3] = v4;
    poly->vertices_[4] = v5;
    std::vector<unsigned int> indices{0, 1, 2, 3, 4};

    EXPECT_TRUE(io::AddTrianglesByEarClipping(*poly, indices));
}

}  // namespace vox::tests
