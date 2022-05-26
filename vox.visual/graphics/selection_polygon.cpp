//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.visual/graphics/selection_polygon.h"

#include "vox.base/logging.h"
#include "vox.base/progress_bar.h"
#include "vox.geometry/point_cloud.h"
#include "vox.geometry/triangle_mesh.h"
#include "vox.visual/graphics/selection_polygon_volume.h"

namespace vox::visualization {

SelectionPolygon &SelectionPolygon::Clear() {
    polygon_.clear();
    is_closed_ = false;
    polygon_interior_mask_.Clear();
    polygon_type_ = SectionPolygonType::Unfilled;
    return *this;
}

bool SelectionPolygon::IsEmpty() const {
    // A valid polygon, either close or open, should have at least 2 vertices.
    return polygon_.size() <= 1;
}

Eigen::Vector2d SelectionPolygon::GetMinBound() const {
    if (polygon_.empty()) {
        return Eigen::Vector2d(0.0, 0.0);
    }
    auto itr_x = std::min_element(polygon_.begin(), polygon_.end(),
                                  [](const Eigen::Vector2d &a, const Eigen::Vector2d &b) { return a(0) < b(0); });
    auto itr_y = std::min_element(polygon_.begin(), polygon_.end(),
                                  [](const Eigen::Vector2d &a, const Eigen::Vector2d &b) { return a(1) < b(1); });
    return Eigen::Vector2d((*itr_x)(0), (*itr_y)(1));
}

Eigen::Vector2d SelectionPolygon::GetMaxBound() const {
    if (polygon_.empty()) {
        return Eigen::Vector2d(0.0, 0.0);
    }
    auto itr_x = std::max_element(polygon_.begin(), polygon_.end(),
                                  [](const Eigen::Vector2d &a, const Eigen::Vector2d &b) { return a(0) < b(0); });
    auto itr_y = std::max_element(polygon_.begin(), polygon_.end(),
                                  [](const Eigen::Vector2d &a, const Eigen::Vector2d &b) { return a(1) < b(1); });
    return Eigen::Vector2d((*itr_x)(0), (*itr_y)(1));
}

void SelectionPolygon::FillPolygon(int width, int height) {
    // Standard scan conversion code. See reference:
    // http://alienryderflex.com/polygon_fill/
    if (IsEmpty()) return;
    is_closed_ = true;
    polygon_interior_mask_.Prepare(width, height, 1, 1);
    std::fill(polygon_interior_mask_.data_.begin(), polygon_interior_mask_.data_.end(), 0);
    std::vector<int> nodes;
    for (int y = 0; y < height; y++) {
        nodes.clear();
        for (size_t i = 0; i < polygon_.size(); i++) {
            size_t j = (i + 1) % polygon_.size();
            if ((polygon_[i](1) < y && polygon_[j](1) >= y) || (polygon_[j](1) < y && polygon_[i](1) >= y)) {
                nodes.push_back((int)lround(polygon_[i](0) +
                                            (y - polygon_[i](1)) / (polygon_[j](1) - polygon_[i](1)) *
                                                    (polygon_[j](0) - polygon_[i](0)) +
                                            0.5));
            }
        }
        std::sort(nodes.begin(), nodes.end());
        for (size_t i = 0; i < nodes.size(); i += 2) {
            if (nodes[i] >= width) {
                break;
            }
            if (nodes[i + 1] > 0) {
                if (nodes[i] < 0) nodes[i] = 0;
                if (nodes[i + 1] > width) nodes[i + 1] = width;
                for (int x = nodes[i]; x < nodes[i + 1]; x++) {
                    polygon_interior_mask_.data_[x + y * width] = 1;
                }
            }
        }
    }
}

std::shared_ptr<geometry::PointCloud> SelectionPolygon::CropPointCloud(const geometry::PointCloud &input,
                                                                       const Matrix4x4F &model_mat,
                                                                       Camera *camera) {
    if (IsEmpty()) {
        return std::make_shared<geometry::PointCloud>();
    }
    switch (polygon_type_) {
        case SectionPolygonType::Rectangle:
            return CropPointCloudInRectangle(input, model_mat, camera);
        case SectionPolygonType::Polygon:
            return CropPointCloudInPolygon(input, model_mat, camera);
        case SectionPolygonType::Unfilled:
        default:
            return std::shared_ptr<geometry::PointCloud>();
    }
}

std::shared_ptr<geometry::TriangleMesh> SelectionPolygon::CropTriangleMesh(const geometry::TriangleMesh &input,
                                                                           const Matrix4x4F &model_mat,
                                                                           Camera *camera) {
    if (IsEmpty()) {
        return std::make_shared<geometry::TriangleMesh>();
    }
    if (input.HasVertices() && !input.HasTriangles()) {
        LOGW("geometry::TriangleMesh contains vertices, but no triangles; "
             "cropping will always yield an empty "
             "geometry::TriangleMesh.")
        return std::make_shared<geometry::TriangleMesh>();
    }
    switch (polygon_type_) {
        case SectionPolygonType::Rectangle:
            return CropTriangleMeshInRectangle(input, model_mat, camera);
        case SectionPolygonType::Polygon:
            return CropTriangleMeshInPolygon(input, model_mat, camera);
        case SectionPolygonType::Unfilled:
        default:
            return std::shared_ptr<geometry::TriangleMesh>();
    }
}

std::shared_ptr<geometry::PointCloud> SelectionPolygon::CropPointCloudInRectangle(const geometry::PointCloud &input,
                                                                                  const Matrix4x4F &model_mat,
                                                                                  Camera *camera) {
    return input.SelectByIndex(CropInRectangle(input.points_, model_mat, camera));
}

std::shared_ptr<geometry::PointCloud> SelectionPolygon::CropPointCloudInPolygon(const geometry::PointCloud &input,
                                                                                const Matrix4x4F &model_mat,
                                                                                Camera *camera) {
    return input.SelectByIndex(CropInPolygon(input.points_, model_mat, camera));
}

std::shared_ptr<geometry::TriangleMesh> SelectionPolygon::CropTriangleMeshInRectangle(
        const geometry::TriangleMesh &input, const Matrix4x4F &model_mat, Camera *camera) {
    return input.SelectByIndex(CropInRectangle(input.vertices_, model_mat, camera));
}

std::shared_ptr<geometry::TriangleMesh> SelectionPolygon::CropTriangleMeshInPolygon(const geometry::TriangleMesh &input,
                                                                                    const Matrix4x4F &model_mat,
                                                                                    Camera *camera) {
    return input.SelectByIndex(CropInPolygon(input.vertices_, model_mat, camera));
}

std::vector<size_t> SelectionPolygon::CropInRectangle(const std::vector<Eigen::Vector3d> &input,
                                                      const Matrix4x4F &model_mat,
                                                      Camera *camera) const {
    std::vector<size_t> output_index;
    Matrix4x4F mvp_matrix = camera->ProjectionMatrix() * camera->ViewMatrix() * model_mat;
    double half_width = (double)camera->FramebufferWidth() * 0.5;
    double half_height = (double)camera->FramebufferHeight() * 0.5;
    auto min_bound = GetMinBound();
    auto max_bound = GetMaxBound();
    utility::ProgressBar progress_bar((int64_t)input.size(), "Cropping geometry: ");
    for (size_t i = 0; i < input.size(); i++) {
        ++progress_bar;
        const auto &point = input[i].cast<float>();
        Vector4F pos = mvp_matrix * Vector4F(point(0), point(1), point(2), 1.0);
        if (pos.z == 0.0) break;
        pos /= pos.z;
        double x = (pos.x + 1.0) * half_width;
        double y = (pos.y + 1.0) * half_height;
        if (x >= min_bound(0) && x <= max_bound(0) && y >= min_bound(1) && y <= max_bound(1)) {
            output_index.push_back(i);
        }
    }
    return output_index;
}

std::vector<size_t> SelectionPolygon::CropInPolygon(const std::vector<Eigen::Vector3d> &input,
                                                    const Matrix4x4F &model_mat,
                                                    Camera *camera) {
    std::vector<size_t> output_index;
    Matrix4x4F mvp_matrix = camera->ProjectionMatrix() * camera->ViewMatrix() * model_mat;
    double half_width = (double)camera->FramebufferWidth() * 0.5;
    double half_height = (double)camera->FramebufferHeight() * 0.5;
    std::vector<double> nodes;
    utility::ProgressBar progress_bar((int64_t)input.size(), "Cropping geometry: ");
    for (size_t k = 0; k < input.size(); k++) {
        ++progress_bar;
        const auto &point = input[k].cast<float>();
        Vector4F pos = mvp_matrix * Vector4F(point(0), point(1), point(2), 1.0);
        if (pos.z == 0.0) break;
        pos /= pos.z;
        double x = (pos.x + 1.0) * half_width;
        double y = (pos.y + 1.0) * half_height;
        nodes.clear();
        for (size_t i = 0; i < polygon_.size(); i++) {
            size_t j = (i + 1) % polygon_.size();
            if ((polygon_[i](1) < y && polygon_[j](1) >= y) || (polygon_[j](1) < y && polygon_[i](1) >= y)) {
                nodes.push_back(polygon_[i](0) + (y - polygon_[i](1)) / (polygon_[j](1) - polygon_[i](1)) *
                                                         (polygon_[j](0) - polygon_[i](0)));
            }
        }
        std::sort(nodes.begin(), nodes.end());
        auto loc = std::lower_bound(nodes.begin(), nodes.end(), x);
        if (std::distance(nodes.begin(), loc) % 2 == 1) {
            output_index.push_back(k);
        }
    }
    return output_index;
}

}  // namespace vox::visualization
