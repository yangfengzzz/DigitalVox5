//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>
#include <iterator>
#include <vector>

#include "vox.compute/common.h"

namespace vox::compute {
class IndexedFaceMesh {
public:
    struct Edge {
        std::array<unsigned int, 2> m_face;
        std::array<unsigned int, 2> m_vert;
    };

public:
    typedef std::vector<unsigned int> Faces;
    typedef std::vector<Vector3r> FaceNormals;
    typedef std::vector<Vector3r> VertexNormals;
    typedef std::vector<std::vector<unsigned int>> FacesEdges;
    typedef std::vector<Edge> Edges;
    typedef std::vector<std::vector<unsigned int>> VerticesEdges;
    typedef std::vector<std::vector<unsigned int>> VerticesFaces;
    typedef std::vector<unsigned int> UVIndices;
    typedef std::vector<Vector2r> UVs;

protected:
    unsigned int m_num_points_{};
    Faces m_indices_;
    Edges m_edges_;
    FacesEdges m_faces_edges_;
    bool m_closed_{};
    UVIndices m_uv_indices_;
    UVs m_uvs_;
    VerticesFaces m_vertices_faces_;
    VerticesEdges m_vertices_edges_;
    const unsigned int m_vertices_per_face_ = 3u;
    FaceNormals m_normals_;
    VertexNormals m_vertex_normals_;
    bool m_flat_shading_{};

public:
    IndexedFaceMesh();
    IndexedFaceMesh(IndexedFaceMesh const& other);
    IndexedFaceMesh& operator=(IndexedFaceMesh const& other);
    ~IndexedFaceMesh();

    void Release();
    [[nodiscard]] bool IsClosed() const;
    [[nodiscard]] bool GetFlatShading() const { return m_flat_shading_; }
    void SetFlatShading(const bool v) { m_flat_shading_ = v; }
    void InitMesh(unsigned int n_points, unsigned int n_edges, unsigned int n_faces);
    void AddFace(const unsigned int* indices);
    void AddFace(const int* indices);
    void AddUv(Real u, Real v);
    void AddUvIndex(unsigned int index);

    [[nodiscard]] const Faces& GetFaces() const { return m_indices_; }
    Faces& GetFaces() { return m_indices_; }
    [[nodiscard]] const FaceNormals& GetFaceNormals() const { return m_normals_; }
    FaceNormals& GetFaceNormals() { return m_normals_; }
    [[nodiscard]] const VertexNormals& GetVertexNormals() const { return m_vertex_normals_; }
    VertexNormals& GetVertexNormals() { return m_vertex_normals_; }
    Edges& GetEdges() { return m_edges_; }
    [[nodiscard]] const Edges& GetEdges() const { return m_edges_; }
    [[nodiscard]] const FacesEdges& GetFacesEdges() const { return m_faces_edges_; }
    [[nodiscard]] const UVIndices& GetUvIndices() const { return m_uv_indices_; }
    [[nodiscard]] const UVs& GetUVs() const { return m_uvs_; }
    [[nodiscard]] const VerticesFaces& GetVertexFaces() const { return m_vertices_faces_; }
    [[nodiscard]] const VerticesEdges& GetVertexEdges() const { return m_vertices_edges_; }

    [[nodiscard]] unsigned int NumVertices() const { return m_num_points_; }
    [[nodiscard]] unsigned int NumFaces() const { return (unsigned int)m_indices_.size() / m_vertices_per_face_; }
    [[nodiscard]] unsigned int NumEdges() const { return (unsigned int)m_edges_.size(); }
    [[nodiscard]] unsigned int NumUVs() const { return (unsigned int)m_uvs_.size(); }

    void CopyUVs(const UVIndices& uv_indices, const UVs& uvs);

    void BuildNeighbors();

    template <class PositionData>
    void UpdateNormals(const PositionData& pd, unsigned int offset);

    template <class PositionData>
    void UpdateVertexNormals(const PositionData& pd);

    [[nodiscard]] unsigned int GetVerticesPerFace() const;
};

template <class PositionData>
void IndexedFaceMesh::UpdateNormals(const PositionData& pd, const unsigned int offset) {
    m_normals_.resize(NumFaces());

#pragma omp parallel shared(pd, offset) default(none)
    {
#pragma omp for schedule(static)
        for (int i = 0; i < (int)NumFaces(); i++) {
            // Get first three points of face
            const Vector3r& a = pd.GetPosition(m_indices_[m_vertices_per_face_ * i] + offset);
            const Vector3r& b = pd.GetPosition(m_indices_[m_vertices_per_face_ * i + 1] + offset);
            const Vector3r& c = pd.GetPosition(m_indices_[m_vertices_per_face_ * i + 2] + offset);

            // Create normal
            Vector3r v_1 = b - a;
            Vector3r v_2 = c - a;

            m_normals_[i] = v_1.cross(v_2);
            m_normals_[i].normalize();
            // fix normals of degenerate triangles that can become zero vectors
            if (m_normals_[i].squaredNorm() < 1e-6f) m_normals_[i] = Vector3r::UnitX();
        }
    }
}

template <class PositionData>
void IndexedFaceMesh::UpdateVertexNormals(const PositionData& pd) {
    m_vertex_normals_.resize(NumVertices());

    for (unsigned int i = 0; i < NumVertices(); i++) {
        m_vertex_normals_[i].setZero();
    }

    for (unsigned int i = 0u; i < NumFaces(); i++) {
        const Vector3r& n = m_normals_[i];
        m_vertex_normals_[m_indices_[m_vertices_per_face_ * i]] += n;
        m_vertex_normals_[m_indices_[m_vertices_per_face_ * i + 1]] += n;
        m_vertex_normals_[m_indices_[m_vertices_per_face_ * i + 2]] += n;
    }

    for (unsigned int i = 0; i < NumVertices(); i++) {
        m_vertex_normals_[i].normalize();
    }
}

}  // namespace vox::compute
