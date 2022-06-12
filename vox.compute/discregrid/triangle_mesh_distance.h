//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "vox.compute/discregrid/triangle_mesh.h"

namespace vox::compute::discregrid {
/* ==========================================  DEFINITIONS  ========================================== */
// Small vector 3D class
template <typename FLOAT>
struct Vec3r {
public:
    std::array<FLOAT, 3> v;

    Vec3r() = default;
    ;
    template <typename FLOAT_I>
    Vec3r(const FLOAT_I& x, const FLOAT_I& y, const FLOAT_I& z) {
        v[0] = static_cast<FLOAT>(x);
        v[1] = static_cast<FLOAT>(y);
        v[2] = static_cast<FLOAT>(z);
    }
    template <typename SIZE_T>
    const FLOAT& operator[](const SIZE_T& i) const {
        return v[i];
    }
    template <typename SIZE_T>
    FLOAT& operator[](const SIZE_T& i) {
        return v[i];
    }
    [[nodiscard]] FLOAT Dot(const Vec3r& u) const { return v[0] * u[0] + v[1] * u[1] + v[2] * u[2]; }
    [[nodiscard]] Vec3r<FLOAT> Cross(const Vec3r& u) const {
        return Vec3r(v[1] * u[2] - v[2] * u[1], -v[0] * u[2] + v[2] * u[0], v[0] * u[1] - v[1] * u[0]);
    }
    Vec3r<FLOAT> operator+(const Vec3r& u) const { return Vec3r(v[0] + u[0], v[1] + u[1], v[2] + u[2]); }
    Vec3r<FLOAT> operator-(const Vec3r& u) const { return Vec3r(v[0] - u[0], v[1] - u[1], v[2] - u[2]); }
    void operator+=(const Vec3r& u) {
        v[0] += u[0];
        v[1] += u[1];
        v[2] += u[2];
    }
    template <typename FLOAT_I>
    Vec3r<FLOAT> operator*(const FLOAT_I& a) const {
        return Vec3r(static_cast<FLOAT>(a) * v[0], static_cast<FLOAT>(a) * v[1], static_cast<FLOAT>(a) * v[2]);
    }
    template <typename FLOAT_I>
    Vec3r<FLOAT> operator/(const FLOAT_I& a) const {
        return Vec3r(v[0] / static_cast<FLOAT>(a), v[1] / static_cast<FLOAT>(a), v[2] / static_cast<FLOAT>(a));
    }
    template <typename FLOAT_I>
    void operator/=(const FLOAT_I& a) {
        v[0] /= static_cast<FLOAT>(a);
        v[1] /= static_cast<FLOAT>(a);
        v[2] /= static_cast<FLOAT>(a);
    }
    [[nodiscard]] FLOAT SquaredNorm() const { return this->Dot(*this); }
    [[nodiscard]] FLOAT Norm() const { return std::sqrt(this->SquaredNorm()); }
    [[nodiscard]] Vec3r<FLOAT> Normalized() const { return (*this) / this->Norm(); }
    void Normalize() {
        const FLOAT kNorm = this->Norm();
        v[0] /= kNorm;
        v[1] /= kNorm;
        v[2] /= kNorm;
    }
};
template <typename FLOAT, typename FLOAT_I>
static inline Vec3r<FLOAT> operator*(const FLOAT_I& a, const Vec3r<FLOAT>& v) {
    return Vec3r<FLOAT>(static_cast<FLOAT>(a) * v[0], static_cast<FLOAT>(a) * v[1], static_cast<FLOAT>(a) * v[2]);
}
using Vec3d = Vec3r<double>;
// -----------------------------------

// Point-Triangle distance definitions
enum class NearestEntity { V0, V1, V2, E01, E12, E02, F };
static double PointTriangleSqUnsigned(NearestEntity& nearest_entity,
                                      Vec3d& nearest_point,
                                      const Vec3d& point,
                                      const Vec3d& v0,
                                      const Vec3d& v1,
                                      const Vec3d& v2);
// -----------------------------------

// Struct that contains the result of a distance query
struct Result {
    double distance = std::numeric_limits<double>::max();
    Vec3d nearest_point;
    vox::compute::discregrid::NearestEntity nearest_entity;
    int triangle_id = -1;
};
// -----------------------------------

/**
 * A class to compute signed and unsigned distances to a connected
 * and watertight triangle mesh.
 */
class TriangleMeshDistance {
private:
    /* Definitions */
    struct BoundingSphere {
        Vec3d center{};
        double radius{};
    };

