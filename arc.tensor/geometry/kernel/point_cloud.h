//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_map>

#include "tensor.h"

namespace vox {
namespace t {
namespace geometry {
namespace kernel {
namespace pointcloud {

void Unproject(const core::Tensor& depth,
               std::optional<std::reference_wrapper<const core::Tensor>> image_colors,
               core::Tensor& points,
               std::optional<std::reference_wrapper<core::Tensor>> colors,
               const core::Tensor& intrinsics,
               const core::Tensor& extrinsics,
               float depth_scale,
               float depth_max,
               int64_t stride);

void Project(core::Tensor& depth,
             std::optional<std::reference_wrapper<core::Tensor>> image_colors,
             const core::Tensor& points,
             std::optional<std::reference_wrapper<const core::Tensor>> colors,
             const core::Tensor& intrinsics,
             const core::Tensor& extrinsics,
             float depth_scale,
             float depth_max);

void UnprojectCPU(const core::Tensor& depth,
                  std::optional<std::reference_wrapper<const core::Tensor>> image_colors,
                  core::Tensor& points,
                  std::optional<std::reference_wrapper<core::Tensor>> colors,
                  const core::Tensor& intrinsics,
                  const core::Tensor& extrinsics,
                  float depth_scale,
                  float depth_max,
                  int64_t stride);

void ProjectCPU(core::Tensor& depth,
                std::optional<std::reference_wrapper<core::Tensor>> image_colors,
                const core::Tensor& points,
                std::optional<std::reference_wrapper<const core::Tensor>> colors,
                const core::Tensor& intrinsics,
                const core::Tensor& extrinsics,
                float depth_scale,
                float depth_max);

#ifdef BUILD_CUDA_MODULE
void UnprojectCUDA(const core::Tensor& depth,
                   std::optional<std::reference_wrapper<const core::Tensor>> image_colors,
                   core::Tensor& points,
                   std::optional<std::reference_wrapper<core::Tensor>> colors,
                   const core::Tensor& intrinsics,
                   const core::Tensor& extrinsics,
                   float depth_scale,
                   float depth_max,
                   int64_t stride);

void ProjectCUDA(core::Tensor& depth,
                 std::optional<std::reference_wrapper<core::Tensor>> image_colors,
                 const core::Tensor& points,
                 std::optional<std::reference_wrapper<const core::Tensor>> colors,
                 const core::Tensor& intrinsics,
                 const core::Tensor& extrinsics,
                 float depth_scale,
                 float depth_max);
#endif

void EstimateCovariancesUsingHybridSearchCPU(const core::Tensor& points,
                                             core::Tensor& covariances,
                                             const double& radius,
                                             const int64_t& max_nn);

void EstimateCovariancesUsingKNNSearchCPU(const core::Tensor& points, core::Tensor& covariances, const int64_t& max_nn);

void EstimateNormalsFromCovariancesCPU(const core::Tensor& covariances, core::Tensor& normals, const bool has_normals);

void EstimateColorGradientsUsingHybridSearchCPU(const core::Tensor& points,
                                                const core::Tensor& normals,
                                                const core::Tensor& colors,
                                                core::Tensor& color_gradient,
                                                const double& radius,
                                                const int64_t& max_nn);

void EstimateColorGradientsUsingKNNSearchCPU(const core::Tensor& points,
                                             const core::Tensor& normals,
                                             const core::Tensor& colors,
                                             core::Tensor& color_gradient,
                                             const int64_t& max_nn);

#ifdef BUILD_CUDA_MODULE
void EstimateCovariancesUsingHybridSearchCUDA(const core::Tensor& points,
                                              core::Tensor& covariances,
                                              const double& radius,
                                              const int64_t& max_nn);

void EstimateCovariancesUsingKNNSearchCUDA(const core::Tensor& points,
                                           core::Tensor& covariances,
                                           const int64_t& max_nn);

void EstimateNormalsFromCovariancesCUDA(const core::Tensor& covariances, core::Tensor& normals, const bool has_normals);

void EstimateColorGradientsUsingHybridSearchCUDA(const core::Tensor& points,
                                                 const core::Tensor& normals,
                                                 const core::Tensor& colors,
                                                 core::Tensor& color_gradient,
                                                 const double& radius,
                                                 const int64_t& max_nn);

void EstimateColorGradientsUsingKNNSearchCUDA(const core::Tensor& points,
                                              const core::Tensor& normals,
                                              const core::Tensor& colors,
                                              core::Tensor& color_gradient,
                                              const int64_t& max_nn);
#endif

}  // namespace pointcloud
}  // namespace kernel
}  // namespace geometry
}  // namespace t
}  // namespace vox
