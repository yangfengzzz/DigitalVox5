//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Core>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "vox.base/helper.h"
#include "vox.geometry/geometry_3d.h"

namespace vox::geometry {

class PointCloud;
class TriangleMesh;

/// \class MeshBase
///
/// \brief MeshBash Class.
///
/// Triangle mesh contains vertices. Optionally, the mesh may also contain
/// vertex normals and vertex colors.
class MeshBase : public Geometry3D {
public:
    /// \brief Indicates the method that is used for mesh simplification if
    /// multiple vertices are combined to a single one.
    ///
    /// \param Average indicates that the average position is computed as
    /// output.
    /// \param Quadric indicates that the distance to the adjacent triangle
    /// planes is minimized. Cf. "Simplifying Surfaces with Color and Texture
    /// using Quadric Error Metrics" by Garland and Heckbert.
    enum class SimplificationContraction { Average, Quadric };

    /// \brief Indicates the scope of filter operations.
    ///
    /// \param All indicates that all properties (color, normal,
    /// vertex position) are filtered.
    /// \param Color indicates that only the colors are filtered.
    /// \param Normal indicates that only the normals are filtered.
    /// \param Vertex indicates that only the vertex positions are filtered.
    enum class FilterScope { All, Color, Normal, Vertex };

    /// Energy model that is minimized in the DeformAsRigidAsPossible method.
    /// \param Spokes is the original energy as formulated in
    /// Sorkine and Alexa, "As-Rigid-As-Possible Surface Modeling", 2007.
    /// \param Smoothed adds a rotation smoothing term to the rotations.
    enum class DeformAsRigidAsPossibleEnergy { Spokes, Smoothed };

    /// \brief Default Constructor.
    MeshBase() : Geometry3D(Geometry::GeometryType::MESH_BASE) {}
    ~MeshBase() override = default;

public:
    MeshBase &Clear() override;
    [[nodiscard]] bool IsEmpty() const override;
    [[nodiscard]] Eigen::Vector3d GetMinBound() const override;
    [[nodiscard]] Eigen::Vector3d GetMaxBound() const override;
    [[nodiscard]] Eigen::Vector3d GetCenter() const override;
    [[nodiscard]] AxisAlignedBoundingBox GetAxisAlignedBoundingBox() const override;
    [[nodiscard]] OrientedBoundingBox GetOrientedBoundingBox(bool robust = false) const override;
    MeshBase &Transform(const Eigen::Matrix4d &transformation) override;
    MeshBase &Translate(const Eigen::Vector3d &translation, bool relative = true) override;
    MeshBase &Scale(double scale, const Eigen::Vector3d &center) override;
    MeshBase &Rotate(const Eigen::Matrix3d &R, const Eigen::Vector3d &center) override;

    MeshBase &operator+=(const MeshBase &mesh);
    MeshBase operator+(const MeshBase &mesh) const;

    /// Returns `True` if the mesh contains vertices.
    [[nodiscard]] bool HasVertices() const { return !vertices_.empty(); }

    /// Returns `True` if the mesh contains vertex normals.
    [[nodiscard]] bool HasVertexNormals() const {
        return !vertices_.empty() && vertex_normals_.size() == vertices_.size();
    }

    /// Returns `True` if the mesh contains vertex colors.
    [[nodiscard]] bool HasVertexColors() const {
        return !vertices_.empty() && vertex_colors_.size() == vertices_.size();
    }

    /// Normalize vertex normals to length 1.
    MeshBase &NormalizeNormals() {
        for (auto &vertex_normal : vertex_normals_) {
            vertex_normal.normalize();
            if (std::isnan(vertex_normal(0))) {
                vertex_normal = Eigen::Vector3d(0.0, 0.0, 1.0);
            }
        }
        return *this;
    }

    /// \brief Assigns each vertex in the TriangleMesh the same color
    ///
    /// \param color RGB colors of vertices.
    MeshBase &PaintUniformColor(const Eigen::Vector3d &color) {
        ResizeAndPaintUniformColor(vertex_colors_, vertices_.size(), color);
        return *this;
    }

    /// Function that computes the convex hull of the triangle mesh using qhull
    [[nodiscard]] std::tuple<std::shared_ptr<TriangleMesh>, std::vector<size_t>> ComputeConvexHull() const;

protected:
    // Forward child class type to avoid indirect nonvirtual base
    explicit MeshBase(Geometry::GeometryType type) : Geometry3D(type) {}
    MeshBase(Geometry::GeometryType type, std::vector<Eigen::Vector3d> vertices)
        : Geometry3D(type), vertices_(std::move(vertices)) {}

public:
    /// Vertex coordinates.
    std::vector<Eigen::Vector3d> vertices_;
    /// Vertex normals.
    std::vector<Eigen::Vector3d> vertex_normals_;
    /// RGB colors of vertices.
    std::vector<Eigen::Vector3d> vertex_colors_;
};

}  // namespace vox::geometry
