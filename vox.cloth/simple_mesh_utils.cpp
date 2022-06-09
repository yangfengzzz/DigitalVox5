//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.cloth/simple_mesh_utils.h"

#include "vox.cloth/foundation/PxMat44.h"
#include "vox.cloth/foundation/PxTransform.h"

namespace vox::cloth {
void Polygon::Triangulate(std::vector<Polygon> &out) const {
    for (int i = 2; i < (int)m_points.size(); i++) {
        out.emplace_back(m_points[0], m_points[i - 1], m_points[i]);
    }
}

void Polygon::Triangulate(std::vector<RenderVertex> &verts, std::vector<uint16_t> &indices) const {
    physx::PxVec3 normal = CalculateNormal();
    for (int i = 2; i < (int)m_points.size(); i++) {
        indices.push_back((uint16_t)verts.size());
        verts.emplace_back(m_points[0].p, normal);
        indices.push_back((uint16_t)verts.size());
        verts.emplace_back(m_points[i - 1].p, normal);
        indices.push_back((uint16_t)verts.size());
        verts.emplace_back(m_points[i].p, normal);
    }
}

void Polygon::TriangulateWeld(std::vector<RenderVertex> &verts, std::vector<uint16_t> &indices) const {
    auto add_vertex = [&verts](const RenderVertex &v) {
        for (int i = 0; i < (int)verts.size(); i++) {
            if ((verts[i].p - v.p).magnitudeSquared() < 0.001f) {
                return i;
            }
        }
        verts.push_back(v);
        return (uint16_t)verts.size() - 1;
    };

    physx::PxVec3 weighted_normal = CalculateNormal() * CalculateArea();

    for (int i = 2; i < (int)m_points.size(); i++) {
        indices.push_back(add_vertex(RenderVertex(m_points[0].p, weighted_normal)));
        indices.push_back(add_vertex(RenderVertex(m_points[i - 1].p, weighted_normal)));
        indices.push_back(add_vertex(RenderVertex(m_points[i].p, weighted_normal)));
    }
}

void Polygon::TriangulateForCollision(std::vector<physx::PxVec3> &verts) const {
    for (int i = 2; i < (int)m_points.size(); i++) {
        verts.push_back(m_points[0].p);
        verts.push_back(m_points[i - 1].p);
        verts.push_back(m_points[i].p);
    }
}

physx::PxVec3 Polygon::CalculateNormal() const {
    physx::PxVec3 normal(0.0f, 0.0f, 0.0f);
    for (int i = 2; i < (int)m_points.size(); i++) {
        physx::PxVec3 p[3];
        p[0] = m_points[0].p;
        p[1] = m_points[i - 1].p;
        p[2] = m_points[i].p;
        normal += (p[1] - p[0]).cross(p[2] - p[0]);
    }
    normal.normalize();
    return normal;
}

float Polygon::CalculateArea() const {
    float double_area = 0.0f;
    for (int i = 2; i < (int)m_points.size(); i++) {
        physx::PxVec3 p[3];
        p[0] = m_points[0].p;
        p[1] = m_points[i - 1].p;
        p[2] = m_points[i].p;
        double_area += (p[1] - p[0]).cross(p[2] - p[0]).magnitude();
    }
    return double_area * 0.5f;
}

void Polygon::SubdivideTriangle(std::vector<Polygon> &out) const {
    if (!IsTriangle()) return;

    for (int i = 0; i < 3; i++) {
        out.emplace_back(Point(m_points[i].p), Point(0.5f * (m_points[(i + 1) % 3].p + m_points[i].p)),
                         Point(0.5f * (m_points[(i + 2) % 3].p + m_points[i].p)));
    }
    out.emplace_back(Point(0.5f * (m_points[0].p + m_points[1].p)), Point(0.5f * (m_points[1].p + m_points[2].p)),
                     Point(0.5f * (m_points[2].p + m_points[0].p)));
}

float IntersetLinePlane(const physx::PxVec3 &a, const physx::PxVec3 &b, const physx::PxVec4 &plane) {
    physx::PxVec3 plane_normal(plane.x, plane.y, plane.z);
    float aprj = plane_normal.dot(a);
    float bprj = plane_normal.dot(b);

    return (-plane.w - aprj) / (bprj - aprj);
}

bool Polygon::PointPlaneSide(const physx::PxVec3 &p, const physx::PxVec4 &plane) {
    physx::PxVec3 plane_normal(plane.x, plane.y, plane.z);
    return p.dot(plane_normal) + plane.w < 0;
}

void Polygon::Clip(const physx::PxVec4 &plane, bool flip) {
    if (m_points.size() < 3) return;

    std::vector<Point> input = m_points;
    m_points.clear();

    Point s = input.back();
    for (auto &point_index : input) {
        if (PointPlaneSide(point_index.p, plane) != flip) {
            if (PointPlaneSide(s.p, plane) == flip) {
                float w = IntersetLinePlane(s.p, point_index.p, plane);
                m_points.push_back(s * (1.0f - w) + point_index * w);
            }
            m_points.push_back(point_index);
        } else if (PointPlaneSide(s.p, plane) != flip) {
            float w = IntersetLinePlane(s.p, point_index.p, plane);
            m_points.push_back(s * (1.0f - w) + point_index * w);
        }
        s = point_index;
    }
}

void PolygonMesh::AddConvexPolygon(const physx::PxVec4 &plane, physx::PxVec4 *planes, uint32_t mask, bool flip) {
    physx::PxVec3 t_1, t_2, normal;
    normal.x = plane.x;
    normal.y = plane.y;
    normal.z = plane.z;
    ComputeBasis(normal, &t_1, &t_2);
    Polygon poly;
    for (int i = 0; i < 4; i++) {
        float x_table[4] = {-1.0f, 1.0f, 1.0f, -1.0f};
        float y_table[4] = {-1.0f, -1.0f, 1.0f, 1.0f};
        poly.m_points.emplace_back(normal * -plane.w + 200.0f * t_1 * x_table[i] + 200.0f * t_2 * y_table[i]);
        // polyTexcoord.push_back(vec2(0.0f, 10.0f) * xTable[i] + vec2(10.0f, 0.0f) * yTable[i]);
    }

    for (int i = 0; i < 32; i++) {
        if ((1 << i) & mask) {
            const physx::PxVec4 kPl = planes[i];
            poly.Clip(kPl, flip);
        }
    }

    m_polygons.push_back(poly);
}

void PolygonMesh::GenerateRenderBuffers(RenderVertex **vertices,
                                        uint16_t **indices,
                                        int *vertex_count,
                                        int *index_count) const {
    std::vector<RenderVertex> verts;
    std::vector<uint16_t> inds;
    verts.reserve(m_polygons.size() * 3);
    verts.reserve(inds.size() * 3);

    for (auto &p : m_polygons) {
        p.Triangulate(verts, inds);
    }

    *vertices = new RenderVertex[verts.size()];
    *indices = new uint16_t[inds.size()];

    memcpy(*vertices, verts.data(), sizeof(RenderVertex) * verts.size());
    memcpy(*indices, inds.data(), sizeof(uint16_t) * inds.size());
    *vertex_count = (uint16_t)verts.size();
    *index_count = (uint16_t)inds.size();
}

void PolygonMesh::GenerateSmoothRenderBuffers(RenderVertex **vertices,
                                              uint16_t **indices,
                                              int *vertex_count,
                                              int *index_count) const {
    std::vector<RenderVertex> verts;
    std::vector<uint16_t> inds;
    verts.reserve(m_polygons.size() * 3);
    verts.reserve(inds.size() * 3);

    for (auto &p : m_polygons) {
        p.TriangulateWeld(verts, inds);
    }

    for (auto &v : verts) {
        v.n.normalize();
    }

    *vertices = new RenderVertex[verts.size()];
    *indices = new uint16_t[inds.size()];

    memcpy(*vertices, verts.data(), sizeof(RenderVertex) * verts.size());
    memcpy(*indices, inds.data(), sizeof(uint16_t) * inds.size());
    *vertex_count = (uint16_t)verts.size();
    *index_count = (uint16_t)inds.size();
}

int PolygonMesh::GenerateTriangleList(physx::PxVec3 **positions) {
    std::vector<physx::PxVec3> verts;
    verts.reserve(m_polygons.size() * 3);

    for (auto &p : m_polygons) {
        p.TriangulateForCollision(verts);
    }

    *positions = new physx::PxVec3[verts.size()];
    memcpy(*positions, verts.data(), sizeof(physx::PxVec3) * verts.size());
    return (int)verts.size();
}

void PolygonMesh::ApplyTransform(const physx::PxMat44 &transform) {
    for (auto &trig : m_polygons)
        for (auto &point : trig.m_points) {
            point.p = transform.transform(point.p);
        }
}

void PolygonMesh::Merge(const PolygonMesh &mesh) {
    m_polygons.insert(m_polygons.end(), mesh.m_polygons.begin(), mesh.m_polygons.end());
}

PolygonMesh GenerateTetrahedron(float radius) {
    PolygonMesh mesh;
    Point p[4];
    for (int i = 0; i < 3; i++) {
        p[i] = Point(radius * physx::PxVec3(cosf((float)i / 3.0f * physx::PxTwoPi),
                                            -sqrtf(2.0f / 3.0f) * 0.5f * sqrtf(3),
                                            sinf((float)i / 3.0f * physx::PxTwoPi)));
    }
    p[3] = Point(radius * physx::PxVec3(0, sqrtf(2.0f / 3.0f) * 0.5f * sqrtf(3), 0));

    mesh.m_polygons.emplace_back(p[0], p[1], p[2]);
    mesh.m_polygons.emplace_back(p[3], p[1], p[0]);
    mesh.m_polygons.emplace_back(p[3], p[2], p[1]);
    mesh.m_polygons.emplace_back(p[3], p[0], p[2]);

    return mesh;
}

PolygonMesh GenerateIcosahedron(float radius, int subdivisions) {
    PolygonMesh mesh;
    Point p[12];

    // generate positions
    float golden_ratio = (1.0f + sqrtf(5.0f)) * 0.5f;
    float scale = radius / physx::PxVec2(golden_ratio, 1.0f).magnitude();
    for (int j = 0; j < 3; j++)
        for (int i = 0; i < 4; i++) {
            float sign_a = i & 1 ? 1.0f : -1.0f;
            float sign_b = i & 2 ? -1.0f : 1.0f;
            physx::PxVec3 point(sign_a, sign_b * golden_ratio, 0.0f);
            p[i + 4 * j] = physx::PxVec3(point[j % 3], point[(j + 1) % 3], point[(j + 2) % 3]) * scale;
        }

    // generate triangles
    uint16_t ti[20 * 3] = {0, 7, 9, 0, 9, 1, 0,  1, 11, 0, 11, 6,  0,  6, 7,

                           1, 9, 5, 9, 7, 8, 7,  6, 2,  6, 11, 10, 11, 1, 4,

                           3, 5, 8, 3, 8, 2, 3,  2, 10, 3, 10, 4,  3,  4, 5,

                           8, 5, 9, 2, 8, 7, 10, 2, 6,  4, 10, 11, 5,  4, 1};

    for (int i = 0; i < 20 * 3; i += 3) {
        mesh.m_polygons.emplace_back(p[ti[i]], p[ti[i + 1]], p[ti[i + 2]]);
    }

    bool project_to_sphere = subdivisions > 0;
    while (subdivisions > 0) {
        subdivisions--;
        PolygonMesh sub;
        for (auto &trig : mesh.m_polygons) {
            trig.SubdivideTriangle(sub.m_polygons);
        }
        std::swap(sub.m_polygons, mesh.m_polygons);
    }

    if (project_to_sphere) {
        for (auto &trig : mesh.m_polygons)
            for (auto &point : trig.m_points) {
                point.p = point.p.getNormalized() * radius;
            }
    }

    return mesh;
}

namespace {
physx::PxVec3 IntersectSpheres(float *circle_radius,
                               const physx::PxVec3 &a_center,
                               float a_radius,
                               const physx::PxVec3 &b_center,
                               float b_radius) {
    // Intersect spheres in 2d (http://paulbourke.net/geometry/circlesphere/ Intersection of two circles)
    float d = (a_center - b_center).magnitude();
    float a = (a_radius * a_radius - b_radius * b_radius + d * d) / (2.0f * d);
    float h = sqrtf(a_radius * a_radius - a * a);
    physx::PxVec3 p_3 = a_center + a * (b_center - a_center) / d;
    if (circle_radius) *circle_radius = h;
    return p_3;
}
}  // namespace

PolygonMesh GenerateCone(physx::PxVec4 a, physx::PxVec4 b, int segments, float grow, bool correct_cone) {
    PolygonMesh mesh;

    if (a.w < b.w) std::swap(a, b);

    physx::PxVec3 a_center = a.getXYZ();
    physx::PxVec3 b_center = b.getXYZ();
    float a_radius = a.w + grow;
    float b_radius = b.w + grow;

    physx::PxVec3 basis[3];
    basis[2] = b_center - a_center;
    basis[2].normalize();
    ComputeBasis(basis[2], &basis[0], &basis[1]);

    if (correct_cone) {
        // make the cone connect seamlessly to the spheres
        {
            // http://jwilson.coe.uga.edu/emt669/Student.Folders/Kertscher.Jeff/Essay.3/Tangents.html

            // sphere a with smaller radius
            float c_radius = a_radius - b_radius;
            if (c_radius > 0.00001) {
                physx::PxVec3 c_center = a_center;

                // sphere in between the a and b
                physx::PxVec3 d_center = (a_center + b_center) * 0.5f;
                float d_radius = (a_center - b_center).magnitude() * 0.5f;

                // intersection between c and d to get tangent point
                float i_radius;
                physx::PxVec3 i_center = IntersectSpheres(&i_radius, d_center, d_radius, c_center, c_radius);
                physx::PxVec3 i_point = i_center + basis[0] * i_radius;       // tangent point on c
                physx::PxVec3 offset = (i_point - a_center).getNormalized();  // offset direction

                physx::PxVec3 a_point = a_center + offset * a_radius;
                a_center = (a_point - a_center).dot(basis[2]) * basis[2] + a_center;
                a_radius = (a_point - a_center).magnitude();
                physx::PxVec3 b_point = b_center + offset * b_radius;
                b_center = (b_point - a_center).dot(basis[2]) * basis[2] + a_center;
                b_radius = (b_point - b_center).magnitude();
            }
        }
    }

    for (int i = 0; i < segments; i++) {
        float angle_1 = (float)i / (float)segments * physx::PxTwoPi;
        float angle_2 = (float)(i + 1) / (float)segments * physx::PxTwoPi;

        Polygon p;
        p.AddPoints(Point(a_center + (cosf(angle_1) * basis[0] + sinf(angle_1) * basis[1]) * a_radius));
        p.AddPoints(Point(a_center + (cosf(angle_2) * basis[0] + sinf(angle_2) * basis[1]) * a_radius));
        p.AddPoints(Point(b_center + (cosf(angle_2) * basis[0] + sinf(angle_2) * basis[1]) * b_radius));
        p.AddPoints(Point(b_center + (cosf(angle_1) * basis[0] + sinf(angle_1) * basis[1]) * b_radius));

        mesh.m_polygons.push_back(p);
    }

    return mesh;
}

PolygonMesh GenerateCollisionConvex(physx::PxVec4 *planes, uint32_t mask, float grow, bool flip) {
    PolygonMesh mesh;
    if (grow != 0.0f) {
        auto *grown_planes = new physx::PxVec4[32];
        for (int i = 0; i < 32; i++) {
            if ((1 << i) & mask) {
                grown_planes[i] = planes[i];
                grown_planes[i].w -= grow;
            }
        }
        planes = grown_planes;
    }

    for (int i = 0; i < 32; i++) {
        if ((1 << i) & mask) mesh.AddConvexPolygon(planes[i], planes, mask ^ (1 << i), flip);
    }

    if (grow != 0.0f) delete[] planes;

    return mesh;
}

PolygonMesh GenerateCollisionCapsules(
        physx::PxVec4 *spheres, int sphere_count, const uint32_t *indices, int index_count, float grow) {
    PolygonMesh final_mesh;
    for (int i = 0; i < sphere_count; i++) {
        PolygonMesh sphere = GenerateIcosahedron(spheres[i].w + grow, 2);
        sphere.ApplyTransform(physx::PxTransform(spheres[i].getXYZ()));
        final_mesh.Merge(sphere);
    }

    for (int i = 0; i < index_count; i += 2) {
        final_mesh.Merge(GenerateCone(spheres[indices[i]], spheres[indices[i + 1]], 32, grow, true));
    }

    return final_mesh;
}

SimpleMesh GenerateFastSphere(int segments_x, int segment_y, const physx::PxMat44 &transform) {
    SimpleMesh mesh;
    const int kXSegments = segments_x;
    const int kYSegments = segment_y;

    {
        // bottom
        SimpleMesh::Vertex v;
        v.position = physx::PxVec3(0.0f, -1.0f, 0.0f);
        v.normal = transform.rotate(physx::PxVec4(v.position, 0.0f)).getXYZ();
        v.position = transform.transform(v.position);
        v.uv = physx::PxVec2(0.0f, 0.0f);
        mesh.vertices_.push_back(v);
    }

    // middle
    for (int y = 1; y < kYSegments; y++) {
        for (int x = 0; x < kXSegments; x++) {
            float xf = (float)x / (kXSegments - 1.0f);
            float yaw = xf * physx::PxTwoPi;
            float yf = (float)y / (kYSegments);
            float pitch = (yf - 0.5f) * physx::PxPi;

            SimpleMesh::Vertex v;
            v.position = physx::PxVec3(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch));
            v.uv = physx::PxVec2(xf, yf);
            v.normal = transform.rotate(physx::PxVec4(v.position, 0.0f)).getXYZ();
            v.position = transform.transform(v.position);
            mesh.vertices_.push_back(v);
        }
    }