    struct Node {
        BoundingSphere bv_left;
        BoundingSphere bv_right;
        int left = -1;  // If left == -1, right is the triangle_id
        int right = -1;
    };

    struct Triangle {
        std::array<Vec3d, 3> vertices{};
        int id = -1;
    };

    /* Fields */
    std::vector<Vec3d> vertices_;
    std::vector<std::array<int, 3>> triangles_;
    std::vector<Node> nodes_;
    std::vector<Vec3d> pseudonormals_triangles_;
    std::vector<std::array<Vec3d, 3>> pseudonormals_edges_;
    std::vector<Vec3d> pseudonormals_vertices_;
    BoundingSphere root_bv_;
    bool is_constructed_ = false;

    /* Methods */
    void Construct();
    void BuildTree(int node_id, BoundingSphere& bounding_sphere, std::vector<Triangle>& triangles, int begin, int end);
    void Query(Result& result, const Node& node, const Vec3d& point) const;

public:
    /* Methods */
    TriangleMeshDistance() = default;

    /**
     * @brief Constructs a new TriangleMeshDistance object.
     *
     * @param vertices Pointer to the vertices coordinates array in xyzxyz... layout.
     * @param n_vertices Number of vertices.
     * @param triangles Pointer to the conectivity array in ijkijk... layout.
     * @param n_triangles Number of triangles.
     */
    template <typename FLOAT, typename INT, typename SIZE_T>
    TriangleMeshDistance(const FLOAT* vertices, SIZE_T n_vertices, const INT* triangles, SIZE_T n_triangles);

    /**
     * @brief Constructs a new TriangleMeshDistance object.
     *
     * @param vertices Vertices of the triangle mesh. Y coordinate of the 3rd vertex should be access by
     * `vertices[2][1]`.
     * @param triangles Triangles of the triangle mesh. Index of the 2nd vertex of the 3rd triangle should be access by
     * `triangles[2][1]`.
     */
    template <typename IndexableVector3double, typename IndexableVector3int>
    TriangleMeshDistance(const std::vector<IndexableVector3double>& vertices,
                         const std::vector<IndexableVector3int>& triangles);

    /**
     * @brief Constructs a new TriangleMeshDistance object.
     *
     * @param mesh Discregrid TriangleMesh object.
     */
    explicit TriangleMeshDistance(const TriangleMesh& mesh);

    /**
     * @brief Initializes an existing TriangleMeshDistance object (including empty ones).
     *
     * @param vertices Pointer to the vertices coordinates array in xyzxyz... layout.
     * @param n_vertices Number of vertices.
     * @param triangles Pointer to the conectivity array in ijkijk... layout.
     * @param n_triangles Number of triangles.
     */
    template <typename FLOAT, typename INT, typename SIZE_T>
    void Construct(const FLOAT* vertices, SIZE_T n_vertices, const INT* triangles, SIZE_T n_triangles);

    /**
     * @brief Initializes an existing TriangleMeshDistance object (including empty ones).
     *
     * @param vertices Vertices of the triangle mesh. Y coordinate of the 3rd vertex should be access by
     * `vertices[2][1]`.
     * @param triangles Triangles of the triangle mesh. Index of the 2nd vertex of the 3rd triangle should be access by
     * `triangles[2][1]`.
     */
    template <typename IndexableVector3double, typename IndexableVector3int>
    void Construct(const std::vector<IndexableVector3double>& vertices,
                   const std::vector<IndexableVector3int>& triangles);

