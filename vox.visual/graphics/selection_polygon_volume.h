//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Core>
#include <memory>
#include <string>
#include <vector>

#include "vox.base/ijson_convertible.h"

namespace vox {

namespace geometry {
class Geometry;
class PointCloud;
class TriangleMesh;
}  // namespace geometry

namespace visualization {

/// \class SelectionPolygonVolume
///
/// \brief Select a polygon volume for cropping.
class SelectionPolygonVolume : public utility::IJsonConvertible {
public:
    ~SelectionPolygonVolume() override = default;

public:
    bool ConvertToJsonValue(Json::Value &value) const override;

    bool ConvertFromJsonValue(const Json::Value &value) override;

    /// Function to crop point cloud.
    ///
    /// \param input The input point cloud.
    [[nodiscard]] std::shared_ptr<geometry::PointCloud> CropPointCloud(const geometry::PointCloud &input) const;

    /// Function to crop crop triangle mesh.
    ///
    /// \param input The input triangle mesh.
    [[nodiscard]] std::shared_ptr<geometry::TriangleMesh> CropTriangleMesh(const geometry::TriangleMesh &input) const;

private:
    [[nodiscard]] std::shared_ptr<geometry::PointCloud> CropPointCloudInPolygon(
            const geometry::PointCloud &input) const;

    [[nodiscard]] std::shared_ptr<geometry::TriangleMesh> CropTriangleMeshInPolygon(
            const geometry::TriangleMesh &input) const;

    [[nodiscard]] std::vector<size_t> CropInPolygon(const std::vector<Eigen::Vector3d> &input) const;

public:
    /// One of `{x, y, z}`.
    std::string orthogonal_axis_;
    /// Bounding polygon boundary.
    std::vector<Eigen::Vector3d> bounding_polygon_;
    /// Minimum axis value.
    double axis_min_ = 0.0;
    /// Maximum axis value.
    double axis_max_ = 0.0;
};

}  // namespace visualization
}  // namespace vox
