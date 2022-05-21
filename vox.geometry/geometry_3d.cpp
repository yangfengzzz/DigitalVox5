//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "geometry_3d.h"

#include <numeric>

#include "logging.h"

namespace vox::geometry {

Geometry3D& Geometry3D::Rotate(const Eigen::Matrix3d& R) { return Rotate(R, GetCenter()); }

Eigen::Vector3d Geometry3D::ComputeMinBound(const std::vector<Eigen::Vector3d>& points) {
    if (points.empty()) {
        return Eigen::Vector3d(0.0, 0.0, 0.0);
    }
    return std::accumulate(
            points.begin(), points.end(), points[0],
            [](const Eigen::Vector3d& a, const Eigen::Vector3d& b) { return a.array().min(b.array()).matrix(); });
}

Eigen::Vector3d Geometry3D::ComputeMaxBound(const std::vector<Eigen::Vector3d>& points) {
    if (points.empty()) {
        return Eigen::Vector3d(0.0, 0.0, 0.0);
    }
    return std::accumulate(
            points.begin(), points.end(), points[0],
            [](const Eigen::Vector3d& a, const Eigen::Vector3d& b) { return a.array().max(b.array()).matrix(); });
}
Eigen::Vector3d Geometry3D::ComputeCenter(const std::vector<Eigen::Vector3d>& points) {
    Eigen::Vector3d center(0, 0, 0);
    if (points.empty()) {
        return center;
    }
    center = std::accumulate(points.begin(), points.end(), center);
    center /= double(points.size());
    return center;
}

void Geometry3D::ResizeAndPaintUniformColor(std::vector<Eigen::Vector3d>& colors,
                                            const size_t size,
                                            const Eigen::Vector3d& color) {
    colors.resize(size);
    Eigen::Vector3d clipped_color = color;
    if (color.minCoeff() < 0 || color.maxCoeff() > 1) {
        LOGW("invalid color in PaintUniformColor, clipping to [0, 1]")
        clipped_color = clipped_color.array().max(Eigen::Vector3d(0, 0, 0).array()).matrix();
        clipped_color = clipped_color.array().min(Eigen::Vector3d(1, 1, 1).array()).matrix();
    }
    for (size_t i = 0; i < size; i++) {
        colors[i] = clipped_color;
    }
}

void Geometry3D::TransformPoints(const Eigen::Matrix4d& transformation, std::vector<Eigen::Vector3d>& points) {
    for (auto& point : points) {
        Eigen::Vector4d new_point = transformation * Eigen::Vector4d(point(0), point(1), point(2), 1.0);
        point = new_point.head<3>() / new_point(3);
    }
}

void Geometry3D::TransformNormals(const Eigen::Matrix4d& transformation, std::vector<Eigen::Vector3d>& normals) {
    for (auto& normal : normals) {
        Eigen::Vector4d new_normal = transformation * Eigen::Vector4d(normal(0), normal(1), normal(2), 0.0);
        normal = new_normal.head<3>();
    }
}

void Geometry3D::TransformCovariances(const Eigen::Matrix4d& transformation,
                                      std::vector<Eigen::Matrix3d>& covariances) {
    RotateCovariances(transformation.block<3, 3>(0, 0), covariances);
}

void Geometry3D::TranslatePoints(const Eigen::Vector3d& translation,
                                 std::vector<Eigen::Vector3d>& points,
                                 bool relative) {
    Eigen::Vector3d transform = translation;
    if (!relative) {
        transform -= ComputeCenter(points);
    }
    for (auto& point : points) {
        point += transform;
    }
}

void Geometry3D::ScalePoints(const double scale, std::vector<Eigen::Vector3d>& points, const Eigen::Vector3d& center) {
    for (auto& point : points) {
        point = (point - center) * scale + center;
    }
}

void Geometry3D::RotatePoints(const Eigen::Matrix3d& R,
                              std::vector<Eigen::Vector3d>& points,
                              const Eigen::Vector3d& center) {
    for (auto& point : points) {
        point = R * (point - center) + center;
    }
}

void Geometry3D::RotateNormals(const Eigen::Matrix3d& R, std::vector<Eigen::Vector3d>& normals) {
    for (auto& normal : normals) {
        normal = R * normal;
    }
}

/// The only part that affects the covariance is the rotation part. For more
/// information on variance propagation please visit:
/// https://en.wikipedia.org/wiki/Propagation_of_uncertainty
void Geometry3D::RotateCovariances(const Eigen::Matrix3d& R, std::vector<Eigen::Matrix3d>& covariances) {
    for (auto& covariance : covariances) {
        covariance = R * covariance * R.transpose();
    }
}

Eigen::Matrix3d Geometry3D::GetRotationMatrixFromXYZ(const Eigen::Vector3d& rotation) {
    return vox::utility::RotationMatrixX(rotation(0)) * vox::utility::RotationMatrixY(rotation(1)) *
           vox::utility::RotationMatrixZ(rotation(2));
}

Eigen::Matrix3d Geometry3D::GetRotationMatrixFromYZX(const Eigen::Vector3d& rotation) {
    return vox::utility::RotationMatrixY(rotation(0)) * vox::utility::RotationMatrixZ(rotation(1)) *
           vox::utility::RotationMatrixX(rotation(2));
}

Eigen::Matrix3d Geometry3D::GetRotationMatrixFromZXY(const Eigen::Vector3d& rotation) {
    return vox::utility::RotationMatrixZ(rotation(0)) * vox::utility::RotationMatrixX(rotation(1)) *
           vox::utility::RotationMatrixY(rotation(2));
}

Eigen::Matrix3d Geometry3D::GetRotationMatrixFromXZY(const Eigen::Vector3d& rotation) {
    return vox::utility::RotationMatrixX(rotation(0)) * vox::utility::RotationMatrixZ(rotation(1)) *
           vox::utility::RotationMatrixY(rotation(2));
}

Eigen::Matrix3d Geometry3D::GetRotationMatrixFromZYX(const Eigen::Vector3d& rotation) {
    return vox::utility::RotationMatrixZ(rotation(0)) * vox::utility::RotationMatrixY(rotation(1)) *
           vox::utility::RotationMatrixX(rotation(2));
}

Eigen::Matrix3d Geometry3D::GetRotationMatrixFromYXZ(const Eigen::Vector3d& rotation) {
    return vox::utility::RotationMatrixY(rotation(0)) * vox::utility::RotationMatrixX(rotation(1)) *
           vox::utility::RotationMatrixZ(rotation(2));
}

Eigen::Matrix3d Geometry3D::GetRotationMatrixFromAxisAngle(const Eigen::Vector3d& rotation) {
    const double phi = rotation.norm();
    if (phi > 0) {
        return Eigen::AngleAxisd(phi, rotation / phi).toRotationMatrix();
    }
    return Eigen::Matrix3d::Identity();
}

Eigen::Matrix3d Geometry3D::GetRotationMatrixFromQuaternion(const Eigen::Vector4d& rotation) {
    return Eigen::Quaterniond(rotation(0), rotation(1), rotation(2), rotation(3)).normalized().toRotationMatrix();
}

}  // namespace vox::geometry