    /**
     * @brief Computes the unsigned distance from a point to the triangle mesh. Thread safe.
     *
     * @param point to query from. Typed to `Vec3d` but can be passed as `{x, y, z}`.
     *
     * @return Result containing distance, nearest point on the mesh, nearest entity and the nearest triangle index.
     */
    template <typename IndexableVector3double>
    Result UnsignedDistance(const IndexableVector3double& point) const;
    [[nodiscard]] Result UnsignedDistance(const std::array<double, 3>& point) const;

    /**
     * @brief Computes the unsigned distance from a point to the triangle mesh. Thread safe.
     *
     * @param point to query from. Typed to `Vec3d` but can be passed as `{x, y, z}`.
     *
     * @return Result containing distance, nearest point on the mesh, nearest entity and the nearest triangle index.
     */
    template <typename IndexableVector3double>
    Result SignedDistance(const IndexableVector3double& point) const;
    [[nodiscard]] Result SignedDistance(const std::array<double, 3>& point) const;
};
}  // namespace vox::compute::discregrid

/* ==========================================  DECLARATIONS  ========================================== */
template <typename FLOAT, typename INT, typename SIZE_T>
inline vox::compute::discregrid::TriangleMeshDistance::TriangleMeshDistance(const FLOAT* vertices,
                                                                          const SIZE_T n_vertices,
                                                                          const INT* triangles,
                                                                          const SIZE_T n_triangles) {
    this->Construct(vertices, n_vertices, triangles, n_triangles);
}

template <typename IndexableVector3double, typename IndexableVector3int>
inline vox::compute::discregrid::TriangleMeshDistance::TriangleMeshDistance(
        const std::vector<IndexableVector3double>& vertices, const std::vector<IndexableVector3int>& triangles) {
    this->Construct(vertices, triangles);
}

inline vox::compute::discregrid::TriangleMeshDistance::TriangleMeshDistance(const TriangleMesh& mesh) {
    this->Construct(mesh.VertexData(), mesh.FaceData());
}

template <typename FLOAT, typename INT, typename SIZE_T>
inline void vox::compute::discregrid::TriangleMeshDistance::Construct(const FLOAT* vertices,
                                                                    const SIZE_T n_vertices,
                                                                    const INT* triangles,
                                                                    const SIZE_T n_triangles) {
    this->vertices_.resize(3 * n_vertices);
    for (size_t i = 0; i < (size_t)n_vertices; i++) {
        this->vertices_[i][0] = (double)vertices[3 * i + 0];
        this->vertices_[i][1] = (double)vertices[3 * i + 1];
        this->vertices_[i][2] = (double)vertices[3 * i + 2];
    }

    this->triangles_.resize(3 * n_triangles);
    for (size_t i = 0; i < (size_t)n_triangles; i++) {
        this->triangles_[i][0] = (int)triangles[3 * i + 0];
        this->triangles_[i][1] = (int)triangles[3 * i + 1];
        this->triangles_[i][2] = (int)triangles[3 * i + 2];
    }
    this->Construct();
}

template <typename IndexableVector3double, typename IndexableVector3int>
inline void vox::compute::discregrid::TriangleMeshDistance::Construct(const std::vector<IndexableVector3double>& vertices,
                                                                    const std::vector<IndexableVector3int>& triangles) {
    this->vertices_.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++) {
        this->vertices_[i][0] = (double)vertices[i][0];
        this->vertices_[i][1] = (double)vertices[i][1];
        this->vertices_[i][2] = (double)vertices[i][2];
    }
    this->triangles_.resize(triangles.size());
    for (size_t i = 0; i < triangles.size(); i++) {
        this->triangles_[i][0] = (int)triangles[i][0];
        this->triangles_[i][1] = (int)triangles[i][1];
        this->triangles_[i][2] = (int)triangles[i][2];
    }
    this->Construct();
}

