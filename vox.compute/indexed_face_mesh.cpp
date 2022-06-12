//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/indexed_face_mesh.h"

namespace vox::compute {

IndexedFaceMesh& IndexedFaceMesh::operator=(IndexedFaceMesh const& other) {
    m_num_points_ = other.m_num_points_;
    m_indices_ = other.m_indices_;
    m_edges_ = other.m_edges_;
    m_faces_edges_ = other.m_faces_edges_;
    m_closed_ = other.m_closed_;
    m_uv_indices_ = other.m_uv_indices_;
    m_uvs_ = other.m_uvs_;
    m_normals_ = other.m_normals_;
    m_vertex_normals_ = other.m_vertex_normals_;

    for (size_t i(0u); i < m_faces_edges_.size(); ++i) {
        m_faces_edges_[i].resize(m_vertices_per_face_);
#if defined(_MSC_VER)
        std::copy(other.m_facesEdges[i].data(), other.m_facesEdges[i].data() + m_verticesPerFace,
                  stdext::unchecked_array_iterator<unsigned int*>(m_facesEdges[i].data()));
#else
        std::copy(other.m_faces_edges_[i].data(), other.m_faces_edges_[i].data() + m_vertices_per_face_,
                  m_faces_edges_[i].data());
#endif
    }

    m_vertices_edges_.resize(other.m_vertices_edges_.size());
    for (size_t i(0u); i < m_vertices_edges_.size(); ++i) m_vertices_edges_[i] = other.m_vertices_edges_[i];

    m_vertices_faces_.resize(other.m_vertices_faces_.size());
    for (size_t i(0u); i < m_vertices_faces_.size(); ++i) m_vertices_faces_[i] = other.m_vertices_faces_[i];

    return *this;
}

IndexedFaceMesh::IndexedFaceMesh(IndexedFaceMesh const& other) { *this = other; }

IndexedFaceMesh::IndexedFaceMesh() {
    m_closed_ = false;
    m_flat_shading_ = false;
}

IndexedFaceMesh::~IndexedFaceMesh() { Release(); }

bool IndexedFaceMesh::IsClosed() const { return m_closed_; }

void IndexedFaceMesh::InitMesh(const unsigned int n_points, const unsigned int n_edges, const unsigned int n_faces) {
    m_num_points_ = n_points;
    m_indices_.reserve(n_faces * m_vertices_per_face_);
    m_edges_.reserve(n_edges);
    m_faces_edges_.reserve(n_faces);
    m_uv_indices_.reserve(n_faces);
    m_uvs_.reserve(n_points);
    m_vertices_faces_.reserve(n_points);
    m_vertices_edges_.reserve(n_points);
    m_normals_.reserve(n_faces);
    m_vertex_normals_.reserve(n_points);
}

void IndexedFaceMesh::Release() {
    m_indices_.clear();
    m_edges_.clear();
    m_faces_edges_.clear();
    m_faces_edges_.clear();
    m_uv_indices_.clear();
    m_uvs_.clear();
    m_vertices_faces_.clear();
    m_vertices_edges_.clear();
    m_normals_.clear();
    m_vertex_normals_.clear();
}

/** Add a new face. Indices must be an array of size m_verticesPerFace.
 */
void IndexedFaceMesh::AddFace(const unsigned int* const indices) {
    for (unsigned int i = 0u; i < m_vertices_per_face_; i++) m_indices_.push_back(indices[i]);
}

/** Add a new face. Indices must be an array of size m_verticesPerFace.
 */
void IndexedFaceMesh::AddFace(const int* const indices) {
    for (unsigned int i = 0u; i < m_vertices_per_face_; i++) m_indices_.push_back((unsigned int)indices[i]);
}

void IndexedFaceMesh::AddUv(Real u, Real v) {
    Vector2r uv;
    uv[0] = u;
    uv[1] = v;
    m_uvs_.push_back(uv);
}

void IndexedFaceMesh::AddUvIndex(const unsigned int index) { m_uv_indices_.push_back(index); }

void IndexedFaceMesh::BuildNeighbors() {
    typedef std::vector<unsigned int> PEdges;

    auto* p_edges = new PEdges[NumVertices()];

    // build vertex-face structure
    m_vertices_faces_.clear();  // to delete old pointers
    m_vertices_faces_.resize(NumVertices());
    m_vertices_edges_.clear();  // to delete old pointers
    m_vertices_edges_.resize(NumVertices());
    m_faces_edges_.clear();
    m_faces_edges_.resize(NumFaces());

    m_edges_.clear();

    auto* v = new unsigned int[m_vertices_per_face_];
    auto* edges = new unsigned int[m_vertices_per_face_ * 2];
    for (unsigned int i = 0; i < NumFaces(); i++) {
        m_faces_edges_[i].resize(m_vertices_per_face_);
        for (unsigned int j = 0u; j < m_vertices_per_face_; j++) v[j] = m_indices_[m_vertices_per_face_ * i + j];

        for (unsigned int j = 0u; j < m_vertices_per_face_ - 1u; j++) {
            edges[2 * j] = v[j];
            edges[2 * j + 1] = v[j + 1];
        }
        edges[2 * (m_vertices_per_face_ - 1)] = v[m_vertices_per_face_ - 1];
        edges[2 * (m_vertices_per_face_ - 1) + 1] = v[0];

        for (unsigned int j = 0u; j < m_vertices_per_face_; j++) {
            // add vertex-face connection
            const unsigned int kVIndex = m_indices_[m_vertices_per_face_ * i + j];
            bool found = false;
            for (unsigned int k : m_vertices_faces_[kVIndex]) {
                if (k == i) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                m_vertices_faces_[kVIndex].push_back(i);
            }

            // add edge information
            const unsigned int kA = edges[j * 2 + 0];
            const unsigned int kB = edges[j * 2 + 1];
            unsigned int edge = 0xffffffff;
            // find edge
            for (unsigned int k = 0; k < p_edges[kA].size(); k++) {
                const Edge& e = m_edges_[p_edges[kA][k]];
                if (((e.m_vert[0] == kA) || (e.m_vert[0] == kB)) && ((e.m_vert[1] == kA) || (e.m_vert[1] == kB))) {
                    edge = p_edges[kA][k];
                    break;
                }
            }
            if (edge == 0xffffffff) {
                // create new
                Edge e{};
                e.m_vert[0] = kA;
                e.m_vert[1] = kB;
                e.m_face[0] = i;
                e.m_face[1] = 0xffffffff;
                m_edges_.push_back(e);
                edge = (unsigned int)m_edges_.size() - 1u;

                // add vertex-edge connection
                m_vertices_edges_[kA].push_back(edge);
                m_vertices_edges_[kB].push_back(edge);
            } else {
                Edge& e = m_edges_[edge];
                e.m_face[1] = i;
            }
            // append to points
            p_edges[kA].push_back(edge);
            p_edges[kB].push_back(edge);
            // append face
            m_faces_edges_[i][j] = edge;
        }
    }
    delete[] v;
    delete[] edges;

    // check for boundary
    m_closed_ = true;
    for (auto& e : m_edges_) {
        if (e.m_face[1] == 0xffffffff) {
            m_closed_ = false;
            break;
        }
    }

    delete[] p_edges;
}

void IndexedFaceMesh::CopyUVs(const UVIndices& uv_indices, const UVs& uvs) {
    m_uvs_.clear();
    m_uvs_.resize(uvs.size());

    for (unsigned int i = 0; i < uvs.size(); i++) {
        m_uvs_[i] = uvs[i];
    }

    m_uv_indices_.clear();
    m_uv_indices_.resize(uv_indices.size());

    for (unsigned int i = 0; i < uv_indices.size(); i++) {
        m_uv_indices_[i] = uv_indices[i];
    }
}

unsigned int IndexedFaceMesh::GetVerticesPerFace() const { return m_vertices_per_face_; }

}  // namespace vox::compute
