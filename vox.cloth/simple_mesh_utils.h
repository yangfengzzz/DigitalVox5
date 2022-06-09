//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.cloth/foundation/PxVec4.h"
#include "vox.cloth/simple_mesh.h"

namespace vox::cloth {
/*
 * returns a PxVec4 containing [x,y,z,d] for plane equation ax + by + cz + d = 0.
 * Where plane contains p and has normal n.
 */
inline physx::PxVec4 ConstructPlaneFromPointNormal(const physx::PxVec3 &p, physx::PxVec3 n) {
    n.normalize();
    return physx::PxVec4(n, -p.dot(n));
}

/*
 * returns two vectors in b and c so that [a b c] form a basis.
 * a needs to be a unit vector.
 */
inline void ComputeBasis(const physx::PxVec3 &a, physx::PxVec3 *b, physx::PxVec3 *c) {
    if (fabsf(a.x) >= 0.57735f)
        *b = physx::PxVec3(a.y, -a.x, 0.0f);
    else
        *b = physx::PxVec3(0.0f, a.z, -a.y);

    *b = b->getNormalized();
    *c = a.cross(*b);
}

struct Point {
    Point() = default;

    Point(const physx::PxVec3 &p) : p(p) {}

    physx::PxVec3 p;

    Point operator*(float f) const { return Point(p * f); }

    Point operator+(const Point &pt) const { return Point(p + pt.p); }
};

struct RenderVertex {
    RenderVertex() = default;

    RenderVertex(const physx::PxVec3 &p, const physx::PxVec3 &n) : p(p), n(n) {}

    physx::PxVec3 p;
    physx::PxVec3 n;
};

struct Polygon {
    Polygon() = default;

    template <typename P, typename... Args>
    Polygon(P p1, P p2, P p3, Args... args) {
        AddPoints(p1, p2, p3, args...);
    }

    std::vector<Point> m_points;

    [[nodiscard]] bool IsTriangle() const { return m_points.size() == 3; }

    template <typename P, typename... Args>
    void AddPoints(P p, Args... args) {
        m_points.push_back(p);
        AddPoints(args...);
    }

    template <typename P>
    void AddPoints(P p) {
        m_points.push_back(p);
    }

    void Triangulate(std::vector<Polygon> &out) const;

    void Triangulate(std::vector<RenderVertex> &verts, std::vector<uint16_t> &indices) const;

    void TriangulateWeld(std::vector<RenderVertex> &verts,
                         std::vector<uint16_t> &indices) const;  // normalize normals afterwards
    void TriangulateForCollision(std::vector<physx::PxVec3> &verts) const;

    [[nodiscard]] physx::PxVec3 CalculateNormal() const;

    [[nodiscard]] float CalculateArea() const;

    void SubdivideTriangle(std::vector<Polygon> &out) const;

    [[nodiscard]] static bool PointPlaneSide(const physx::PxVec3 &p, const physx::PxVec4 &plane);

    void Clip(const physx::PxVec4 &plane, bool flip = false);
};

struct PolygonMesh {
    std::vector<Polygon> m_polygons;

    [[nodiscard]] bool IsTriangleMesh() const {
        bool b = true;
        for (const auto &p : m_polygons) {
            b = b && p.IsTriangle();
        }
        return b;
    }

    void AddConvexPolygon(const physx::PxVec4 &plane, physx::PxVec4 *planes, uint32_t mask, bool flip);

    void GenerateRenderBuffers(RenderVertex **vertices, uint16_t **indices, int *vertex_count, int *index_count) const;

    void GenerateSmoothRenderBuffers(RenderVertex **vertices,
                                     uint16_t **indices,
                                     int *vertex_count,
                                     int *index_count) const;

    int GenerateTriangleList(physx::PxVec3 **positions);

    void ApplyTransform(const physx::PxMat44 &transform);

    void Merge(const PolygonMesh &mesh);
};

PolygonMesh GenerateTetrahedron(float radius);

PolygonMesh GenerateIcosahedron(float radius, int subdivisions);

PolygonMesh GenerateCone(physx::PxVec4 a, physx::PxVec4 b, int segments, float grow, bool correct_cone);

PolygonMesh GenerateCollisionConvex(physx::PxVec4 *planes, uint32_t mask, float grow, bool flip);

PolygonMesh GenerateCollisionCapsules(
        physx::PxVec4 *spheres, int sphere_count, const uint32_t *indices, int index_count, float grow);

// Generates simple meshes with smooth shading
SimpleMesh GenerateFastSphere(int segments_x, int segment_y, const physx::PxMat44 &transform);
SimpleMesh GenerateFastCylinder(int segments_x, int segment_y, const physx::PxMat44 &transform);  // no caps

// Combines cashed spheres and cylinders to generate the capsules
SimpleMesh GenerateCollisionCapsulesFast(
        physx::PxVec4 *spheres, int sphere_count, const uint32_t *indices, int index_count, float grow);

uint32_t GenerateConvexPolyhedronPlanes(
        int segments_x, int segments_y, const physx::PxVec3 &center, float radius, std::vector<physx::PxVec4> *planes);

}  // namespace vox::cloth