    {
        // top
        SimpleMesh::Vertex v;
        v.position = physx::PxVec3(0.0f, 1.0f, 0.0f);
        v.normal = transform.rotate(physx::PxVec4(v.position, 0.0f)).getXYZ();
        v.position = transform.transform(v.position);
        v.uv = physx::PxVec2(0.0f, 1.0f);
        mesh.vertices_.push_back(v);
    }

    // bottom cap
    for (int x = 0; x < kXSegments; x++) {
        mesh.indices_.push_back(0);
        mesh.indices_.push_back(1 + x);
        mesh.indices_.push_back(1 + (x + 1) % kXSegments);
    }

    const auto kRingVertex = [kXSegments](int x, int y) { return 1 + y * kXSegments + x % kXSegments; };

    // middle
    for (int y = 0; y < kYSegments - 2; y++) {
        for (int x = 0; x < kXSegments; x++) {
            mesh.indices_.push_back(kRingVertex(x, y));
            mesh.indices_.push_back(kRingVertex(x + 1, y));
            mesh.indices_.push_back(kRingVertex(x, y + 1));

            mesh.indices_.push_back(kRingVertex(x + 1, y));
            mesh.indices_.push_back(kRingVertex(x + 1, y + 1));
            mesh.indices_.push_back(kRingVertex(x, y + 1));
        }
    }

