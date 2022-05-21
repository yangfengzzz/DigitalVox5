//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sort.h"

#include <algorithm>

#include "logging.h"

namespace vox::tests {

TEST(Sort, Sort) {
    std::vector<Eigen::Vector3d> points{
            {3, 3, 3},
            {1, 1, 1},
            {0, 0, 0},
            {2, 2, 2},
    };
    std::vector<Eigen::Vector3d> sorted_points{
            {0, 0, 0},
            {1, 1, 1},
            {2, 2, 2},
            {3, 3, 3},
    };
    ExpectEQ(Sort(points), sorted_points);
}

TEST(Sort, SortWithIndices) {
    std::vector<Eigen::Vector3d> points{
            {3, 3, 3},
            {1, 1, 1},
            {0, 0, 0},
            {2, 2, 2},
    };
    std::vector<Eigen::Vector3d> sorted_points{
            {0, 0, 0},
            {1, 1, 1},
            {2, 2, 2},
            {3, 3, 3},
    };
    std::vector<size_t> indices{2, 1, 3, 0};
    ExpectEQ(SortWithIndices(points).first, sorted_points);
    EXPECT_EQ(SortWithIndices(points).second, indices);
}

TEST(Sort, GetIndicesAToB) {
    std::vector<Eigen::Vector3d> a{
            {3, 3, 3},
            {1, 1, 1},
            {0, 0, 0},
            {2, 2, 2},
    };
    std::vector<Eigen::Vector3d> b{
            {2, 2, 2},
            {0, 0, 0},
            {1, 1, 1},
            {3, 3, 3},
    };
    ExpectEQ(ApplyIndices(a, GetIndicesAToB(a, a)), a);
    ExpectEQ(ApplyIndices(b, GetIndicesAToB(b, b)), b);
    ExpectEQ(ApplyIndices(a, GetIndicesAToB(a, b)), b);
    ExpectEQ(ApplyIndices(b, GetIndicesAToB(b, a)), a);
}

TEST(Sort, GetIndicesAToBClose) {
    std::vector<Eigen::Vector3d> a{
            {3, 3, 3},
            {1, 1, 1},
            {4, 4, 4},
            {2, 2, 2},
    };
    std::vector<Eigen::Vector3d> b{
            {2.00001, 2.00001, 2},
            {4, 4.00001, 4},
            {1.00001, 1, 1.00001},
            {3, 3, 3.00001},
    };
    double threshold = 0.001;
    ExpectEQ(ApplyIndices(a, GetIndicesAToB(a, a, threshold)), a, threshold);
    ExpectEQ(ApplyIndices(b, GetIndicesAToB(b, b, threshold)), b, threshold);
    ExpectEQ(ApplyIndices(a, GetIndicesAToB(a, b, threshold)), b, threshold);
    ExpectEQ(ApplyIndices(b, GetIndicesAToB(b, a, threshold)), a, threshold);
}

}  // namespace vox::tests
