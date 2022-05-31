//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>
#include <cassert>
#include <Eigen/Dense>
#include <string>
#include <vector>

#include "vox.force/discregrid/entity_containers.h"
#include "vox.force/discregrid/halfedge.h"

namespace vox::force::discregrid {

class TriangleMesh {
public:
    TriangleMesh(std::vector<Eigen::Vector3d> const& vertices, std::vector<std::array<unsigned int, 3>> const& faces);

    TriangleMesh(double const* vertices, unsigned int const* faces, std::size_t nv, std::size_t nf);

    explicit TriangleMesh(std::string const& filename);

    void ExportObj(std::string const& filename) const;

    // Halfedge modifiers.
    [[nodiscard]] unsigned int Source(Halfedge const h) const {
        if (h.IsBoundary()) return Target(Opposite(h));
        return m_faces_[h.Face()][h.Edge()];
    }
    [[nodiscard]] unsigned int Target(Halfedge const h) const {
        if (h.IsBoundary()) return Source(Opposite(h));
        return Source(h.Next());
    }
    [[nodiscard]] Halfedge Opposite(Halfedge const h) const {
        if (h.IsBoundary()) return m_b2e_[h.Face()];
        return m_e2e_[h.Face()][h.Edge()];
    }

    // Container getters.
    FaceContainer Faces() { return FaceContainer(this); }
    [[nodiscard]] FaceConstContainer Faces() const { return FaceConstContainer(this); }
    [[nodiscard]] IncidentFaceContainer IncidentFaces(unsigned int v) const { return {v, this}; }
    VertexContainer Vertices() { return VertexContainer(this); }
    [[nodiscard]] VertexConstContainer Vertices() const { return VertexConstContainer(this); }

    // Entity size getters.
    [[nodiscard]] std::size_t NFaces() const { return m_faces_.size(); }
    [[nodiscard]] std::size_t NVertices() const { return m_v2e_.size(); }
    [[nodiscard]] std::size_t NBorderEdges() const { return m_b2e_.size(); }

    // Entity getters.
    [[nodiscard]] unsigned int const& FaceVertex(unsigned int f, unsigned int i) const {
        assert(i < 3);
        assert(f < m_faces_.size());
        return m_faces_[f][i];
    }
    unsigned int& FaceVertex(unsigned int f, unsigned int i) {
        assert(i < 3);
        assert(f < m_faces_.size());
        return m_faces_[f][i];
    }

    [[nodiscard]] Eigen::Vector3d const& Vertex(unsigned int i) const { return m_vertices_[i]; }
    Eigen::Vector3d& Vertex(unsigned int i) { return m_vertices_[i]; }
    [[nodiscard]] std::array<unsigned int, 3> const& Face(unsigned int i) const { return m_faces_[i]; }
    std::array<unsigned int, 3>& Face(unsigned int i) { return m_faces_[i]; }
    [[nodiscard]] Halfedge IncidentHalfedge(unsigned int v) const { return m_v2e_[v]; }

    // Data getters.
    [[nodiscard]] std::vector<Eigen::Vector3d> const& VertexData() const { return m_vertices_; }
    std::vector<Eigen::Vector3d>& VertexData() { return m_vertices_; }
    [[nodiscard]] std::vector<std::array<unsigned int, 3>> const& FaceData() const { return m_faces_; }
    std::vector<std::array<unsigned int, 3>>& FaceData() { return m_faces_; }

    [[nodiscard]] Eigen::Vector3d ComputeFaceNormal(unsigned int f) const;

private:
    void Construct();

private:
    std::vector<Eigen::Vector3d> m_vertices_;
    std::vector<std::array<unsigned int, 3>> m_faces_;
    std::vector<std::array<Halfedge, 3>> m_e2e_;
    std::vector<Halfedge> m_v2e_;
    std::vector<Halfedge> m_b2e_;
};
}  // namespace vox::force::discregrid