inline vox::compute::discregrid::Result vox::compute::discregrid::TriangleMeshDistance::SignedDistance(
        const std::array<double, 3>& point) const {
    const Vec3d kP(point[0], point[1], point[2]);
    Result result = this->UnsignedDistance(kP);

    const std::array<int, 3>& triangle = this->triangles_[result.triangle_id];
    Vec3d pseudonormal;
    switch (result.nearest_entity) {
        case vox::compute::discregrid::NearestEntity::V0:
            pseudonormal = this->pseudonormals_vertices_[triangle[0]];
            break;
        case vox::compute::discregrid::NearestEntity::V1:
            pseudonormal = this->pseudonormals_vertices_[triangle[1]];
            break;
        case vox::compute::discregrid::NearestEntity::V2:
            pseudonormal = this->pseudonormals_vertices_[triangle[2]];
            break;
        case vox::compute::discregrid::NearestEntity::E01:
            pseudonormal = this->pseudonormals_edges_[result.triangle_id][0];
            break;
        case vox::compute::discregrid::NearestEntity::E12:
            pseudonormal = this->pseudonormals_edges_[result.triangle_id][1];
            break;
        case vox::compute::discregrid::NearestEntity::E02:
            pseudonormal = this->pseudonormals_edges_[result.triangle_id][2];
            break;
        case vox::compute::discregrid::NearestEntity::F:
            pseudonormal = this->pseudonormals_triangles_[result.triangle_id];
            break;

        default:
            break;
    }

    const Vec3d kU = kP - result.nearest_point;
    result.distance *= (kU.Dot(pseudonormal) >= 0.0) ? 1.0 : -1.0;

    return result;
}

template <typename IndexableVector3double>
inline vox::compute::discregrid::Result vox::compute::discregrid::TriangleMeshDistance::SignedDistance(
        const IndexableVector3double& point) const {
    return this->SignedDistance(
            {static_cast<double>(point[0]), static_cast<double>(point[1]), static_cast<double>(point[2])});
}

inline vox::compute::discregrid::Result vox::compute::discregrid::TriangleMeshDistance::UnsignedDistance(
        const std::array<double, 3>& point) const {
    if (!this->is_constructed_) {
        std::cout << "DistanceTriangleMesh error: not constructed." << std::endl;
        exit(-1);
    }

    const Vec3d kP(point[0], point[1], point[2]);
    Result result;
    result.distance = std::numeric_limits<double>::max();
    this->Query(result, this->nodes_[0], kP);
    return result;
}

template <typename IndexableVector3double>
inline vox::compute::discregrid::Result vox::compute::discregrid::TriangleMeshDistance::UnsignedDistance(
        const IndexableVector3double& point) const {
    return this->UnsignedDistance(
            {static_cast<double>(point[0]), static_cast<double>(point[1]), static_cast<double>(point[2])});
}