    // bottom cap
    for (int x = 0; x < kXSegments; x++) {
        mesh.indices_.push_back((uint16_t)mesh.vertices_.size() - 1);
        mesh.indices_.push_back(kRingVertex(x, kYSegments - 2));
        mesh.indices_.push_back(kRingVertex(x + 1, kYSegments - 2));
    }

    return mesh;
}

SimpleMesh GenerateFastCylinder(int segments_x, int segment_y, const physx::PxMat44 &transform) {
    SimpleMesh mesh;
    const int kXSegments = segments_x;
    const int kYSegments = segment_y;

    // middle
    for (int y = 0; y < kYSegments + 1; y++) {
        for (int x = 0; x < kXSegments; x++) {
            float xf = (float)x / (kXSegments - 1.0f);
            float yaw = xf * physx::PxTwoPi;
            float yf = (float)y / (kYSegments)*2.0f - 1.0f;

            SimpleMesh::Vertex v;
            v.position = physx::PxVec3(cos(yaw), yf, sin(yaw));
            v.uv = physx::PxVec2(xf, yf);
            v.normal = transform.rotate(physx::PxVec4(physx::PxVec3(cos(yaw), 0.0f, sin(yaw)), 0.0f)).getXYZ();
            v.position = transform.transform(v.position);
            mesh.vertices_.push_back(v);
        }
    }

    const auto kRingVertex = [kXSegments](int x, int y) { return y * kXSegments + x % kXSegments; };

    // middle
    for (int y = 0; y < kYSegments; y++) {
        for (int x = 0; x < kXSegments; x++) {
            mesh.indices_.push_back(kRingVertex(x, y));
            mesh.indices_.push_back(kRingVertex(x + 1, y));
            mesh.indices_.push_back(kRingVertex(x, y + 1));

            mesh.indices_.push_back(kRingVertex(x + 1, y));
            mesh.indices_.push_back(kRingVertex(x + 1, y + 1));
            mesh.indices_.push_back(kRingVertex(x, y + 1));
        }
    }

    return mesh;
}

