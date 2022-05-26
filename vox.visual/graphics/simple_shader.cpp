//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simple_shader.h"

#include "vox.base/logging.h"
#include "vox.visual/graphics/color_map.h"

namespace vox::visualization {
bool SimpleShader::BindLineSet(const geometry::LineSet &lineset, const RenderOption &option) {
    if (!lineset.HasLines()) {
        LOGW("Binding failed with empty geometry::LineSet.")
        return false;
    }
    points_.resize(lineset.lines_.size() * 2);
    colors_.resize(lineset.lines_.size() * 2);
    for (size_t i = 0; i < lineset.lines_.size(); i++) {
        const auto kPointPair = lineset.GetLineCoordinate(i);
        auto float_point_first = kPointPair.first.cast<float>();
        points_[i * 2] = Vector3F(float_point_first.x(), float_point_first.y(), float_point_first.z());
        auto float_point_second = kPointPair.second.cast<float>();
        points_[i * 2 + 1] = Vector3F(float_point_second.x(), float_point_second.y(), float_point_second.z());
        Eigen::Vector3d color;
        if (lineset.HasColors()) {
            color = lineset.colors_[i];
        } else {
            color = Eigen::Vector3d::Zero();
        }
        auto float_color = color.cast<float>();
        colors_[i * 2] = colors_[i * 2 + 1] = Color(float_color.x(), float_color.y(), float_color.z());
    }
    return true;
}

bool SimpleShader::BindOrientedBoundingBox(const geometry::OrientedBoundingBox &bound, const RenderOption &option) {
    auto lineset = geometry::LineSet::CreateFromOrientedBoundingBox(bound);
    points_.resize(lineset->lines_.size() * 2);
    colors_.resize(lineset->lines_.size() * 2);
    for (size_t i = 0; i < lineset->lines_.size(); i++) {
        const auto kPointPair = lineset->GetLineCoordinate(i);
        auto point_first = kPointPair.first.cast<float>();
        points_[i * 2] = Vector3F(point_first.x(), point_first.y(), point_first.z());
        auto point_second = kPointPair.second.cast<float>();
        points_[i * 2 + 1] = Vector3F(point_second.x(), point_second.y(), point_second.z());
        Eigen::Vector3d color;
        if (lineset->HasColors()) {
            color = lineset->colors_[i];
        } else {
            color = Eigen::Vector3d::Zero();
        }
        auto float_color = color.cast<float>();
        colors_[i * 2] = colors_[i * 2 + 1] = Color(float_color.x(), float_color.y(), float_color.z());
    }
    return true;
}

bool SimpleShader::BindAxisAlignedBoundingBox(const geometry::AxisAlignedBoundingBox &bound,
                                              const RenderOption &option) {
    auto lineset = geometry::LineSet::CreateFromAxisAlignedBoundingBox(bound);
    points_.resize(lineset->lines_.size() * 2);
    colors_.resize(lineset->lines_.size() * 2);
    for (size_t i = 0; i < lineset->lines_.size(); i++) {
        const auto kPointPair = lineset->GetLineCoordinate(i);
        auto point_first = kPointPair.first.cast<float>();
        points_[i * 2] = Vector3F(point_first.x(), point_first.y(), point_first.z());
        auto point_second = kPointPair.second.cast<float>();
        points_[i * 2 + 1] = Vector3F(point_second.x(), point_second.y(), point_second.z());
        Eigen::Vector3d color;
        if (lineset->HasColors()) {
            color = lineset->colors_[i];
        } else {
            color = Eigen::Vector3d::Zero();
        }
        auto float_color = color.cast<float>();
        colors_[i * 2] = colors_[i * 2 + 1] = Color(float_color.x(), float_color.y(), float_color.z());
    }
    return true;
}

bool SimpleShader::BindTriangleMesh(const geometry::TriangleMesh &mesh, const RenderOption &option) {
    if (!mesh.HasTriangles()) {
        LOGW("Binding failed with empty triangle mesh.")
        return false;
    }
    const ColorMap &global_color_map = *GetGlobalColorMap();
    points_.resize(mesh.triangles_.size() * 3);
    colors_.resize(mesh.triangles_.size() * 3);

    for (size_t i = 0; i < mesh.triangles_.size(); i++) {
        const auto &triangle = mesh.triangles_[i];
        for (size_t j = 0; j < 3; j++) {
            size_t idx = i * 3 + j;
            size_t vi = triangle(j);
            const auto &vertex = mesh.vertices_[vi].cast<float>();
            points_[idx] = Vector3F(vertex.x(), vertex.y(), vertex.z());

            Color color;
            switch (option.mesh_color_option_) {
                case RenderOption::MeshColorOption::XCoordinate:
                    color = global_color_map.GetColor(
                            (float)mesh.GetAxisAlignedBoundingBox().GetXPercentage(vertex(0)));
                    break;
                case RenderOption::MeshColorOption::YCoordinate:
                    color = global_color_map.GetColor(
                            (float)mesh.GetAxisAlignedBoundingBox().GetYPercentage(vertex(1)));
                    break;
                case RenderOption::MeshColorOption::ZCoordinate:
                    color = global_color_map.GetColor(
                            (float)mesh.GetAxisAlignedBoundingBox().GetZPercentage(vertex(2)));
                    break;
                case RenderOption::MeshColorOption::Color:
                    if (mesh.HasVertexColors()) {
                        auto float_color = mesh.vertex_colors_[vi].cast<float>();
                        color = Color(float_color.x(), float_color.y(), float_color.z());
                        break;
                    }
                    // fallthrough
                case RenderOption::MeshColorOption::Default:
                default:
                    color = option.default_mesh_color_;
                    break;
            }
            colors_[idx] = color;
        }
    }
    return true;
}

bool SimpleShader::BindTetraMesh(const geometry::TetraMesh &tetra_mesh, const RenderOption &option) {
    typedef decltype(geometry::TetraMesh::tetras_)::value_type TetraIndices;
    typedef decltype(geometry::TetraMesh::tetras_)::value_type::Scalar Index;
    typedef std::tuple<Index, Index> Index2;

    if (!tetra_mesh.HasTetras()) {
        LOGW("Binding failed with empty geometry::TetraMesh.")
        return false;
    }

    std::unordered_set<Index2, utility::hash_tuple<Index2>> inserted_edges;
    auto insert_edge = [&](Index vidx_0, Index vidx_1) {
        Index2 edge(std::min(vidx_0, vidx_1), std::max(vidx_0, vidx_1));
        if (inserted_edges.count(edge) == 0) {
            inserted_edges.insert(edge);
            Eigen::Vector3f p_0 = tetra_mesh.vertices_[vidx_0].cast<float>();
            Eigen::Vector3f p_1 = tetra_mesh.vertices_[vidx_1].cast<float>();
            points_.insert(points_.end(), {Vector3F(p_0.x(), p_0.y(), p_0.z()), Vector3F(p_1.x(), p_1.y(), p_1.z())});
            colors_.insert(colors_.end(), {Color(), Color()});
        }
    };

    for (auto tetra : tetra_mesh.tetras_) {
        insert_edge(tetra(0), tetra(1));
        insert_edge(tetra(1), tetra(2));
        insert_edge(tetra(2), tetra(0));
        insert_edge(tetra(3), tetra(0));
        insert_edge(tetra(3), tetra(1));
        insert_edge(tetra(3), tetra(2));
    }
    return true;
}

bool SimpleShader::BindPointCloud(const geometry::PointCloud &pointcloud, const RenderOption &option) {
    if (!pointcloud.HasPoints()) {
        LOGW("Binding failed with empty pointcloud.")
        return false;
    }

    const ColorMap &global_color_map = *GetGlobalColorMap();
    points_.resize(pointcloud.points_.size());
    colors_.resize(pointcloud.points_.size());
    for (size_t i = 0; i < pointcloud.points_.size(); i++) {
        const auto &point = pointcloud.points_[i].cast<float>();
        points_[i] = Vector3F(point.x(), point.y(), point.z());

        Color color;
        switch (option.point_color_option_) {
            case RenderOption::PointColorOption::XCoordinate:
                color = global_color_map.GetColor(
                        (float)pointcloud.GetAxisAlignedBoundingBox().GetXPercentage(point(0)));
                break;
            case RenderOption::PointColorOption::YCoordinate:
                color = global_color_map.GetColor(
                        (float)pointcloud.GetAxisAlignedBoundingBox().GetYPercentage(point(0)));
                break;
            case RenderOption::PointColorOption::ZCoordinate:
                color = global_color_map.GetColor(
                        (float)pointcloud.GetAxisAlignedBoundingBox().GetZPercentage(point(0)));
                break;
            case RenderOption::PointColorOption::Color:
            case RenderOption::PointColorOption::Default:
            default:
                if (pointcloud.HasColors()) {
                    auto float_color = pointcloud.colors_[i].cast<float>();
                    color = Color(float_color.x(), float_color.y(), float_color.z());
                } else {
                    color = global_color_map.GetColor(
                            (float)pointcloud.GetAxisAlignedBoundingBox().GetZPercentage(point(0)));
                }
                break;
        }
        color.a = 1.0;
        colors_[i] = color;
    }
    return true;
}

bool SimpleShader::BindVoxelGridLine(const geometry::VoxelGrid &grid, const RenderOption &option) { return true; }

bool SimpleShader::BindVoxelGridFace(const geometry::VoxelGrid &grid, const RenderOption &option) { return true; }

bool SimpleShader::BindVoxelOctreeLine(const geometry::Octree &octree, const RenderOption &option) { return true; }

bool SimpleShader::BindVoxelOctreeFace(const geometry::Octree &octree, const RenderOption &option) { return true; }

}  // namespace vox::visualization