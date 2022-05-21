//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
// @author Ignacio Vizzo     [ivizzo@uni-bonn.de]
//
// Copyright (c) 2020 Ignacio Vizzo, Cyrill Stachniss, University of Bonn.
// ----------------------------------------------------------------------------

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <cmath>
#include <memory>
#include <vector>

#include "eigen.h"
#include "kdtree_flann.h"
#include "key_point.h"
#include "logging.h"
#include "point_cloud.h"

namespace vox {

namespace {

bool IsLocalMaxima(int query_idx, const std::vector<int>& indices, const std::vector<double>& third_eigen_values) {
    for (const auto& idx : indices) {
        if (third_eigen_values[query_idx] < third_eigen_values[idx]) {
            return false;
        }
    }
    return true;
}

double ComputeModelResolution(const std::vector<Eigen::Vector3d>& points, const geometry::KDTreeFlann& kdtree) {
    std::vector<int> indices(2);
    std::vector<double> distances(2);
    double resolution = 0.0;

    for (const auto& point : points) {
        if (kdtree.SearchKNN(point, 2, indices, distances) != 0) {
            resolution += std::sqrt(distances[1]);
        }
    }
    resolution /= points.size();
    return resolution;
}

}  // namespace

namespace geometry::keypoint {
std::shared_ptr<PointCloud> ComputeISSKeypoints(const PointCloud& input,
                                                double salient_radius /* = 0.0 */,
                                                double non_max_radius /* = 0.0 */,
                                                double gamma_21 /* = 0.975 */,
                                                double gamma_32 /* = 0.975 */,
                                                int min_neighbors /*= 5 */) {
    if (input.points_.empty()) {
        LOGW("[ComputeISSKeypoints] Input PointCloud is empty!")
        return std::make_shared<PointCloud>();
    }
    const auto& points = input.points_;
    KDTreeFlann kdtree(input);

    if (salient_radius == 0.0 || non_max_radius == 0.0) {
        const double resolution = ComputeModelResolution(points, kdtree);
        salient_radius = 6 * resolution;
        non_max_radius = 4 * resolution;
        LOGD("[ComputeISSKeypoints] Computed salient_radius = {}, "
             "non_max_radius = {} from input model",
             salient_radius, non_max_radius)
    }

    std::vector<double> third_eigen_values(points.size());
#pragma omp parallel for schedule(static) shared(third_eigen_values)
    for (int i = 0; i < (int)points.size(); i++) {
        std::vector<int> indices;
        std::vector<double> dist;
        int nb_neighbors = kdtree.SearchRadius(points[i], salient_radius, indices, dist);
        if (nb_neighbors < min_neighbors) {
            continue;
        }

        Eigen::Matrix3d cov = utility::ComputeCovariance(points, indices);
        if (cov.isZero()) {
            continue;
        }

        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov);
        const double& e1c = solver.eigenvalues()[2];
        const double& e2c = solver.eigenvalues()[1];
        const double& e3c = solver.eigenvalues()[0];

        if ((e2c / e1c) < gamma_21 && e3c / e2c < gamma_32) {
            third_eigen_values[i] = e3c;
        }
    }

    std::vector<size_t> kp_indices;
    kp_indices.reserve(points.size());
#pragma omp parallel for schedule(static) shared(kp_indices)
    for (int i = 0; i < (int)points.size(); i++) {
        if (third_eigen_values[i] > 0.0) {
            std::vector<int> nn_indices;
            std::vector<double> dist;
            int nb_neighbors = kdtree.SearchRadius(points[i], non_max_radius, nn_indices, dist);

            if (nb_neighbors >= min_neighbors && IsLocalMaxima(i, nn_indices, third_eigen_values)) {
                kp_indices.emplace_back(i);
            }
        }
    }

    LOGD("[ComputeISSKeypoints] Extracted {} keypoints", kp_indices.size())
    return input.SelectByIndex(kp_indices);
}

}  // namespace geometry::keypoint
}  // namespace vox