SimpleMesh GenerateCollisionCapsulesFast(
        physx::PxVec4 *spheres, int sphere_count, const uint32_t *indices, int index_count, float grow) {
    static SimpleMesh sphere = GenerateFastSphere(
            24, 12,
            physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f), physx::PxQuat(0.0f, physx::PxVec3(0.0f, 1.0f, 0.0f))));
    static SimpleMesh cylinder = GenerateFastCylinder(
            24, 1,
            physx::PxTransform(physx::PxVec3(0.0f, 1.0f, 0.0f), physx::PxQuat(0.0f, physx::PxVec3(0.0f, 1.0f, 0.0f))));

    SimpleMesh mesh;
    mesh.vertices_.resize(sphere.vertices_.size() * sphere_count + cylinder.vertices_.size() * (index_count / 2));
    mesh.indices_.resize(sphere.indices_.size() * sphere_count + cylinder.indices_.size() * (index_count / 2));

    int next_vertex = 0;
    int next_index = 0;
    for (int i = 0; i < sphere_count; i++) {
        int base_index = next_vertex;
        physx::PxMat44 transform =
                physx::PxMat44(physx::PxMat33(physx::PxIdentity), spheres[i].getXYZ()) *
                physx::PxMat44(physx::PxVec4(spheres[i].w + grow, spheres[i].w + grow, spheres[i].w + grow, 1.0f));

        for (auto v : sphere.vertices_) {
            v.normal = transform.rotate(physx::PxVec4(v.normal, 0.0f)).getXYZ();
            v.position = transform.transform(v.position);
            mesh.vertices_[next_vertex++] = v;
        }

        for (unsigned short indice : sphere.indices_) {
            mesh.indices_[next_index++] = indice + base_index;
        }
    }

    for (int i = 0; i < index_count; i += 2) {
        int base_index = next_vertex;

        physx::PxVec3 sphere_pos_a = spheres[indices[i]].getXYZ();
        physx::PxVec3 sphere_pos_b = spheres[indices[i + 1]].getXYZ();
        float sphere_radius_a = spheres[indices[i]].w + grow;
        float sphere_radius_b = spheres[indices[i + 1]].w + grow;

        if (sphere_radius_a < sphere_radius_b) {
            std::swap(sphere_radius_a, sphere_radius_b);
            std::swap(sphere_pos_a, sphere_pos_b);
        }

        {
            // http://jwilson.coe.uga.edu/emt669/Student.Folders/Kertscher.Jeff/Essay.3/Tangents.html

            // sphere a with smaller radius
            float c_radius = sphere_radius_a - sphere_radius_b;
            if (c_radius > 0.00001) {
                physx::PxVec3 basis[3];
                basis[2] = sphere_pos_b - sphere_pos_a;
                basis[2].normalize();
                ComputeBasis(basis[2], &basis[0], &basis[1]);

                physx::PxVec3 c_center = sphere_pos_a;

                // sphere in between the a and b
                physx::PxVec3 d_center = (sphere_pos_a + sphere_pos_b) * 0.5f;
                float d_radius = (sphere_pos_a - sphere_pos_b).magnitude() * 0.5f;

                // intersection between c and d to get tangent point
                float i_radius;
                physx::PxVec3 i_center = IntersectSpheres(&i_radius, d_center, d_radius, c_center, c_radius);
                physx::PxVec3 i_point = i_center + basis[0] * i_radius;           // tangent point on c
                physx::PxVec3 offset = (i_point - sphere_pos_a).getNormalized();  // offset direction

                physx::PxVec3 a_point = sphere_pos_a + offset * sphere_radius_a;
                sphere_pos_a = (a_point - sphere_pos_a).dot(basis[2]) * basis[2] + sphere_pos_a;
                sphere_radius_a = (a_point - sphere_pos_a).magnitude();
                physx::PxVec3 b_point = sphere_pos_b + offset * sphere_radius_b;
                sphere_pos_b = (b_point - sphere_pos_a).dot(basis[2]) * basis[2] + sphere_pos_a;
                sphere_radius_b = (b_point - sphere_pos_b).magnitude();
            }
        }

        float length = (sphere_pos_b - sphere_pos_a).magnitude();

        physx::PxMat44 scale_a =
                physx::PxMat44(physx::PxVec4(sphere_radius_a, length / 2.0f, sphere_radius_a + grow, 1.0f));
        physx::PxMat44 scale_b = physx::PxMat44(physx::PxVec4(sphere_radius_b, length / 2.0f, sphere_radius_b, 1.0f));

        physx::PxQuat orientation;
        {
            physx::PxVec3 u = physx::PxVec3(0.0f, 1.0f, 0.0f);
            physx::PxVec3 v = sphere_pos_b - sphere_pos_a;
            v.normalize();

            if (u.dot(v) < -0.9999)
                orientation = physx::PxQuat(physx::PxTwoPi, physx::PxVec3(1.0f, 0.0f, 0.0f));
            else if (u.dot(v) > 0.9999)
                orientation = physx::PxQuat(0.0f, physx::PxVec3(1.0f, 0.0f, 0.0f));
            else {
                physx::PxVec3 half = u + v;
                half.normalize();
                physx::PxVec3 imaginary = u.cross(half);
                orientation = physx::PxQuat(imaginary.x, imaginary.y, imaginary.z, u.dot(half));
            }
        }

        physx::PxMat44 transform = physx::PxMat44(physx::PxTransform(sphere_pos_a, orientation)) * scale_a;

        int first_ring = (int)cylinder.vertices_.size() / 2;
        for (int vi = 0; vi < first_ring; vi++) {
            SimpleMesh::Vertex v = cylinder.vertices_[vi];
            v.normal = transform.rotate(physx::PxVec4(v.normal, 0.0f)).getXYZ();
            v.position = transform.transform(v.position);
            mesh.vertices_[next_vertex++] = v;
        }
        transform = physx::PxMat44(physx::PxTransform(sphere_pos_a, orientation)) * scale_b;
        for (int vi = first_ring; vi < (int)cylinder.vertices_.size(); vi++) {
            SimpleMesh::Vertex v = cylinder.vertices_[vi];
            v.normal = transform.rotate(physx::PxVec4(v.normal, 0.0f)).getXYZ();
            v.position = transform.transform(v.position);
            mesh.vertices_[next_vertex++] = v;
        }

        for (unsigned short indice : cylinder.indices_) {
            mesh.indices_[next_index++] = indice + base_index;
        }
    }

    return mesh;
}

