//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.compute/bounding_sphere.h"
#include "vox.compute/common.h"
#include "vox.compute/kdTree.h"

namespace vox::compute {
class PointCloudBSH : public KDTree<BoundingSphere> {
public:
    using Super = KDTree<BoundingSphere>;

    PointCloudBSH();

    void Init(const Vector3r* vertices, unsigned int num_vertices);
    [[nodiscard]] Vector3r const& EntityPosition(unsigned int i) const final;
    void ComputeHull(unsigned int b, unsigned int n, BoundingSphere& hull) const final;
    void ComputeHullApprox(unsigned int b, unsigned int n, BoundingSphere& hull) const final;

private:
    const Vector3r* m_vertices_{};
    unsigned int m_num_vertices_{};
};

class TetMeshBSH : public KDTree<BoundingSphere> {
public:
    using Super = KDTree<BoundingSphere>;

    TetMeshBSH();

    void Init(const Vector3r* vertices,
              unsigned int num_vertices,
              const unsigned int* indices,
              unsigned int num_tets,
              Real tolerance);
    [[nodiscard]] Vector3r const& EntityPosition(unsigned int i) const final;
    void ComputeHull(unsigned int b, unsigned int n, BoundingSphere& hull) const final;
    void ComputeHullApprox(unsigned int b, unsigned int n, BoundingSphere& hull) const final;
    void UpdateVertices(const Vector3r* vertices);

private:
    const Vector3r* m_vertices_{};
    unsigned int m_num_vertices_{};
    const unsigned int* m_indices_{};
    unsigned int m_num_tets_{};
    Real m_tolerance_{};
    std::vector<Vector3r> m_com_;
};

class BVHTest {
public:
    using TraversalCallback = std::function<void(unsigned int node_index_1, unsigned int node_index_2)>;

    static void Traverse(PointCloudBSH const& b_1, TetMeshBSH const& b_2, const TraversalCallback& func);
    static void Traverse(PointCloudBSH const& b_1,
                         unsigned int node_index_1,
                         TetMeshBSH const& b_2,
                         unsigned int node_index_2,
                         const TraversalCallback& func);
};
}  // namespace vox::compute