inline void vox::compute::discregrid::TriangleMeshDistance::Construct() {
    if (this->triangles_.empty()) {
        std::cout << "DistanceTriangleMesh error: Empty triangle list." << std::endl;
        exit(-1);
    }

    // Build the tree containing the triangles
    std::vector<Triangle> triangles;

    triangles.resize(this->triangles_.size());
    for (int i = 0; i < (int)this->triangles_.size(); i++) {
        triangles[i].id = i;

        const std::array<int, 3>& triangle = this->triangles_[i];
        triangles[i].vertices[0] = this->vertices_[triangle[0]];
        triangles[i].vertices[1] = this->vertices_[triangle[1]];
        triangles[i].vertices[2] = this->vertices_[triangle[2]];
    }

    this->nodes_.emplace_back();
    this->BuildTree(0, this->root_bv_, triangles, 0, (int)triangles.size());

    // Compute pseudonormals
    //// Edge data structure
    std::unordered_map<uint64_t, Vec3d> edge_normals;
    std::unordered_map<uint64_t, int> edges_count;
    const auto kNVertices = (uint64_t)this->vertices_.size();
    auto add_edge_normal = [&](const int i, const int j, const Vec3d& triangle_normal) {
        const uint64_t kEy = std::min(i, j) * kNVertices + std::max(i, j);
        if (edge_normals.find(kEy) == edge_normals.end()) {
            edge_normals[kEy] = triangle_normal;
            edges_count[kEy] = 1;
        } else {
            edge_normals[kEy] += triangle_normal;
            edges_count[kEy] += 1;
        }
    };
    auto get_edge_normal = [&](const int i, const int j) {
        const uint64_t kEy = std::min(i, j) * kNVertices + std::max(i, j);
        return edge_normals.find(kEy)->second;
    };

    //// Compute
    this->pseudonormals_triangles_.resize(this->triangles_.size());
    this->pseudonormals_edges_.resize(this->triangles_.size());
    this->pseudonormals_vertices_.resize(this->vertices_.size(), {0, 0, 0});
    for (int i = 0; i < (int)this->triangles_.size(); i++) {
        // Triangle
        const std::array<int, 3>& triangle = this->triangles_[i];
        const Vec3d& a = this->vertices_[triangle[0]];
        const Vec3d& b = this->vertices_[triangle[1]];
        const Vec3d& c = this->vertices_[triangle[2]];

        const Vec3d kTriangleNormal = (b - a).Cross(c - a).Normalized();
        this->pseudonormals_triangles_[i] = kTriangleNormal;

        // Vertex
        const double kAlpha0 = std::acos(std::abs((b - a).Normalized().Dot((c - a).Normalized())));
        const double kAlpha1 = std::acos(std::abs((a - b).Normalized().Dot((c - b).Normalized())));
        const double kAlpha2 = std::acos(std::abs((b - c).Normalized().Dot((a - c).Normalized())));
        this->pseudonormals_vertices_[triangle[0]] += kAlpha0 * kTriangleNormal;
        this->pseudonormals_vertices_[triangle[1]] += kAlpha1 * kTriangleNormal;
        this->pseudonormals_vertices_[triangle[2]] += kAlpha2 * kTriangleNormal;

        // Edge
        add_edge_normal(triangle[0], triangle[1], kTriangleNormal);
        add_edge_normal(triangle[1], triangle[2], kTriangleNormal);
        add_edge_normal(triangle[0], triangle[2], kTriangleNormal);
    }

    for (Vec3d& n : this->pseudonormals_vertices_) {
        n.Normalize();
    }

    for (int tri_i = 0; tri_i < (int)this->triangles_.size(); tri_i++) {
        const std::array<int, 3>& triangle = this->triangles_[tri_i];
        this->pseudonormals_edges_[tri_i][0] = get_edge_normal(triangle[0], triangle[1]).Normalized();
        this->pseudonormals_edges_[tri_i][1] = get_edge_normal(triangle[1], triangle[2]).Normalized();
        this->pseudonormals_edges_[tri_i][2] = get_edge_normal(triangle[0], triangle[2]).Normalized();
    }

    // Check that the mesh is watertight: All edges appear exactly once.
    bool single_edge_found = false;
    bool triple_edge_found = false;
    for (const auto kEdgeCount : edges_count) {
        if (kEdgeCount.second == 1) {
            single_edge_found = true;
        } else if (kEdgeCount.second > 2) {
            triple_edge_found = true;
        }
    }
    if (single_edge_found) {
        std::cout << "DistanceTriangleMesh warning: mesh is not watertight. At least one edge found belonging to just "
                     "one triangle."
                  << std::endl;
    }
    if (triple_edge_found) {
        std::cout << "DistanceTriangleMesh warning: mesh is not watertight. At least one edge found belonging to more "
                     "than two triangle."
                  << std::endl;
    }

    this->is_constructed_ = true;
}

