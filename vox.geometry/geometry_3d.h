//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>

#include "eigen.h"
#include "geometry.h"

namespace vox::geometry {

class AxisAlignedBoundingBox;
class OrientedBoundingBox;

/// \class Geometry3D
///
/// \brief The base geometry class for 3D geometries.
///
/// Main class for 3D geometries, Derives all data from Geometry Base class.
class Geometry3D : public Geometry {
public:
    ~Geometry3D() override = default;

protected:
    /// \brief Parameterized Constructor.
    ///
    /// \param type type of object based on GeometryType.
    explicit Geometry3D(GeometryType type) : Geometry(type, 3) {}

public:
    Geometry3D& Clear() override = 0;

    [[nodiscard]] bool IsEmpty() const override = 0;

    /// Returns min bounds for geometry coordinates.
    [[nodiscard]] virtual Eigen::Vector3d GetMinBound() const = 0;

    /// Returns max bounds for geometry coordinates.
    [[nodiscard]] virtual Eigen::Vector3d GetMaxBound() const = 0;

    /// Returns the center of the geometry coordinates.
    [[nodiscard]] virtual Eigen::Vector3d GetCenter() const = 0;

    /// Returns an axis-aligned bounding box of the geometry.
    [[nodiscard]] virtual AxisAlignedBoundingBox GetAxisAlignedBoundingBox() const = 0;

    /// Computes the oriented bounding box based on the PCA of the convex hull.
    /// The returned bounding box is an approximation to the minimal bounding
    /// box.
    /// \param robust If set to true uses a more robust method which works
    ///               in degenerate cases but introduces noise to the points
    ///               coordinates.
    [[nodiscard]] virtual OrientedBoundingBox GetOrientedBoundingBox(bool robust = false) const = 0;

    /// \brief Apply transformation (4x4 matrix) to the geometry coordinates.
    virtual Geometry3D& Transform(const Eigen::Matrix4d& transformation) = 0;

    /// \brief Apply translation to the geometry coordinates.
    ///
    /// \param translation A 3D vector to transform the geometry.
    /// \param relative If `true`, the \p translation is directly applied to the
    /// geometry. Otherwise, the geometry center is moved to the \p translation.
    virtual Geometry3D& Translate(const Eigen::Vector3d& translation, bool relative = true) = 0;

    /// \brief Apply scaling to the geometry coordinates.
    /// Given a scaling factor \f$s\f$, and center \f$c\f$, a given point
    /// \f$p\f$ is transformed according to \f$s (p - c) + c\f$.
    ///
    /// \param scale The scale parameter that is multiplied to the
    /// points/vertices of the geometry.
    /// \param center Scale center that is used to resize the geometry.
    virtual Geometry3D& Scale(double scale, const Eigen::Vector3d& center) = 0;

    /// \brief Apply rotation to the geometry coordinates and normals.
    /// Given a rotation matrix \f$R\f$, and center \f$c\f$, a given point
    /// \f$p\f$ is transformed according to \f$R (p - c) + c\f$.
    ///
    /// \param R A 3x3 rotation matrix
    /// \param center Rotation center that is used for the rotation.
    virtual Geometry3D& Rotate(const Eigen::Matrix3d& R, const Eigen::Vector3d& center) = 0;

    virtual Geometry3D& Rotate(const Eigen::Matrix3d& R);

    /// Get Rotation Matrix from XYZ RotationType.
    static Eigen::Matrix3d GetRotationMatrixFromXYZ(const Eigen::Vector3d& rotation);

    /// Get Rotation Matrix from YZX RotationType.
    static Eigen::Matrix3d GetRotationMatrixFromYZX(const Eigen::Vector3d& rotation);

    /// Get Rotation Matrix from ZXY RotationType.
    static Eigen::Matrix3d GetRotationMatrixFromZXY(const Eigen::Vector3d& rotation);

    /// Get Rotation Matrix from XZY RotationType.
    static Eigen::Matrix3d GetRotationMatrixFromXZY(const Eigen::Vector3d& rotation);

    /// Get Rotation Matrix from ZYX RotationType.
    static Eigen::Matrix3d GetRotationMatrixFromZYX(const Eigen::Vector3d& rotation);