uint32_t GenerateConvexPolyhedronPlanes(
        int segments_x, int segments_y, const physx::PxVec3 &center, float radius, std::vector<physx::PxVec4> *planes) {
    int offset = 0;
    if (planes) {
        planes->reserve(planes->size() + segments_x * segments_y);
        offset = (int)planes->size();
    }

    segments_y += 1;
    for (int i = 1; i < segments_y; i++) {
        float angle_y = (float)i / (float)segments_y * physx::PxPi + physx::PxPiDivTwo;
        for (int j = 0; j < segments_x; j++) {
            float angle_x = (float)j / (float)segments_x * physx::PxTwoPi;

            physx::PxVec3 nx(cosf(angle_x), 0.0f, sinf(angle_x));
            physx::PxVec3 n = cosf(angle_y) * nx + sinf(angle_y) * physx::PxVec3(0.0f, 1.0f, 0.0f);

            physx::PxVec3 p = n * radius + center;

            if (planes) planes->push_back(ConstructPlaneFromPointNormal(p, n));
        }
    }
    uint64_t shift = (segments_x * (segments_y - 1) + offset);
    uint64_t exclude_mask = (((uint64_t)1 << offset) - 1);
    uint64_t mask = (((uint64_t)1 << shift) - 1) & ~exclude_mask;
    return static_cast<uint32_t>(mask);
}

}  // namespace vox::cloth