inline void vox::compute::discregrid::TriangleMeshDistance::BuildTree(const int node_id,
                                                                    BoundingSphere& bounding_sphere,
                                                                    std::vector<Triangle>& triangles,
                                                                    const int begin,
                                                                    const int end) {
    const int kNTriangles = end - begin;

    if (kNTriangles == 0) {
        std::cout << "DistanceTriangleMesh::_construct error: Empty leave." << std::endl;
        exit(-1);
    } else if (kNTriangles == 1) {
        // Build node leaf
        this->nodes_[node_id].left = -1;
        this->nodes_[node_id].right = triangles[begin].id;

        //// Bounding sphere
        const Triangle& tri = triangles[begin];
        const Vec3d kCenter = (tri.vertices[0] + tri.vertices[1] + tri.vertices[2]) / 3.0;
        const double kRadius =
                std::max(std::max((tri.vertices[0] - kCenter).Norm(), (tri.vertices[1] - kCenter).Norm()),
                         (tri.vertices[2] - kCenter).Norm());
        bounding_sphere.center = kCenter;
        bounding_sphere.radius = kRadius;
    } else {
        // Compute AxisAligned Bounding Box center and largest dimension of all current triangles
        Vec3d top = {std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(),
                     std::numeric_limits<double>::lowest()};
        Vec3d bottom = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                        std::numeric_limits<double>::max()};
        Vec3d center = {0, 0, 0};
        for (int tri_i = begin; tri_i < end; tri_i++) {
            for (int vertex_i = 0; vertex_i < 3; vertex_i++) {
                const Vec3d& p = triangles[tri_i].vertices[vertex_i];
                center += p;

                for (int coord_i = 0; coord_i < 3; coord_i++) {
                    top[coord_i] = std::max(top[coord_i], p[coord_i]);
                    bottom[coord_i] = std::min(bottom[coord_i], p[coord_i]);
                }
            }
        }
        center /= 3 * kNTriangles;
        const Vec3d kDiagonal = top - bottom;
        const int kSplitDim = (int)(std::max_element(&kDiagonal[0], &kDiagonal[0] + 3) - &kDiagonal[0]);

        // Set node bounding sphere
        double radius_sq = 0.0;
        for (int tri_i = begin; tri_i < end; tri_i++) {
            for (int i = 0; i < 3; i++) {
                radius_sq = std::max(radius_sq, (center - triangles[tri_i].vertices[i]).SquaredNorm());
            }
        }
        bounding_sphere.center = center;
        bounding_sphere.radius = std::sqrt(radius_sq);

        // Sort the triangles according to their center along the split dimension
        std::sort(triangles.begin() + begin, triangles.begin() + end,
                  [kSplitDim](const Triangle& a, const Triangle& b) {
                      return a.vertices[0][kSplitDim] < b.vertices[0][kSplitDim];
                  });

        // Children
        const int kMid = (int)(0.5 * (begin + end));

        this->nodes_[node_id].left = (int)this->nodes_.size();
        this->nodes_.emplace_back();
        this->BuildTree(this->nodes_[node_id].left, this->nodes_[node_id].bv_left, triangles, begin, kMid);

        this->nodes_[node_id].right = (int)this->nodes_.size();
        this->nodes_.emplace_back();
        this->BuildTree(this->nodes_[node_id].right, this->nodes_[node_id].bv_right, triangles, kMid, end);
    }
}

