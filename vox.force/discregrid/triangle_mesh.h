//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Eigen/Dense>
#include <array>
#include <cassert>
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

    void exportOBJ(std::string const& filename) const;

    // Halfedge modifiers.
    [[nodiscard]] unsigned int source(Halfedge const h) const {
        if (h.isBoundary()) return target(opposite(h));
        return m_faces[h.face()][h.edge()];
    }
    [[nodiscard]] unsigned int target(Halfedge const h) const {
        if (h.isBoundary()) return source(opposite(h));
        return source(h.next());
    }
    [[nodiscard]] Halfedge opposite(Halfedge const h) const {
        if (h.isBoundary()) return m_b2e[h.face()];
        return m_e2e[h.face()][h.edge()];
    }

    // Container getters.
    FaceContainer faces() { return FaceContainer(this); }
    [[nodiscard]] FaceConstContainer faces() const { return FaceConstContainer(this); }
    [[nodiscard]] IncidentFaceContainer incident_faces(unsigned int v) const { return {v, this}; }
    VertexContainer vertices() { return VertexContainer(this); }
    [[nodiscard]] VertexConstContainer vertices() const { return VertexConstContainer(this); }

    // Entity size getters.
    [[nodiscard]] std::size_t nFaces() const { return m_faces.size(); }
    [[nodiscard]] std::size_t nVertices() const { return m_v2e.size(); }
    [[nodiscard]] std::size_t nBorderEdges() const { return m_b2e.size(); }

    // Entity getters.
    [[nodiscard]] unsigned int const& faceVertex(unsigned int f, unsigned int i) const {
        assert(i < 3);
        assert(f < m_faces.size());
        return m_faces[f][i];
    }
    unsigned int& faceVertex(unsigned int f, unsigned int i) {
        assert(i < 3);
        assert(f < m_faces.size());
        return m_faces[f][i];
    }

    [[nodiscard]] Eigen::Vector3d const& vertex(unsigned int i) const { return m_vertices[i]; }
    Eigen::Vector3d& vertex(unsigned int i) { return m_vertices[i]; }
    [[nodiscard]] std::array<unsigned int, 3> const& face(unsigned int i) const { return m_faces[i]; }
    std::array<unsigned int, 3>& face(unsigned int i) { return m_faces[i]; }
    [[nodiscard]] Halfedge incident_halfedge(unsigned int v) const { return m_v2e[v]; }

    // Data getters.
    [[nodiscard]] std::vector<Eigen::Vector3d> const& vertex_data() const { return m_vertices; }
    std::vector<Eigen::Vector3d>& vertex_data() { return m_vertices; }
    [[nodiscard]] std::vector<std::array<unsigned int, 3>> const& face_data() const { return m_faces; }
    std::vector<std::array<unsigned int, 3>>& face_data() { return m_faces; }

    [[nodiscard]] Eigen::Vector3d computeFaceNormal(unsigned int f) const;

private:
    void construct();

private:
    std::vector<Eigen::Vector3d> m_vertices;
    std::vector<std::array<unsigned int, 3>> m_faces;
    std::vector<std::array<Halfedge, 3>> m_e2e;
    std::vector<Halfedge> m_v2e;
    std::vector<Halfedge> m_b2e;
};
}  // namespace vox::force::discregrid