    /// Get Rotation Matrix from YXZ RotationType.
    static Eigen::Matrix3d GetRotationMatrixFromYXZ(const Eigen::Vector3d& rotation);

    /// Get Rotation Matrix from AxisAngle RotationType.
    static Eigen::Matrix3d GetRotationMatrixFromAxisAngle(const Eigen::Vector3d& rotation);

    /// Get Rotation Matrix from Quaternion.
    static Eigen::Matrix3d GetRotationMatrixFromQuaternion(const Eigen::Vector4d& rotation);

protected:
    /// Compute min bound of a list points.
    [[nodiscard]] static Eigen::Vector3d ComputeMinBound(const std::vector<Eigen::Vector3d>& points);

    /// Compute max bound of a list points.
    [[nodiscard]] static Eigen::Vector3d ComputeMaxBound(const std::vector<Eigen::Vector3d>& points);

    /// Computer center of a list of points.
    [[nodiscard]] static Eigen::Vector3d ComputeCenter(const std::vector<Eigen::Vector3d>& points);

    /// \brief Resizes the colors vector and paints a uniform color.
    ///
    /// \param colors An array of eigen vectors specifies colors in RGB.
    /// \param size The resultant size of the colors array.
    /// \param color The final color in which the colors will be painted.
    static void ResizeAndPaintUniformColor(std::vector<Eigen::Vector3d>& colors,
                                           size_t size,
                                           const Eigen::Vector3d& color);

    /// \brief Transforms all points with the transformation matrix.
    ///
    /// \param transformation 4x4 matrix for transformation.
    /// \param points A list of points to be transformed.
    static void TransformPoints(const Eigen::Matrix4d& transformation, std::vector<Eigen::Vector3d>& points);

    /// \brief Transforms the normals with the transformation matrix.
    ///
    /// \param transformation 4x4 matrix for transformation.
    /// \param normals A list of normals to be transformed.
    static void TransformNormals(const Eigen::Matrix4d& transformation, std::vector<Eigen::Vector3d>& normals);

    /// \brief Transforms all covariance matrices with the transformation.
    ///
    /// \param transformation 4x4 matrix for transformation.
    /// \param covariances A list of covariance matrices to be transformed.
    static void TransformCovariances(const Eigen::Matrix4d& transformation, std::vector<Eigen::Matrix3d>& covariances);

    /// \brief Apply translation to the geometry coordinates.
    ///
    /// \param translation A 3D vector to transform the geometry.
    /// \param points A list of points to be transformed.
    /// \param relative If `true`, the \p translation is directly applied to the
    /// \p points. Otherwise, the center of the \p points is moved to the \p
    /// translation.
    static void TranslatePoints(const Eigen::Vector3d& translation,
                                std::vector<Eigen::Vector3d>& points,
                                bool relative);

    /// \brief Scale the coordinates of all points by the scaling factor \p
    /// scale.
    ///
    /// \param scale The scale factor that is used to resize the geometry
    /// \param points A list of points to be transformed
    /// \param center Scale center that is used to resize the geometry..
    static void ScalePoints(double scale, std::vector<Eigen::Vector3d>& points, const Eigen::Vector3d& center);

    /// \brief Rotate all points with the rotation matrix \p R.
    ///
    /// \param R A 3x3 rotation matrix
    /// defines the axis of rotation and the norm the angle around this axis.
    /// \param points A list of points to be transformed.
    /// \param center Rotation center that is used for the rotation.
    static void RotatePoints(const Eigen::Matrix3d& R,
                             std::vector<Eigen::Vector3d>& points,
                             const Eigen::Vector3d& center);

    /// \brief Rotate all normals with the rotation matrix \p R.
    ///
    /// \param R A 3x3 rotation matrix
    /// \param normals A list of normals to be transformed.
    static void RotateNormals(const Eigen::Matrix3d& R, std::vector<Eigen::Vector3d>& normals);

    /// \brief Rotate all covariance matrices with the rotation matrix \p R.
    ///
    /// \param R A 3x3 rotation matrix
    /// \param covariances A list of covariance matrices to be transformed.
    static void RotateCovariances(const Eigen::Matrix3d& R, std::vector<Eigen::Matrix3d>& covariances);
};

}  // namespace vox::geometry
