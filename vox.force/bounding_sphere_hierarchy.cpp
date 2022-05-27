
#include "bounding_sphere_hierarchy.h"

#include <set>
#include <unordered_set>

namespace vox::force {

PointCloudBSH::PointCloudBSH() : Super(0, 10) {}

Vector3r const& PointCloudBSH::EntityPosition(unsigned int i) const { return m_vertices_[i]; }

void PointCloudBSH::ComputeHull(unsigned int b, unsigned int n, BoundingSphere& hull) const {
    auto vertices_subset = std::vector<Vector3r>(n);
    for (unsigned int i = b; i < b + n; ++i) vertices_subset[i - b] = m_vertices_[m_lst_[i]];

    const BoundingSphere kS(vertices_subset);

    hull.X() = kS.X();
    hull.R() = kS.R();
}

void PointCloudBSH::ComputeHullApprox(unsigned int b, unsigned int n, BoundingSphere& hull) const {
    // compute center
    Vector3r x;
    x.setZero();
    for (unsigned int i = b; i < b + n; i++) x += m_vertices_[m_lst_[i]];
    x /= (Real)n;

    Real radius_2 = 0.0;
    for (unsigned int i = b; i < b + n; i++) {
        radius_2 = std::max(radius_2, (x - m_vertices_[m_lst_[i]]).squaredNorm());
    }

    hull.X() = x;
    hull.R() = sqrt(radius_2);
}

void PointCloudBSH::Init(const Vector3r* vertices, const unsigned int num_vertices) {
    m_lst_.resize(num_vertices);
    m_vertices_ = vertices;
    m_num_vertices_ = num_vertices;
}

//////////////////////////////////////////////////////////////////////////

TetMeshBSH::TetMeshBSH() : Super(0) {}

Vector3r const& TetMeshBSH::EntityPosition(unsigned int i) const { return m_com_[i]; }

void TetMeshBSH::ComputeHull(unsigned int b, unsigned int n, BoundingSphere& hull) const {
    ComputeHullApprox(b, n, hull);
}

void TetMeshBSH::ComputeHullApprox(unsigned int b, unsigned int n, BoundingSphere& hull) const {
    // compute center
    Vector3r x;
    x.setZero();
    for (unsigned int i = b; i < b + n; i++) {
        const unsigned int kTet = m_lst_[i];
        x += m_vertices_[m_indices_[4 * kTet]];
        x += m_vertices_[m_indices_[4 * kTet + 1]];
        x += m_vertices_[m_indices_[4 * kTet + 2]];
        x += m_vertices_[m_indices_[4 * kTet + 3]];
    }
    x /= ((Real)4.0 * (Real)n);

    Real radius_2 = 0.0;
    for (unsigned int i = b; i < b + n; i++) {
        const unsigned int kTet = m_lst_[i];
        radius_2 = std::max(radius_2, (x - m_vertices_[m_indices_[4 * kTet]]).squaredNorm());
        radius_2 = std::max(radius_2, (x - m_vertices_[m_indices_[4 * kTet + 1]]).squaredNorm());
        radius_2 = std::max(radius_2, (x - m_vertices_[m_indices_[4 * kTet + 2]]).squaredNorm());
        radius_2 = std::max(radius_2, (x - m_vertices_[m_indices_[4 * kTet + 3]]).squaredNorm());
    }

    hull.X() = x;
    hull.R() = sqrt(radius_2) + m_tolerance_;
}

void TetMeshBSH::Init(const Vector3r* vertices,
                      const unsigned int num_vertices,
                      const unsigned int* indices,
                      const unsigned int num_tets,
                      const Real tolerance) {
    m_lst_.resize(num_tets);
    m_vertices_ = vertices;
    m_num_vertices_ = num_vertices;
    m_indices_ = indices;
    m_num_tets_ = num_tets;
    m_tolerance_ = tolerance;
    m_com_.resize(num_tets);
    for (unsigned int i = 0; i < num_tets; i++) {
        m_com_[i] = 0.25 * (m_vertices_[m_indices_[4 * i]] + m_vertices_[m_indices_[4 * i + 1]] +
                            m_vertices_[m_indices_[4 * i + 2]] + m_vertices_[m_indices_[4 * i + 3]]);
    }
}

void TetMeshBSH::UpdateVertices(const Vector3r* vertices) { m_vertices_ = vertices; }

void BVHTest::Traverse(PointCloudBSH const& b_1, TetMeshBSH const& b_2, const TraversalCallback& func) {
    Traverse(b_1, 0, b_2, 0, func);
}

void BVHTest::Traverse(PointCloudBSH const& b_1,
                       const unsigned int node_index_1,
                       TetMeshBSH const& b_2,
                       const unsigned int node_index_2,
                       const TraversalCallback& func) {
    const BoundingSphere& bs_1 = b_1.GetHull(node_index_1);
    const BoundingSphere& bs_2 = b_2.GetHull(node_index_2);
    if (!bs_1.Overlaps(bs_2)) return;

    auto const& node_1 = b_1.GetNode(node_index_1);
    auto const& node_2 = b_2.GetNode(node_index_2);
    if (node_1.IsLeaf() && node_2.IsLeaf()) {
        func(node_index_1, node_index_2);
        return;
    }

    if (bs_1.R() < bs_2.R()) {
        if (!node_1.IsLeaf()) {
            Traverse(b_1, node_1.children[0], b_2, node_index_2, func);
            Traverse(b_1, node_1.children[1], b_2, node_index_2, func);
        } else {
            Traverse(b_1, node_index_1, b_2, node_2.children[0], func);
            Traverse(b_1, node_index_1, b_2, node_2.children[1], func);
        }
    } else {
        if (!node_2.IsLeaf()) {
            Traverse(b_1, node_index_1, b_2, node_2.children[0], func);
            Traverse(b_1, node_index_1, b_2, node_2.children[1], func);
        } else {
            Traverse(b_1, node_1.children[0], b_2, node_index_2, func);
            Traverse(b_1, node_1.children[1], b_2, node_index_2, func);
        }
    }
}

}  // namespace vox::force