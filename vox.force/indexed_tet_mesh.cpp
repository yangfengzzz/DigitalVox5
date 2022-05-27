#include "indexed_tet_mesh.h"

namespace vox::force {

IndexedTetMesh::IndexedTetMesh() = default;

IndexedTetMesh::~IndexedTetMesh() { Release(); }

void IndexedTetMesh::InitMesh(const unsigned int n_points,
                              const unsigned int n_edges,
                              const unsigned int n_faces,
                              const unsigned int n_tets) {
    m_num_points_ = n_points;
    m_face_indices_.reserve(n_faces * 3);
    m_tet_indices_.reserve(n_tets * 4);
    m_edges_.reserve(n_edges);
    m_faces_.reserve(n_faces);
    m_tets_.reserve(n_tets);
    m_vertices_tets_.reserve(n_points);
    m_vertices_faces_.reserve(n_points);
    m_vertices_edges_.reserve(n_points);
}

void IndexedTetMesh::Release() {
    m_face_indices_.clear();
    m_tet_indices_.clear();
    m_edges_.clear();
    m_tets_.clear();
    m_faces_.clear();
    m_vertices_tets_.clear();
    m_vertices_faces_.clear();
    m_vertices_edges_.clear();
}

/** Add a new tet. Indices must be an array of size 4.
 */
void IndexedTetMesh::AddTet(const unsigned int* const indices) {
    for (unsigned int i = 0u; i < 4; i++) m_tet_indices_.push_back(indices[i]);
}

/** Add a new tet. Indices must be an array of size 4.
 */
void IndexedTetMesh::AddTet(const int* const indices) {
    for (unsigned int i = 0u; i < 4; i++) m_tet_indices_.push_back((unsigned int)indices[i]);
}

void IndexedTetMesh::BuildNeighbors() {
    typedef std::vector<unsigned int> VertexEdges;

    m_vertices_edges_.clear();  // to delete old pointers
    m_vertices_edges_.resize(NumVertices());

    m_vertices_faces_.clear();  // to delete old pointers
    m_vertices_faces_.resize(NumVertices());

    m_vertices_tets_.clear();  // to delete old pointers
    m_vertices_tets_.resize(NumVertices());

    m_faces_.clear();
    m_edges_.clear();
    m_tets_.resize(NumTets());

    for (unsigned int i = 0; i < NumTets(); i++) {
        // tet edge indices: {0,1, 0,2, 0,3, 1,2, 1,3, 2,3}
        const unsigned int edges[12] = {
                m_tet_indices_[4 * i],     m_tet_indices_[4 * i + 1], m_tet_indices_[4 * i],
                m_tet_indices_[4 * i + 2], m_tet_indices_[4 * i],     m_tet_indices_[4 * i + 3],
                m_tet_indices_[4 * i + 1], m_tet_indices_[4 * i + 2], m_tet_indices_[4 * i + 1],
                m_tet_indices_[4 * i + 3], m_tet_indices_[4 * i + 2], m_tet_indices_[4 * i + 3]};

        // tet face indices: {0,1,2, 1,3,2, 3,0,2, 1,0,3} => clock wise
        /*const unsigned int faces[12] = {	m_tetIndices[4*i], m_tetIndices[4*i+1], m_tetIndices[4*i+2],
                                                                                m_tetIndices[4*i+1],
           m_tetIndices[4*i+3], m_tetIndices[4*i+2], m_tetIndices[4*i+3], m_tetIndices[4*i], m_tetIndices[4*i+2],
                                                                                m_tetIndices[4*i+1], m_tetIndices[4*i],
           m_tetIndices[4*i+3]};*/

        // tet face indices: {1,0,2, 3,1,2, 0,3,2, 0,1,3} => counter clock wise
        const unsigned int faces[12] = {
                m_tet_indices_[4 * i + 1], m_tet_indices_[4 * i],     m_tet_indices_[4 * i + 2],
                m_tet_indices_[4 * i + 3], m_tet_indices_[4 * i + 1], m_tet_indices_[4 * i + 2],
                m_tet_indices_[4 * i],     m_tet_indices_[4 * i + 3], m_tet_indices_[4 * i + 2],
                m_tet_indices_[4 * i],     m_tet_indices_[4 * i + 1], m_tet_indices_[4 * i + 3]};

        for (unsigned int j = 0u; j < 4; j++) {
            // add vertex-tet connection
            const unsigned int kVIndex = m_tet_indices_[4 * i + j];
            m_vertices_tets_[kVIndex].push_back(i);
        }

        for (unsigned int j = 0u; j < 4; j++) {
            // add face information
            const unsigned int kA = faces[j * 3 + 0];
            const unsigned int kB = faces[j * 3 + 1];
            const unsigned int kC = faces[j * 3 + 2];
            unsigned int face = 0xffffffff;
            // find face
            for (unsigned int k = 0; k < m_vertices_faces_[kA].size(); k++) {
                // Check if we already have this face in the list
                const unsigned int& face_index = m_vertices_faces_[kA][k];
                if (((m_face_indices_[3 * face_index] == kA) || (m_face_indices_[3 * face_index] == kB) ||
                     (m_face_indices_[3 * face_index] == kC)) &&
                    ((m_face_indices_[3 * face_index + 1] == kA) || (m_face_indices_[3 * face_index + 1] == kB) ||
                     (m_face_indices_[3 * face_index + 1] == kC)) &&
                    ((m_face_indices_[3 * face_index + 2] == kA) || (m_face_indices_[3 * face_index + 2] == kB) ||
                     (m_face_indices_[3 * face_index + 2] == kC))) {
                    face = m_vertices_faces_[kA][k];
                    break;
                }
            }
            if (face == 0xffffffff) {
                // create new
                Face f{};
                m_face_indices_.push_back(kA);
                m_face_indices_.push_back(kB);
                m_face_indices_.push_back(kC);
                face = (unsigned int)m_face_indices_.size() / 3 - 1u;
                f.m_tets[0] = i;
                f.m_tets[1] = 0xffffffff;
                m_faces_.push_back(f);

                // add vertex-face connection
                m_vertices_faces_[kA].push_back(face);
                m_vertices_faces_[kB].push_back(face);
                m_vertices_faces_[kC].push_back(face);
            } else {
                Face& fd = m_faces_[face];
                fd.m_tets[1] = i;
            }
            // append face
            m_tets_[i].m_faces[j] = face;
        }

        for (unsigned int j = 0u; j < 6; j++) {
            // add face information
            const unsigned int kA = edges[j * 2 + 0];
            const unsigned int kB = edges[j * 2 + 1];
            unsigned int edge = 0xffffffff;
            // find edge
            for (unsigned int k = 0; k < m_vertices_edges_[kA].size(); k++) {
                // Check if we already have this edge in the list
                const Edge& e = m_edges_[m_vertices_edges_[kA][k]];
                if (((e.m_vert[0] == kA) || (e.m_vert[0] == kB)) && ((e.m_vert[1] == kA) || (e.m_vert[1] == kB))) {
                    edge = m_vertices_edges_[kA][k];
                    break;
                }
            }
            if (edge == 0xffffffff) {
                // create new
                Edge e{};
                e.m_vert[0] = kA;
                e.m_vert[1] = kB;
                m_edges_.push_back(e);
                edge = (unsigned int)m_edges_.size() - 1u;

                // add vertex-edge connection
                m_vertices_edges_[kA].push_back(edge);
                m_vertices_edges_[kB].push_back(edge);
            }
            // append edge
            m_tets_[i].m_edges[j] = edge;
        }
    }
}

}  // namespace vox::force