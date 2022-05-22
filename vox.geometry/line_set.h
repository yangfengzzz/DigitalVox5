//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Core>
#include <memory>
#include <utility>
#include <vector>

#include "vox.geometry/geometry_3d.h"

namespace vox::geometry {

class PointCloud;
class OrientedBoundingBox;
class AxisAlignedBoundingBox;
class TriangleMesh;
class TetraMesh;

/// \class LineSet
///
/// \brief LineSet define a sets of lines in 3D. A typical application is to
/// display the point cloud correspondence pairs.
class LineSet : public Geometry3D {
public:
    /// \brief Default Constructor.
    LineSet() : Geometry3D(Geometry::GeometryType::LINE_SET) {}
    /// \brief Parameterized Constructor.
    ///
    ///  Create a LineSet from given points and line indices
    ///
    /// \param points Point coordinates.
    /// \param lines Lines denoted by the index of points forming the line.
    LineSet(std::vector<Eigen::Vector3d> points, std::vector<Eigen::Vector2i> lines)
        : Geometry3D(Geometry::GeometryType::LINE_SET), points_(std::move(points)), lines_(std::move(lines)) {}
    ~LineSet() override = default;

public:
    LineSet &Clear() override;
    [[nodiscard]] bool IsEmpty() const override;
    [[nodiscard]] Eigen::Vector3d GetMinBound() const override;
    [[nodiscard]] Eigen::Vector3d GetMaxBound() const override;
    [[nodiscard]] Eigen::Vector3d GetCenter() const override;
    [[nodiscard]] AxisAlignedBoundingBox GetAxisAlignedBoundingBox() const override;
    [[nodiscard]] OrientedBoundingBox GetOrientedBoundingBox(bool robust = false) const override;
    LineSet &Transform(const Eigen::Matrix4d &transformation) override;
    LineSet &Translate(const Eigen::Vector3d &translation, bool relative = true) override;
    LineSet &Scale(double scale, const Eigen::Vector3d &center) override;
    LineSet &Rotate(const Eigen::Matrix3d &R, const Eigen::Vector3d &center) override;

    LineSet &operator+=(const LineSet &lineset);
    LineSet operator+(const LineSet &lineset) const;

    /// Returns `true` if the object contains points.
    [[nodiscard]] bool HasPoints() const { return !points_.empty(); }

    /// Returns `true` if the object contains lines.
    [[nodiscard]] bool HasLines() const { return HasPoints() && !lines_.empty(); }

    /// Returns `true` if the objects lines contains colors.
    [[nodiscard]] bool HasColors() const { return HasLines() && colors_.size() == lines_.size(); }

    /// \brief Returns the coordinates of the line at the given index.
    ///
    /// \param line_index Index of the line.
    [[nodiscard]] std::pair<Eigen::Vector3d, Eigen::Vector3d> GetLineCoordinate(size_t line_index) const {
        return std::make_pair(points_[lines_[line_index][0]], points_[lines_[line_index][1]]);
    }

    /// \brief Assigns each line in the LineSet the same color.
    ///
    /// \param color Specifies the color to be applied.
    LineSet &PaintUniformColor(const Eigen::Vector3d &color) {
        ResizeAndPaintUniformColor(colors_, lines_.size(), color);
        return *this;
    }

    /// \brief Factory function to create a LineSet from two PointClouds
    /// (\p cloud0, \p cloud1) and a correspondence set.
    ///
    /// \param cloud0 First point cloud.
    /// \param cloud1 Second point cloud.
    /// \param correspondences Set of correspondences.
    static std::shared_ptr<LineSet> CreateFromPointCloudCorrespondences(
            const PointCloud &cloud0,
            const PointCloud &cloud1,
            const std::vector<std::pair<int, int>> &correspondences);

    /// \brief Factory function to create a LineSet from an OrientedBoundingBox.
    ///
    /// \param box The input bounding box.
    static std::shared_ptr<LineSet> CreateFromOrientedBoundingBox(const OrientedBoundingBox &box);

    /// \brief Factory function to create a LineSet from an
    /// AxisAlignedBoundingBox.
    ///
    /// \param box The input bounding box.
    static std::shared_ptr<LineSet> CreateFromAxisAlignedBoundingBox(const AxisAlignedBoundingBox &box);

    /// Factory function to create a LineSet from edges of a triangle mesh.
    ///
    /// \param mesh The input triangle mesh.
    static std::shared_ptr<LineSet> CreateFromTriangleMesh(const TriangleMesh &mesh);

    /// Factory function to create a LineSet from edges of a tetra mesh.
    ///
    /// \param mesh The input tetra mesh.
    static std::shared_ptr<LineSet> CreateFromTetraMesh(const TetraMesh &mesh);

    /// Factory function to create a LineSet from intrinsic and extrinsic
    /// matrices.
    ///
    /// \param view_width_px The width of the view, in pixels
    /// \param view_height_px The height of the view, in pixels
    /// \param intrinsic The intrinsic matrix
    /// \param extrinsic The extrinsic matrix
    static std::shared_ptr<LineSet> CreateCameraVisualization(int view_width_px,
                                                              int view_height_px,
                                                              const Eigen::Matrix3d &intrinsic,
                                                              const Eigen::Matrix4d &extrinsic,
                                                              double scale = 1.0);

public:
    /// Points coordinates.
    std::vector<Eigen::Vector3d> points_;
    /// Lines denoted by the index of points forming the line.
    std::vector<Eigen::Vector2i> lines_;
    /// RGB colors of lines.
    std::vector<Eigen::Vector3d> colors_;
};

}  // namespace vox::geometry
