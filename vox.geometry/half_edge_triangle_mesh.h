//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Core>
#include <unordered_map>

#include "geometry_3d.h"
#include "mesh_base.h"

namespace vox::geometry {

/// \class HalfEdgeTriangleMesh
///
/// \brief HalfEdgeTriangleMesh inherits TriangleMesh class with the addition of
/// HalfEdge data structure for each half edge in the mesh as well as related
/// functions.
class HalfEdgeTriangleMesh : public MeshBase {
public:
    /// \class HalfEdge
    ///
    /// \brief HalfEdge class contains vertex, triangle info about a half edge,
    /// as well as relations of next and twin half edge.
    class HalfEdge {
    public:
        /// \brief Default Constructor.
        ///
        /// Initializes all members of the instance with invalid values.
        HalfEdge() : next_(-1), twin_(-1), vertex_indices_(-1, -1), triangle_index_(-1) {}
        HalfEdge(Eigen::Vector2i vertex_indices, int triangle_index, int next, int twin);
        /// Returns `true` iff the half edge is the boundary (has not twin, i.e.
        /// twin index == -1).
        [[nodiscard]] bool IsBoundary() const { return twin_ == -1; }

    public:
        /// Index of the next HalfEdge in the same triangle.
        int next_;
        /// Index of the twin HalfEdge.
        int twin_;
        /// Index of the ordered vertices forming this half edge.
        Eigen::Vector2i vertex_indices_;
        /// Index of the triangle containing this half edge.
        int triangle_index_;
    };

public:
    /// \brief Default Constructor.
    ///
    /// Creates an empty instance with GeometryType of HalfEdgeTriangleMesh.
    HalfEdgeTriangleMesh() : MeshBase(Geometry::GeometryType::HALF_EDGE_TRIANGLE_MESH) {}

    HalfEdgeTriangleMesh &Clear() override;

    /// Returns `true` if half-edges have already been computed.
    [[nodiscard]] bool HasHalfEdges() const;

    /// Query manifold boundary half edges from a starting vertex
    /// If query vertex is not on boundary, empty vector will be returned.
    [[nodiscard]] std::vector<int> BoundaryHalfEdgesFromVertex(int vertex_index) const;

    /// Query manifold boundary vertices from a starting vertex
    /// If query vertex is not on boundary, empty vector will be returned.
    [[nodiscard]] std::vector<int> BoundaryVerticesFromVertex(int vertex_index) const;

    /// Returns a vector of boundaries. A boundary is a vector of vertices.
    [[nodiscard]] std::vector<std::vector<int>> GetBoundaries() const;

    HalfEdgeTriangleMesh &operator+=(const HalfEdgeTriangleMesh &mesh);

    HalfEdgeTriangleMesh operator+(const HalfEdgeTriangleMesh &mesh) const;

    /// Convert HalfEdgeTriangleMesh from TriangleMesh. Throws exception if the
    /// input mesh is not manifold.
    static std::shared_ptr<HalfEdgeTriangleMesh> CreateFromTriangleMesh(const TriangleMesh &mesh);

protected:
    /// \brief Parameterized Constructor.
    ///
    /// Creates an empty instance with GeometryType of specified type.
    ///
    /// \param type Specifies GeometryType for the HalfEdgeTriangleMesh.
    explicit HalfEdgeTriangleMesh(Geometry::GeometryType type) : MeshBase(type) {}

    /// Returns the next half edge from starting vertex of the input half edge,
    /// in a counter-clock wise manner. Returns -1 if when hitting a boundary.
    /// This is done by traversing to the next, next and twin half edge.
    [[nodiscard]] int NextHalfEdgeFromVertex(int init_half_edge_index) const;
    [[nodiscard]] int NextHalfEdgeOnBoundary(int curr_half_edge_index) const;

public:
    /// List of triangles in the mesh.
    std::vector<Eigen::Vector3i> triangles_;
    /// List of triangle normals in the mesh.
    std::vector<Eigen::Vector3d> triangle_normals_;
    /// List of HalfEdge in the mesh.
    std::vector<HalfEdge> half_edges_;

    /// Counter-clockwise ordered half-edges started from each vertex.
    /// If the vertex is on boundary, the starting edge must be on boundary too.
    std::vector<std::vector<int>> ordered_half_edge_from_vertex_;
};

}  // namespace vox::geometry
