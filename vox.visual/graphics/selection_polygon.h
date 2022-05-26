//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Core>
#include <memory>
#include <vector>

#include "vox.geometry/geometry_2d.h"
#include "vox.geometry/image.h"
#include "vox.render/camera.h"

namespace vox {

namespace geometry {
class PointCloud;
class TriangleMesh;
}  // namespace geometry

namespace visualization {
class SelectionPolygonVolume;

/// A 2D polygon used for selection on screen
/// It is a utility class for Visualization
/// The coordinates in SelectionPolygon are lower-left corner based (the OpenGL
/// convention).
class SelectionPolygon : public geometry::Geometry2D {
public:
    enum class SectionPolygonType {
        Unfilled = 0,
        Rectangle = 1,
        Polygon = 2,
    };

    std::vector<Eigen::Vector2d> polygon_;
    bool is_closed_ = false;
    geometry::Image polygon_interior_mask_;
    SectionPolygonType polygon_type_ = SectionPolygonType::Unfilled;

public:
    SelectionPolygon() : geometry::Geometry2D(geometry::Geometry::GeometryType::UNSPECIFIED) {}

    ~SelectionPolygon() override = default;

public:
    SelectionPolygon &Clear() override;

    [[nodiscard]] bool IsEmpty() const override;

    [[nodiscard]] Eigen::Vector2d GetMinBound() const final;

    [[nodiscard]] Eigen::Vector2d GetMaxBound() const final;

    void FillPolygon(int width, int height);

    std::shared_ptr<geometry::PointCloud> CropPointCloud(const geometry::PointCloud &input,
                                                         const Matrix4x4F &model_mat,
                                                         Camera *camera);

    std::shared_ptr<geometry::TriangleMesh> CropTriangleMesh(const geometry::TriangleMesh &input,
                                                             const Matrix4x4F &model_mat,
                                                             Camera *camera);

private:
    std::shared_ptr<geometry::PointCloud> CropPointCloudInRectangle(const geometry::PointCloud &input,
                                                                    const Matrix4x4F &model_mat,
                                                                    Camera *camera);

    std::shared_ptr<geometry::PointCloud> CropPointCloudInPolygon(const geometry::PointCloud &input,
                                                                  const Matrix4x4F &model_mat,
                                                                  Camera *camera);

    std::shared_ptr<geometry::TriangleMesh> CropTriangleMeshInRectangle(const geometry::TriangleMesh &input,
                                                                        const Matrix4x4F &model_mat,
                                                                        Camera *camera);

    std::shared_ptr<geometry::TriangleMesh> CropTriangleMeshInPolygon(const geometry::TriangleMesh &input,
                                                                      const Matrix4x4F &model_mat,
                                                                      Camera *camera);

    std::vector<size_t> CropInRectangle(const std::vector<Eigen::Vector3d> &input,
                                        const Matrix4x4F &model_mat,
                                        Camera *camera) const;

    std::vector<size_t> CropInPolygon(const std::vector<Eigen::Vector3d> &input,
                                      const Matrix4x4F &model_mat,
                                      Camera *camera);
};

}  // namespace visualization
}  // namespace vox