inline void vox::compute::discregrid::TriangleMeshDistance::Query(Result& result,
                                                                const Node& node,
                                                                const Vec3d& point) const {
    // End of recursion
    if (node.left == -1) {
        const int kTriangleId = node.right;
        const std::array<int, 3>& triangle = this->triangles_[node.right];  // If left == -1, right is the triangle_id
        const Vec3d& v0 = this->vertices_[triangle[0]];
        const Vec3d& v1 = this->vertices_[triangle[1]];
        const Vec3d& v2 = this->vertices_[triangle[2]];

        Vec3d nearest_point;
        vox::compute::discregrid::NearestEntity nearest_entity;
        const double kDistanceSq =
                vox::compute::discregrid::PointTriangleSqUnsigned(nearest_entity, nearest_point, point, v0, v1, v2);

        if (kDistanceSq < result.distance * result.distance) {
            result.nearest_point = nearest_point;
            result.nearest_entity = nearest_entity;
            result.distance = std::sqrt(kDistanceSq);
            result.triangle_id = kTriangleId;
        }
    }

    // Recursion
    else {
        // Find which child bounding volume is closer
        const double kDLeft = (point - node.bv_left.center).Norm() - node.bv_left.radius;
        const double kDRight = (point - node.bv_right.center).Norm() - node.bv_right.radius;

        if (kDLeft < kDRight) {
            // Overlap test
            if (kDLeft < result.distance) {
                this->Query(result, this->nodes_[node.left], point);
            }

            if (kDRight < result.distance) {
                this->Query(result, this->nodes_[node.right], point);
            }
        } else {
            if (kDRight < result.distance) {
                this->Query(result, this->nodes_[node.right], point);
            }
            if (kDLeft < result.distance) {
                this->Query(result, this->nodes_[node.left], point);
            }
        }
    }
}

