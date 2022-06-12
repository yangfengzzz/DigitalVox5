//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/discregrid/triangle_mesh.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <unordered_set>

using namespace Eigen;

namespace {
template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
}  // namespace

namespace vox::compute::discregrid {

struct HalfedgeHasher {
    explicit HalfedgeHasher(std::vector<std::array<unsigned int, 3>> const& faces) : faces(&faces) {}

    std::size_t operator()(Halfedge const& he) const {
        unsigned int f = he.Face();
        unsigned int e = he.Edge();
        std::array<unsigned int, 2> v = {(*faces)[f][e], (*faces)[f][(e + 1) % 3]};
        if (v[0] > v[1]) std::swap(v[0], v[1]);

        std::size_t seed(0);
        hash_combine(seed, v[0]);
        hash_combine(seed, v[1]);
        return seed;
    }

    std::vector<std::array<unsigned int, 3>> const* faces;
};

struct HalfedgeEqualTo {
    explicit HalfedgeEqualTo(std::vector<std::array<unsigned int, 3>> const& faces) : faces(&faces) {}

    bool operator()(Halfedge const& a, Halfedge const& b) const {
        unsigned int fa = a.Face();
        unsigned int ea = a.Edge();
        std::array<unsigned int, 2> va = {(*faces)[fa][ea], (*faces)[fa][(ea + 1) % 3]};

        unsigned int fb = b.Face();
        unsigned int eb = b.Edge();
        std::array<unsigned int, 2> vb = {(*faces)[fb][eb], (*faces)[fb][(eb + 1) % 3]};

        return va[0] == vb[1] && va[1] == vb[0];
    }

    std::vector<std::array<unsigned int, 3>> const* faces;
};

typedef std::unordered_set<Halfedge, HalfedgeHasher, HalfedgeEqualTo> FaceSet;

TriangleMesh::TriangleMesh(std::vector<Vector3d> const& vertices, std::vector<std::array<unsigned int, 3>> const& faces)
    : m_faces_(faces), m_e2e_(3 * faces.size()), m_vertices_(vertices), m_v2e_(vertices.size()) {
    Construct();
}

TriangleMesh::TriangleMesh(double const* vertices, unsigned int const* faces, std::size_t nv, std::size_t nf)
    : m_faces_(nf), m_vertices_(nv), m_e2e_(3 * nf), m_v2e_(nv) {
    std::copy(vertices, vertices + 3 * nv, m_vertices_[0].data());
    std::copy(faces, faces + 3 * nf, m_faces_[0].data());
    Construct();
}

TriangleMesh::TriangleMesh(std::string const& path) {
    std::ifstream in(path, std::ios::in);
    if (!in) {
        std::cerr << "Cannot open " << path << std::endl;
        return;
    }

    std::string line;
    while (getline(in, line)) {
        if (line.substr(0, 2) == "v ") {
            std::istringstream s(line.substr(2));
            Vector3d v;
            s >> v.x();
            s >> v.y();
            s >> v.z();
            m_vertices_.push_back(v);
        } else if (line.substr(0, 2) == "f ") {
            std::istringstream s(line.substr(2));
            std::array<unsigned int, 3> f;
            for (unsigned int j(0); j < 3; ++j) {
                std::string buf;
                s >> buf;
                buf = buf.substr(0, buf.find_first_of('/'));
                f[j] = std::stoi(buf) - 1;
            }
            m_faces_.push_back(f);
        } else if (line[0] == '#') { /* ignoring this line */
        } else {                     /* ignoring this line */
        }
    }

    Construct();
}

void TriangleMesh::ExportObj(std::string const& filename) const {
    auto outfile = std::ofstream(filename.c_str());
    auto str_stream = std::stringstream(std::stringstream::in);

    outfile << "g default" << std::endl;
    for (auto const& pos : m_vertices_) {
        outfile << "v " << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
    }

    for (auto const& f : m_faces_) {
        outfile << "f";
        for (auto v : f) outfile << " " << v + 1;
        outfile << std::endl;
    }

    outfile.close();
}

void TriangleMesh::Construct() {
    m_e2e_.resize(3 * m_faces_.size());
    m_v2e_.resize(m_vertices_.size());

    // Build adjacencies for mesh faces.
    FaceSet face_set((m_faces_.size() * 3) / 2, HalfedgeHasher(m_faces_), HalfedgeEqualTo(m_faces_));
    for (unsigned int i(0); i < m_faces_.size(); ++i)
        for (unsigned char j(0); j < 3; ++j) {
            Halfedge he(i, j);
            auto ret = face_set.insert(he);
            if (!ret.second) {
                m_e2e_[he.Face()][he.Edge()] = *(ret.first);
                m_e2e_[ret.first->Face()][ret.first->Edge()] = he;

                face_set.erase(ret.first);
            }

            m_v2e_[m_faces_[i][j]] = he;
        }

    m_b2e_.reserve(face_set.size());

    for (Halfedge const kHe : face_set) {
        m_b2e_.push_back(kHe);
        Halfedge b(static_cast<unsigned int>(m_b2e_.size()) - 1u, 3);
        m_e2e_[kHe.Face()][kHe.Edge()] = b;
        m_v2e_[Target(kHe)] = b;

        assert(Source(b) == Target(kHe));
    }

#ifdef _DEBUG
    for (unsigned int i(0); i < nFaces(); ++i) {
        Halfedge h(i, 0);
        for (unsigned int j(0); j < 3; ++j) {
            assert(faceVertex(i, j) == source(h));
            h = h.next();
        }
    }
#endif

    if (!m_b2e_.empty()) {
        std::cout << std::endl << "WARNING: Mesh not closed!" << std::endl;
    }
}

Vector3d TriangleMesh::ComputeFaceNormal(unsigned int f) const {
    Vector3d const& x0 = Vertex(FaceVertex(f, 0));
    Vector3d const& x1 = Vertex(FaceVertex(f, 1));
    Vector3d const& x2 = Vertex(FaceVertex(f, 2));

    return (x1 - x0).cross(x2 - x0).normalized();
}

}  // namespace vox::compute::discregrid
