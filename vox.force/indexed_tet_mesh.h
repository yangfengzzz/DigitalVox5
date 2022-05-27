//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>
#include <vector>

#include "vox.force/common.h"

namespace vox::force {
class IndexedTetMesh {
public:
    struct Edge {
        std::array<unsigned int, 2> m_vert;
    };

    struct Face {
        // edge indices
        std::array<unsigned int, 3> m_edges;
        // tet indices
        std::array<unsigned int, 2> m_tets;
    };

    struct Tet {
        std::array<unsigned int, 6> m_edges;
        std::array<unsigned int, 4> m_faces;
    };

public:
    typedef std::vector<unsigned int> Tets;
    typedef std::vector<unsigned int> Faces;
    typedef std::vector<Tet> TetData;
    typedef std::vector<Face> FaceData;
    typedef std::vector<Edge> Edges;
    typedef std::vector<std::vector<unsigned int>> VerticesTets;
    typedef std::vector<std::vector<unsigned int>> VerticesFaces;
    typedef std::vector<std::vector<unsigned int>> VerticesEdges;

protected:
    unsigned int m_num_points_{};
    Tets m_tet_indices_;
    Faces m_face_indices_;
    Edges m_edges_;
    FaceData m_faces_;
    TetData m_tets_;
    VerticesTets m_vertices_tets_;
    VerticesFaces m_vertices_faces_;
    VerticesEdges m_vertices_edges_;

public:
    IndexedTetMesh();
    ~IndexedTetMesh();

    void Release();
    void InitMesh(unsigned int n_points, unsigned int n_edges, unsigned int n_faces, unsigned int n_tets);
    void AddTet(const unsigned int* indices);
    void AddTet(const int* indices);

    [[nodiscard]] const Faces& GetFaces() const { return m_face_indices_; }
    Faces& GetFaces() { return m_face_indices_; }
    [[nodiscard]] const Tets& GetTets() const { return m_tet_indices_; }
    Tets& GetTets() { return m_tet_indices_; }
    Edges& GetEdges() { return m_edges_; }
    [[nodiscard]] const Edges& GetEdges() const { return m_edges_; }
    [[nodiscard]] const FaceData& GetFaceData() const { return m_faces_; }
    [[nodiscard]] const TetData& GetTetData() const { return m_tets_; }
    [[nodiscard]] const VerticesTets& GetVertexTets() const { return m_vertices_tets_; }
    [[nodiscard]] const VerticesFaces& GetVertexFaces() const { return m_vertices_faces_; }
    [[nodiscard]] const VerticesEdges& GetVertexEdges() const { return m_vertices_edges_; }

    [[nodiscard]] unsigned int NumVertices() const { return m_num_points_; }
    [[nodiscard]] unsigned int NumFaces() const { return (unsigned int)m_face_indices_.size() / 3; }
    [[nodiscard]] unsigned int NumTets() const { return (unsigned int)m_tet_indices_.size() / 4; }
    [[nodiscard]] unsigned int NumEdges() const { return (unsigned int)m_edges_.size(); }

    void BuildNeighbors();
};

}  // namespace vox::force
