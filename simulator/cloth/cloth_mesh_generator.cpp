//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simulator/cloth/cloth_mesh_generator.h"

#include <fstream>

namespace vox::cloth {

namespace {
template <typename T>
std::vector<T> ReadValuesFromFile(const std::string &path) {
    std::ifstream input_file(path);
    std::vector<T> data{std::istream_iterator<T>{input_file}, {}};
    return std::move(data);
}
}  // end of anonymous namespace

void ClothMeshData::Clear() {
    m_vertices.clear();
    m_triangles.clear();
    m_quads.clear();
}

void ClothMeshData::GeneratePlaneCloth(float width,
                                       float height,
                                       int segments_x,
                                       int segments_y,
                                       bool create_quads,
                                       const physx::PxMat44 &transform,
                                       bool alternating_diagonals,
                                       int zigzag) {
    /*
     GeneratePlaneCloth(x,y,2,2) generates:

     v0______v1_____v2     v0______v1_____v2
     |      |      |       |\     |\     |
     |  Q0  |  Q1  |       |  \t0 |  \t2 |
     |      |      |       | t1 \ | t3 \ |
     v3------v4-----v5     v3-----\v4----\v5
     |      |      |       | \    | \    |
     |  Q2  |  Q3  |       |   \t4|   \t6|
     |______|______|       |_t5_\_|_t7__\|
     v6      v7     v8     v6      v7     v8
     */

    Clear();
    m_vertices.resize((segments_x + 1) * (segments_y + 1));
    m_inv_masses.resize((segments_x + 1) * (segments_y + 1));
    m_triangles.resize(segments_x * segments_y * 2);
    if (create_quads) m_quads.resize(segments_x * segments_y);

    m_mesh.vertices_.resize(m_vertices.size());
    m_mesh.indices_.resize(3 * m_triangles.size());

    physx::PxVec3 top_left(-width * 0.5f, 0.f, -height * 0.5f);
    //    vec3 topLeftGLM(-width * 0.5f, translateUp, -height * 0.5f);

    // calculate uv scale and offset to keep texture aspect ratio 1:1
    float uv_sx = width > height ? 1.0f : width / height;
    float uv_sy = width > height ? height / width : 1.0f;
    float uv_ox = 0.5f * (1.0f - uv_sx);
    float uv_oy = 0.5f * (1.0f - uv_sy);

    // Vertices
    for (int y = 0; y < segments_y + 1; y++) {
        for (int x = 0; x < segments_x + 1; x++) {
            physx::PxVec3 pos;
            switch (zigzag) {
                case 1:
                    pos = physx::PxVec3(((float)x / (float)segments_x) * width,
                                        sinf(y * 0.5) / (float)segments_y * height,
                                        ((float)y / (float)segments_y) * height);
                    break;
                case 2:
                    pos = physx::PxVec3(((float)x / (float)segments_x) * width,
                                        ((float)(y & 2) / (float)segments_y) * height,
                                        ((float)((y + 1) & ~1) / (float)segments_y) * height);
                    break;
                default:
                    pos = physx::PxVec3(((float)x / (float)segments_x) * width, 0.f,
                                        ((float)y / (float)segments_y) * height);
            }

            m_vertices[x + y * (segments_x + 1)] = transform.transform(top_left + pos);

            m_inv_masses[x + y * (segments_x + 1)] = 1.0f;

            m_mesh.vertices_[x + y * (segments_x + 1)].position = transform.transform(top_left + pos);

            m_mesh.vertices_[x + y * (segments_x + 1)].normal = transform.transform(physx::PxVec3(0.f, 1.f, 0.f));

            m_mesh.vertices_[x + y * (segments_x + 1)].uv =
                    physx::PxVec2(uv_ox + uv_sx * (float)x / (float)segments_x,
                                  uv_oy + uv_sy * (1.0f - (float)y / (float)segments_y));
        }
    }

    if (create_quads) {
        // Quads
        for (int y = 0; y < segments_y; y++) {
            for (int x = 0; x < segments_x; x++) {
                m_quads[(x + y * segments_x)] = Quad(
                        (uint32_t)(x + 0) + (y + 0) * (segments_x + 1), (uint32_t)(x + 1) + (y + 0) * (segments_x + 1),
                        (uint32_t)(x + 1) + (y + 1) * (segments_x + 1), (uint32_t)(x + 0) + (y + 1) * (segments_x + 1));
            }
        }
    }

    // Triangles
    for (int y = 0; y < segments_y; y++) {
        for (int x = 0; x < segments_x; x++) {
            if (alternating_diagonals && (x ^ y) & 1) {
                // Top right to bottom left
                m_triangles[(x + y * segments_x) * 2 + 0] = Triangle((uint32_t)(x + 0) + (y + 0) * (segments_x + 1),
                                                                     (uint32_t)(x + 1) + (y + 0) * (segments_x + 1),
                                                                     (uint32_t)(x + 0) + (y + 1) * (segments_x + 1));

                m_triangles[(x + y * segments_x) * 2 + 1] = Triangle((uint32_t)(x + 1) + (y + 0) * (segments_x + 1),
                                                                     (uint32_t)(x + 1) + (y + 1) * (segments_x + 1),
                                                                     (uint32_t)(x + 0) + (y + 1) * (segments_x + 1));
            } else {
                // Top left to bottom right
                m_triangles[(x + y * segments_x) * 2 + 0] = Triangle((uint32_t)(x + 0) + (y + 0) * (segments_x + 1),
                                                                     (uint32_t)(x + 1) + (y + 0) * (segments_x + 1),
                                                                     (uint32_t)(x + 1) + (y + 1) * (segments_x + 1));

                m_triangles[(x + y * segments_x) * 2 + 1] = Triangle((uint32_t)(x + 0) + (y + 0) * (segments_x + 1),
                                                                     (uint32_t)(x + 1) + (y + 1) * (segments_x + 1),
                                                                     (uint32_t)(x + 0) + (y + 1) * (segments_x + 1));
            }
        }
    }

    for (int i = 0; i < (int)m_triangles.size(); i++) {
        m_mesh.indices_[3 * i] = m_triangles[i].a;
        m_mesh.indices_[3 * i + 1] = m_triangles[i].b;
        m_mesh.indices_[3 * i + 2] = m_triangles[i].c;
    }
}

void ClothMeshData::GenerateCylinderWave(float radius_top,
                                         float radius_bottom,
                                         float height,
                                         float frequency,
                                         float ampitude_top,
                                         float ampitude_bottom,
                                         int segments_x,
                                         int segments_y,
                                         const physx::PxMat44 &transform,
                                         bool attach_top,
                                         bool attach_bottom,
                                         bool create_quads,
                                         int missing_x_segments) {
    Clear();
    int particle_x_segments = segments_x - std::max(0, missing_x_segments - 1);
    int triangle_x_segments = segments_x - missing_x_segments;
    assert(missing_x_segments < segments_x);
    m_vertices.resize((particle_x_segments + 0) * (segments_y + 1));
    m_inv_masses.resize((particle_x_segments + 0) * (segments_y + 1));
    m_triangles.resize(triangle_x_segments * segments_y * 2);
    if (create_quads) m_quads.resize(triangle_x_segments * segments_y);

    m_mesh.vertices_.resize(m_vertices.size());
    m_mesh.indices_.resize(3 * m_triangles.size());

    float slope_x;
    float slope_y;
    {
        float y = height;
        float x = radius_bottom - radius_top;
        float l = sqrtf(x * x + y * y);
        slope_y = x / l;
        slope_x = y / l;
    }

    // Vertices
    for (int y = 0; y < segments_y + 1; y++) {
        float h = height - (float)y / (float)segments_y * height - 0.5f * height;
        float ynorm = (float)y / (float)(segments_y - 1);
        float w = ynorm;
        float r = radius_bottom * w + (1.0f - w) * radius_top;
        for (int x = 0; x < particle_x_segments; x++) {
            float theta = (float)x / (float)segments_x * physx::PxTwoPi;
            float rw = r + cosf(frequency * theta) * (ampitude_bottom * w + (1.0f - w) * ampitude_top);
            m_vertices[x + y * particle_x_segments] =
                    transform.transform(physx::PxVec3(sinf(theta) * rw, h, cosf(theta) * rw));
            m_inv_masses[x + y * particle_x_segments] =
                    ((y == 0 && attach_top) || y == segments_y && attach_bottom) ? 0.0f : 1.0f;

            m_mesh.vertices_[x + y * particle_x_segments].position = m_vertices[x + y * particle_x_segments];
            m_mesh.vertices_[x + y * particle_x_segments].uv =
                    physx::PxVec2((float)x / (float)particle_x_segments, (float)y / (float)segments_y);
            // Not the correct normal, but we recalculate it anyway when updating the cloth mesh
            m_mesh.vertices_[x + y * particle_x_segments].normal =
                    physx::PxVec3(cosf(theta) * slope_x, slope_y, -sinf(theta) * slope_x);
        }
    }

    if (create_quads) {
        // Quads
        for (int y = 0; y < segments_y; y++) {
            for (int x = 0; x < triangle_x_segments; x++) {
                m_quads[(x + y * triangle_x_segments)] =
                        Quad((uint32_t)(x + 0) + (y + 0) * (particle_x_segments),
                             (uint32_t)((x + 1) % particle_x_segments) + (y + 0) * (particle_x_segments),
                             (uint32_t)((x + 1) % particle_x_segments) + (y + 1) * (particle_x_segments),
                             (uint32_t)((x + 0) % particle_x_segments) + (y + 1) * (particle_x_segments));
            }
        }
    }

    // Triangles
    for (int y = 0; y < segments_y; y++) {
        for (int x = 0; x < triangle_x_segments; x++) {
            m_triangles[(x + y * triangle_x_segments) * 2 + 0] =
                    Triangle((uint32_t)((x + 1) % particle_x_segments) + (y + 1) * (particle_x_segments),
                             (uint32_t)((x + 1) % particle_x_segments) + (y + 0) * (particle_x_segments),
                             (uint32_t)(x + 0) + (y + 0) * (particle_x_segments));

            m_triangles[(x + y * triangle_x_segments) * 2 + 1] =
                    Triangle((uint32_t)((x + 0) % particle_x_segments) + (y + 1) * (particle_x_segments),
                             (uint32_t)((x + 1) % particle_x_segments) + (y + 1) * (particle_x_segments),
                             (uint32_t)(x + 0) + (y + 0) * (particle_x_segments));
        }
    }

    for (int i = 0; i < (int)m_triangles.size(); i++) {
        m_mesh.indices_[3 * i + 0] = m_triangles[i].a;
        m_mesh.indices_[3 * i + 1] = m_triangles[i].b;
        m_mesh.indices_[3 * i + 2] = m_triangles[i].c;
    }
}

void ClothMeshData::AttachClothPlaneByAngles(int segments_x, int segments_y, bool attach_by_width) {
    for (int y = 0; y < segments_y + 1; y++)
        for (int x = 0; x < segments_x + 1; x++)
            if ((attach_by_width && y == 0) || (!attach_by_width && x == 0))
                if (x == 0 || x == segments_x) m_inv_masses[x + y * (segments_x + 1)] = 0.0f;
}

void ClothMeshData::AttachClothPlaneBySide(int segments_x, int segments_y, bool attach_by_width) {
    for (int y = 0; y < segments_y + 1; y++)
        for (int x = 0; x < segments_x + 1; x++)
            if ((attach_by_width && y == 0) || (!attach_by_width && x == 0))
                m_inv_masses[x + y * (segments_x + 1)] = 0.0f;
}

void ClothMeshData::AttachClothUsingTopVertices(float threshold_y) {
    int top_vertex_index = -1;
    physx::PxVec3 top_vertex(-1e30f, -1e30f, -1e30f);

    for (int i = 0; i < (int)m_vertices.size(); ++i) {
        if (m_vertices[i].y > top_vertex.y) {
            top_vertex = m_vertices[i];
            top_vertex_index = i;
        }
    }
    NV_CLOTH_ASSERT(top_vertex_index >= 0);

    for (int i = 0; i < (int)m_vertices.size(); ++i) {
        if (top_vertex.y - m_vertices[i].y < threshold_y) {
            m_inv_masses[i] = 0.0f;
        }
    }
}

bool ClothMeshData::ReadClothFromFile(const std::string &vertices_path,
                                      const std::string &indices_path,
                                      const physx::PxMat44 &transform) {
    std::vector<float> vertices_xyz = ReadValuesFromFile<float>(vertices_path);
    std::vector<uint32_t> indices = ReadValuesFromFile<uint32_t>(indices_path);

    if (vertices_xyz.size() < 3 * 3 || indices.size() < 3) return false;

    return InitializeFromData<float, uint32_t>(ToBoundedData(vertices_xyz), ToBoundedData(indices), transform);
}

template <typename PositionType, typename IndexType>
bool ClothMeshData::InitializeFromData(nv::cloth::BoundedData positions,
                                       nv::cloth::BoundedData indices,
                                       const physx::PxMat44 &transform) {
    if (positions.count < 3 || indices.count < 3) return false;

    NV_CLOTH_ASSERT(sizeof(PositionType) != sizeof(physx::PxVec3) || positions.count % 3 == 0);
    NV_CLOTH_ASSERT(indices.count % 3 == 0);

    auto num_vertices = (sizeof(PositionType) == sizeof(physx::PxVec3)) ? positions.count : positions.count / 3;
    const auto kNumTriangles = indices.count / 3;

    Clear();
    m_vertices.resize(num_vertices);
    m_inv_masses.resize(num_vertices);
    m_triangles.resize(kNumTriangles);

    // Quads not supported yet
    // mQuads.resize(numTriangles / 2);

    m_mesh.vertices_.resize(m_vertices.size());
    m_mesh.indices_.resize(3 * m_triangles.size());

    for (int i = 0; i < (int)num_vertices; ++i) {
        physx::PxVec3 pos;
        if (sizeof(PositionType) == sizeof(physx::PxVec3))
            pos = positions.at<physx::PxVec3>(i);
        else
            pos = physx::PxVec3(positions.at<float>(i * 3), positions.at<float>(i * 3 + 1),
                                positions.at<float>(i * 3 + 2));

        pos = transform.transform(pos);

        m_vertices[i] = pos;
        m_inv_masses[i] = 1.0f;

        m_mesh.vertices_[i].position = pos;
        m_mesh.vertices_[i].normal = transform.transform(physx::PxVec3(0.f, 1.f, 0.f));  // TODO
        m_mesh.vertices_[i].uv = physx::PxVec2(0.0f, 0.0f);                              // TODO
    }

    for (int i = 0; i < (int)kNumTriangles; ++i) {
        m_triangles[i] = Triangle(indices.at<IndexType>(i * 3), indices.at<IndexType>(i * 3 + 1),
                                  indices.at<IndexType>(i * 3 + 2));
    }

    for (int i = 0; i < (int)kNumTriangles; i++) {
        m_mesh.indices_[3 * i + 0] = m_triangles[i].a;
        m_mesh.indices_[3 * i + 1] = m_triangles[i].b;
        m_mesh.indices_[3 * i + 2] = m_triangles[i].c;
    }

    return true;
}

template bool ClothMeshData::InitializeFromData<float, uint16_t>(nv::cloth::BoundedData positions,
                                                                 nv::cloth::BoundedData indices,
                                                                 const physx::PxMat44 &transform);

template bool ClothMeshData::InitializeFromData<float, uint32_t>(nv::cloth::BoundedData positions,
                                                                 nv::cloth::BoundedData indices,
                                                                 const physx::PxMat44 &transform);

template bool ClothMeshData::InitializeFromData<physx::PxVec3, uint16_t>(nv::cloth::BoundedData positions,
                                                                         nv::cloth::BoundedData indices,
                                                                         const physx::PxMat44 &transform);

template bool ClothMeshData::InitializeFromData<physx::PxVec3, uint32_t>(nv::cloth::BoundedData positions,
                                                                         nv::cloth::BoundedData indices,
                                                                         const physx::PxMat44 &transform);

void ClothMeshData::SetInvMasses(float inv_mass) {
    // Doesn't modify attached vertices
    for (float &m_inv_masse : m_inv_masses)
        if (m_inv_masse > 1e-6f) m_inv_masse = inv_mass;
}

void ClothMeshData::SetInvMassesFromDensity(float density) {
    // Tempt code, should take into account triangle's areas
    // Doesn't modify attached vertices
    for (float &m_inv_masse : m_inv_masses)
        if (m_inv_masse > 1e-6f) m_inv_masse = 1.f / density;
}

nv::cloth::ClothMeshDesc ClothMeshData::GetClothMeshDesc() {
    nv::cloth::ClothMeshDesc d;
    d.setToDefault();
    d.points = ToBoundedData(m_vertices);
    if (!m_quads.empty()) d.quads = ToBoundedData(m_quads);
    if (!m_triangles.empty()) d.triangles = ToBoundedData(m_triangles);
    d.invMasses = ToBoundedData(m_inv_masses);

    return d;
}

SimpleMesh ClothMeshData::GetRenderMesh() const { return m_mesh; }

void ClothMeshData::Merge(const ClothMeshData &other) {
    auto first_vertex = (uint32_t)m_vertices.size();
    auto first_triangle = (uint32_t)m_triangles.size();
    auto first_quad = (uint32_t)m_quads.size();

    m_vertices.insert(m_vertices.end(), other.m_vertices.begin(), other.m_vertices.end());
    m_uvs.insert(m_uvs.end(), other.m_uvs.begin(), other.m_uvs.end());
    m_inv_masses.insert(m_inv_masses.end(), other.m_inv_masses.begin(), other.m_inv_masses.end());

    m_mesh.vertices_.insert(m_mesh.vertices_.end(), other.m_mesh.vertices_.begin(), other.m_mesh.vertices_.end());

    for (const auto &t : other.m_triangles) {
        m_triangles.push_back(t + first_vertex);
    }
    for (const auto &q : other.m_quads) {
        m_quads.push_back(q + first_vertex);
        m_mesh.indices_.push_back(m_quads.back().a);
        m_mesh.indices_.push_back(m_quads.back().b);
        m_mesh.indices_.push_back(m_quads.back().c);
    }
}

}  // namespace vox::cloth
