//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simple_shader.h"

#include "vox.base/logging.h"
#include "vox.visual/graphics/color_map.h"

namespace vox::visualization {
// Coordinates of 8 vertices in a cuboid (assume origin (0,0,0), size 1)
const static std::vector<Eigen::Vector3i> cuboid_vertex_offsets{
        Eigen::Vector3i(0, 0, 0), Eigen::Vector3i(1, 0, 0), Eigen::Vector3i(0, 1, 0), Eigen::Vector3i(1, 1, 0),
        Eigen::Vector3i(0, 0, 1), Eigen::Vector3i(1, 0, 1), Eigen::Vector3i(0, 1, 1), Eigen::Vector3i(1, 1, 1),
};

// Vertex indices of 12 triangles in a cuboid, for right-handed manifold mesh
const static std::vector<Eigen::Vector3i> cuboid_triangles_vertex_indices{
        Eigen::Vector3i(0, 2, 1), Eigen::Vector3i(0, 1, 4), Eigen::Vector3i(0, 4, 2), Eigen::Vector3i(5, 1, 7),
        Eigen::Vector3i(5, 7, 4), Eigen::Vector3i(5, 4, 1), Eigen::Vector3i(3, 7, 1), Eigen::Vector3i(3, 1, 2),
        Eigen::Vector3i(3, 2, 7), Eigen::Vector3i(6, 4, 7), Eigen::Vector3i(6, 7, 2), Eigen::Vector3i(6, 2, 4),
};

// Vertex indices of 12 lines in a cuboid
const static std::vector<Eigen::Vector2i> cuboid_lines_vertex_indices{
        Eigen::Vector2i(0, 1), Eigen::Vector2i(0, 2), Eigen::Vector2i(0, 4), Eigen::Vector2i(3, 1),
        Eigen::Vector2i(3, 2), Eigen::Vector2i(3, 7), Eigen::Vector2i(5, 1), Eigen::Vector2i(5, 4),
        Eigen::Vector2i(5, 7), Eigen::Vector2i(6, 2), Eigen::Vector2i(6, 4), Eigen::Vector2i(6, 7),
};

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

bool SimpleShader::BindVoxelGridLine(const geometry::VoxelGrid &voxel_grid, const RenderOption &option) {
    if (!voxel_grid.HasVoxels()) {
        LOGW("Binding failed with empty voxel grid.")
        return false;
    }
    const ColorMap &global_color_map = *GetGlobalColorMap();
    points_.clear();  // Final size: num_voxels * 12 * 2
    colors_.clear();  // Final size: num_voxels * 12 * 2

    for (auto &it : voxel_grid.voxels_) {
        const geometry::Voxel &voxel = it.second;
        // 8 vertices in a voxel
        Eigen::Vector3f base_vertex =
                voxel_grid.origin_.cast<float>() + voxel.grid_index_.cast<float>() * voxel_grid.voxel_size_;
        std::vector<Eigen::Vector3f> vertices;
        for (const Eigen::Vector3i &vertex_offset : cuboid_vertex_offsets) {
            vertices.emplace_back(base_vertex + vertex_offset.cast<float>() * voxel_grid.voxel_size_);
        }

        // Voxel color (applied to all points)
        Color voxel_color;
        switch (option.mesh_color_option_) {
            case RenderOption::MeshColorOption::XCoordinate:
                voxel_color = global_color_map.GetColor(
                        (float)voxel_grid.GetAxisAlignedBoundingBox().GetXPercentage(base_vertex(0)));
                break;
            case RenderOption::MeshColorOption::YCoordinate:
                voxel_color = global_color_map.GetColor(
                        (float)voxel_grid.GetAxisAlignedBoundingBox().GetYPercentage(base_vertex(1)));
                break;
            case RenderOption::MeshColorOption::ZCoordinate:
                voxel_color = global_color_map.GetColor(
                        (float)voxel_grid.GetAxisAlignedBoundingBox().GetZPercentage(base_vertex(2)));
                break;
            case RenderOption::MeshColorOption::Color:
                if (voxel_grid.HasColors()) {
                    auto float_color = voxel.color_.cast<float>();
                    voxel_color = Color(float_color.x(), float_color.y(), float_color.z());
                    break;
                }
                // fallthrough
            case RenderOption::MeshColorOption::Default:
            default:
                voxel_color = option.default_mesh_color_;
                break;
        }

        // 12 lines
        for (const Eigen::Vector2i &line_vertex_indices : cuboid_lines_vertex_indices) {
            auto point_first = vertices[line_vertex_indices(0)].cast<float>();
            points_.emplace_back(point_first.x(), point_first.y(), point_first.z());
            auto point_second = vertices[line_vertex_indices(1)].cast<float>();
            points_.emplace_back(point_second.x(), point_second.y(), point_second.z());
            colors_.push_back(voxel_color);
            colors_.push_back(voxel_color);
        }
    }

    return true;
}

bool SimpleShader::BindVoxelGridFace(const geometry::VoxelGrid &voxel_grid, const RenderOption &option) {
    if (!voxel_grid.HasVoxels()) {
        LOGW("Binding failed with empty voxel grid.")
        return false;
    }
    const ColorMap &global_color_map = *GetGlobalColorMap();
    points_.clear();  // Final size: num_voxels * 36
    colors_.clear();  // Final size: num_voxels * 36

    for (auto &it : voxel_grid.voxels_) {
        const geometry::Voxel &voxel = it.second;
        // 8 vertices in a voxel
        Eigen::Vector3f base_vertex =
                voxel_grid.origin_.cast<float>() + voxel.grid_index_.cast<float>() * voxel_grid.voxel_size_;
        std::vector<Eigen::Vector3f> vertices;
        for (const Eigen::Vector3i &vertex_offset : cuboid_vertex_offsets) {
            vertices.emplace_back(base_vertex + vertex_offset.cast<float>() * voxel_grid.voxel_size_);
        }

        // Voxel color (applied to all points)
        Color voxel_color;
        switch (option.mesh_color_option_) {
            case RenderOption::MeshColorOption::XCoordinate:
                voxel_color = global_color_map.GetColor(
                        (float)voxel_grid.GetAxisAlignedBoundingBox().GetXPercentage(base_vertex(0)));
                break;
            case RenderOption::MeshColorOption::YCoordinate:
                voxel_color = global_color_map.GetColor(
                        (float)voxel_grid.GetAxisAlignedBoundingBox().GetYPercentage(base_vertex(1)));
                break;
            case RenderOption::MeshColorOption::ZCoordinate:
                voxel_color = global_color_map.GetColor(
                        (float)voxel_grid.GetAxisAlignedBoundingBox().GetZPercentage(base_vertex(2)));
                break;
            case RenderOption::MeshColorOption::Color:
                if (voxel_grid.HasColors()) {
                    auto float_color = voxel.color_.cast<float>();
                    voxel_color = Color(float_color.x(), float_color.y(), float_color.z());
                    break;
                }
                // fallthrough
            case RenderOption::MeshColorOption::Default:
            default:
                voxel_color = option.default_mesh_color_;
                break;
        }

        // 12 triangles in a voxel
        for (const Eigen::Vector3i &triangle_vertex_indices : cuboid_triangles_vertex_indices) {
            auto point_1 = vertices[triangle_vertex_indices(0)].cast<float>();
            points_.emplace_back(point_1.x(), point_1.y(), point_1.z());
            auto point_2 = vertices[triangle_vertex_indices(1)].cast<float>();
            points_.emplace_back(point_2.x(), point_2.y(), point_2.z());
            auto point_3 = vertices[triangle_vertex_indices(2)].cast<float>();
            points_.emplace_back(point_3.x(), point_3.y(), point_3.z());
            colors_.push_back(voxel_color);
            colors_.push_back(voxel_color);
            colors_.push_back(voxel_color);
        }
    }
    return true;
}

bool SimpleShader::BindVoxelOctreeLine(const geometry::Octree &octree, const RenderOption &option) {
    if (octree.IsEmpty()) {
        LOGW("Binding failed with empty octree.")
        return false;
    }
    const ColorMap &global_color_map = *GetGlobalColorMap();
    points_.clear();  // Final size: num_voxels * 36
    colors_.clear();  // Final size: num_voxels * 36

    auto f = [this, &octree, &option, &global_color_map](
                     const std::shared_ptr<geometry::OctreeNode> &node,
                     const std::shared_ptr<geometry::OctreeNodeInfo> &node_info) -> bool {
        if (auto leaf_node = std::dynamic_pointer_cast<geometry::OctreeColorLeafNode>(node)) {
            // All vertex in the voxel share the same color
            Eigen::Vector3f base_vertex = node_info->origin_.cast<float>();
            std::vector<Eigen::Vector3f> vertices;
            for (const Eigen::Vector3i &vertex_offset : cuboid_vertex_offsets) {
                vertices.emplace_back(base_vertex + vertex_offset.cast<float>() * float(node_info->size_));
            }

            Color voxel_color;
            switch (option.mesh_color_option_) {
                case RenderOption::MeshColorOption::XCoordinate:
                    voxel_color = global_color_map.GetColor(
                            (float)octree.GetAxisAlignedBoundingBox().GetXPercentage(base_vertex(0)));
                    break;
                case RenderOption::MeshColorOption::YCoordinate:
                    voxel_color = global_color_map.GetColor(
                            (float)octree.GetAxisAlignedBoundingBox().GetYPercentage(base_vertex(1)));
                    break;
                case RenderOption::MeshColorOption::ZCoordinate:
                    voxel_color = global_color_map.GetColor(
                            (float)octree.GetAxisAlignedBoundingBox().GetZPercentage(base_vertex(2)));
                    break;
                case RenderOption::MeshColorOption::Color: {
                    auto float_color = leaf_node->color_.cast<float>();
                    voxel_color = Color(float_color.x(), float_color.y(), float_color.z());
                    break;
                }
                case RenderOption::MeshColorOption::Default:
                default:
                    voxel_color = option.default_mesh_color_;
                    break;
            }

            // 12 triangles in a voxel
            for (const Eigen::Vector3i &triangle_vertex_indices : cuboid_triangles_vertex_indices) {
                auto point_1 = vertices[triangle_vertex_indices(0)].cast<float>();
                points_.emplace_back(point_1.x(), point_1.y(), point_1.z());
                auto point_2 = vertices[triangle_vertex_indices(1)].cast<float>();
                points_.emplace_back(point_2.x(), point_2.y(), point_2.z());
                auto point_3 = vertices[triangle_vertex_indices(2)].cast<float>();
                points_.emplace_back(point_3.x(), point_3.y(), point_3.z());
                colors_.push_back(voxel_color);
                colors_.push_back(voxel_color);
                colors_.push_back(voxel_color);
            }
        }
        return false;
    };

    octree.Traverse(f);
    return true;
}

bool SimpleShader::BindVoxelOctreeFace(const geometry::Octree &octree, const RenderOption &option) {
    if (octree.IsEmpty()) {
        LOGW("Binding failed with empty octree.")
        return false;
    }
    points_.clear();  // Final size: num_voxels * 36
    colors_.clear();  // Final size: num_voxels * 36

    auto f = [this](const std::shared_ptr<geometry::OctreeNode> &node,
                             const std::shared_ptr<geometry::OctreeNodeInfo> &node_info) -> bool {
        Eigen::Vector3f base_vertex = node_info->origin_.cast<float>();
        std::vector<Eigen::Vector3f> vertices;
        for (const Eigen::Vector3i &vertex_offset : cuboid_vertex_offsets) {
            vertices.emplace_back(base_vertex + vertex_offset.cast<float>() * float(node_info->size_));
        }
        Color voxel_color;
        if (auto leaf_node = std::dynamic_pointer_cast<geometry::OctreeColorLeafNode>(node)) {
            auto float_color = leaf_node->color_.cast<float>();
            voxel_color = Color(float_color.x(), float_color.y(), float_color.z());
        }

        for (const Eigen::Vector2i &line_vertex_indices : cuboid_lines_vertex_indices) {
            auto point_first = vertices[line_vertex_indices(0)].cast<float>();
            points_.emplace_back(point_first.x(), point_first.y(), point_first.z());
            auto point_second = vertices[line_vertex_indices(1)].cast<float>();
            points_.emplace_back(point_second.x(), point_second.y(), point_second.z());
            colors_.push_back(voxel_color);
            colors_.push_back(voxel_color);
        }
        return false;
    };

    octree.Traverse(f);
    return true;
}

}  // namespace vox::visualization