static double vox::compute::discregrid::PointTriangleSqUnsigned(NearestEntity& nearest_entity,
                                                              Vec3d& nearest_point,
                                                              const Vec3d& point,
                                                              const Vec3d& v0,
                                                              const Vec3d& v1,
                                                              const Vec3d& v2) {
    Vec3d diff = v0 - point;
    Vec3d edge0 = v1 - v0;
    Vec3d edge1 = v2 - v0;
    double a00 = edge0.Dot(edge0);
    double a01 = edge0.Dot(edge1);
    double a11 = edge1.Dot(edge1);
    double b0 = diff.Dot(edge0);
    double b1 = diff.Dot(edge1);
    double c = diff.Dot(diff);
    double det = std::abs(a00 * a11 - a01 * a01);
    double s = a01 * b1 - a11 * b0;
    double t = a01 * b0 - a00 * b1;

    double d2 = -1.0;

    if (s + t <= det) {
        if (s < 0) {
            if (t < 0)  // region 4
            {
                if (b0 < 0) {
                    t = 0;
                    if (-b0 >= a00) {
                        nearest_entity = NearestEntity::V1;
                        s = 1;
                        d2 = a00 + (2) * b0 + c;
                    } else {
                        nearest_entity = NearestEntity::E01;
                        s = -b0 / a00;
                        d2 = b0 * s + c;
                    }
                } else {
                    s = 0;
                    if (b1 >= 0) {
                        nearest_entity = NearestEntity::V0;
                        t = 0;
                        d2 = c;
                    } else if (-b1 >= a11) {
                        nearest_entity = NearestEntity::V2;
                        t = 1;
                        d2 = a11 + (2) * b1 + c;
                    } else {
                        nearest_entity = NearestEntity::E02;
                        t = -b1 / a11;
                        d2 = b1 * t + c;
                    }
                }
            } else  // region 3
            {
                s = 0;
                if (b1 >= 0) {
                    nearest_entity = NearestEntity::V0;
                    t = 0;
                    d2 = c;
                } else if (-b1 >= a11) {
                    nearest_entity = NearestEntity::V2;
                    t = 1;
                    d2 = a11 + (2) * b1 + c;
                } else {
                    nearest_entity = NearestEntity::E02;
                    t = -b1 / a11;
                    d2 = b1 * t + c;
                }
            }
        } else if (t < 0)  // region 5
        {
            t = 0;
            if (b0 >= 0) {
                nearest_entity = NearestEntity::V0;
                s = 0;
                d2 = c;
            } else if (-b0 >= a00) {
                nearest_entity = NearestEntity::V1;
                s = 1;
                d2 = a00 + (2) * b0 + c;
            } else {
                nearest_entity = NearestEntity::E01;
                s = -b0 / a00;
                d2 = b0 * s + c;
            }
        } else  // region 0
        {
            nearest_entity = NearestEntity::F;
            // minimum at interior point
            double invDet = (1) / det;
            s *= invDet;
            t *= invDet;
            d2 = s * (a00 * s + a01 * t + (2) * b0) + t * (a01 * s + a11 * t + (2) * b1) + c;
        }
    } else {
        double tmp0, tmp1, numer, denom;

        if (s < 0)  // region 2
        {
            tmp0 = a01 + b0;
            tmp1 = a11 + b1;
            if (tmp1 > tmp0) {
                numer = tmp1 - tmp0;
                denom = a00 - (2) * a01 + a11;
                if (numer >= denom) {
                    nearest_entity = NearestEntity::V1;
                    s = 1;
                    t = 0;
                    d2 = a00 + (2) * b0 + c;
                } else {
                    nearest_entity = NearestEntity::E12;
                    s = numer / denom;
                    t = 1 - s;
                    d2 = s * (a00 * s + a01 * t + (2) * b0) + t * (a01 * s + a11 * t + (2) * b1) + c;
                }
            } else {
                s = 0;
                if (tmp1 <= 0) {
                    nearest_entity = NearestEntity::V2;
                    t = 1;
                    d2 = a11 + (2) * b1 + c;
                } else if (b1 >= 0) {
                    nearest_entity = NearestEntity::V0;
                    t = 0;
                    d2 = c;
                } else {
                    nearest_entity = NearestEntity::E02;
                    t = -b1 / a11;
                    d2 = b1 * t + c;
                }
            }
        } else if (t < 0)  // region 6
        {
            tmp0 = a01 + b1;
            tmp1 = a00 + b0;
            if (tmp1 > tmp0) {
                numer = tmp1 - tmp0;
                denom = a00 - (2) * a01 + a11;
                if (numer >= denom) {
                    nearest_entity = NearestEntity::V2;
                    t = 1;
                    s = 0;
                    d2 = a11 + (2) * b1 + c;
                } else {
                    nearest_entity = NearestEntity::E12;
                    t = numer / denom;
                    s = 1 - t;
                    d2 = s * (a00 * s + a01 * t + (2) * b0) + t * (a01 * s + a11 * t + (2) * b1) + c;
                }
            } else {
                t = 0;
                if (tmp1 <= 0) {
                    nearest_entity = NearestEntity::V1;
                    s = 1;
                    d2 = a00 + (2) * b0 + c;
                } else if (b0 >= 0) {
                    nearest_entity = NearestEntity::V0;
                    s = 0;
                    d2 = c;
                } else {
                    nearest_entity = NearestEntity::E01;
                    s = -b0 / a00;
                    d2 = b0 * s + c;
                }
            }
        } else  // region 1
        {
            numer = a11 + b1 - a01 - b0;
            if (numer <= 0) {
                nearest_entity = NearestEntity::V2;
                s = 0;
                t = 1;
                d2 = a11 + (2) * b1 + c;
            } else {
                denom = a00 - (2) * a01 + a11;
                if (numer >= denom) {
                    nearest_entity = NearestEntity::V1;
                    s = 1;
                    t = 0;
                    d2 = a00 + (2) * b0 + c;
                } else {
                    nearest_entity = NearestEntity::E12;
                    s = numer / denom;
                    t = 1 - s;
                    d2 = s * (a00 * s + a01 * t + (2) * b0) + t * (a01 * s + a11 * t + (2) * b1) + c;
                }
            }
        }
    }

    // Account for numerical round-off error.
    if (d2 < 0) {
        d2 = 0;
    }

    nearest_point = v0 + s * edge0 + t * edge1;
    return d2;
}
