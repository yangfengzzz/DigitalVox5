//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <omp.h>

#include <cstring>

#include "vox.force/bounding_sphere.h"
#include "vox.force/kdTree.h"

namespace vox::force {
template <typename HullType>
void KDTree<HullType>::Construct() {
    m_nodes_.clear();
    m_hulls_.clear();
    if (m_lst_.empty()) return;

    std::iota(m_lst_.begin(), m_lst_.end(), 0);

    // Determine bounding box of considered domain.
    auto box = AlignedBox3r{};
    for (auto i = 0u; i < m_lst_.size(); ++i) box.extend(EntityPosition(i));

    auto ni = AddNode(0, static_cast<unsigned int>(m_lst_.size()));
    Construct(ni, box, 0, static_cast<unsigned int>(m_lst_.size()));
}

template <typename HullType>
void KDTree<HullType>::Construct(unsigned int node, AlignedBox3r const& box, unsigned int b, unsigned int n) {
    // If only one element is left end recursion.
    // if (n == 1) return;
    if (n <= m_max_primitives_per_leaf_) return;

    // Determine longest side of bounding box.
    auto max_dir = 0;
    auto d = box.diagonal().eval();
    if (d(1) >= d(0) && d(1) >= d(2))
        max_dir = 1;
    else if (d(2) >= d(0) && d(2) >= d(1))
        max_dir = 2;

#ifdef _DEBUG
    for (auto i = 0u; i < n; ++i) {
        if (!box.contains(entity_position(m_lst[b + i]))) std::cerr << "ERROR: Bounding box wrong!" << std::endl;
    }
#endif

    // Sort range according to center of the longest side.
    std::sort(m_lst_.begin() + b, m_lst_.begin() + b + n,
              [&](unsigned int a, unsigned int b) { return EntityPosition(a)(max_dir) < EntityPosition(b)(max_dir); });

    auto hal = n / 2;
    auto n_0 = AddNode(b, hal);
    auto n_1 = AddNode(b + hal, n - hal);
    m_nodes_[node].children[0] = n_0;
    m_nodes_[node].children[1] = n_1;

    auto c = static_cast<Real>(0.5) *
             (EntityPosition(m_lst_[b + hal - 1])(max_dir) + EntityPosition(m_lst_[b + hal])(max_dir));
    auto l_box = box;
    l_box.max()(max_dir) = c;
    auto r_box = box;
    r_box.min()(max_dir) = c;

    Construct(m_nodes_[node].children[0], l_box, b, hal);
    Construct(m_nodes_[node].children[1], r_box, b + hal, n - hal);
}

template <typename HullType>
void KDTree<HullType>::TraverseDepthFirst(TraversalPredicate pred,
                                          TraversalCallback cb,
                                          TraversalPriorityLess const& less) const {
    if (m_nodes_.empty()) return;

    if (pred(0, 0)) TraverseDepthFirst(0, 0, pred, cb, less);
}

template <typename HullType>
void KDTree<HullType>::TraverseDepthFirst(unsigned int node_index,
                                          unsigned int depth,
                                          TraversalPredicate pred,
                                          TraversalCallback cb,
                                          TraversalPriorityLess const& less) const {
    Node const& node = m_nodes_[node_index];

    cb(node_index, depth);
    auto is_pred = pred(node_index, depth);
    if (!node.IsLeaf() && is_pred) {
        if (less && !less(node.children)) {
            TraverseDepthFirst(m_nodes_[node_index].children[1], depth + 1, pred, cb, less);
            TraverseDepthFirst(m_nodes_[node_index].children[0], depth + 1, pred, cb, less);
        } else {
            TraverseDepthFirst(m_nodes_[node_index].children[0], depth + 1, pred, cb, less);
            TraverseDepthFirst(m_nodes_[node_index].children[1], depth + 1, pred, cb, less);
        }
    }
}

template <typename HullType>
void KDTree<HullType>::TraverseBreadthFirst(TraversalPredicate const& pred,
                                            TraversalCallback const& cb,
                                            unsigned int start_node,
                                            TraversalPriorityLess const& less,
                                            TraversalQueue& pending) const {
    cb(start_node, 0);
    if (pred(start_node, 0)) pending.push({start_node, 0});
    TraverseBreadthFirst(pending, pred, cb, less);
}

template <typename HullType>
void KDTree<HullType>::TraverseBreadthFirstParallel(TraversalPredicate pred, TraversalCallback cb) const {
    auto start_nodes = std::vector<QueueItem>{};
#ifdef _DEBUG
    const unsigned int maxThreads = 1;
#else
    const unsigned int kMaxThreads = omp_get_max_threads();
#endif

    // compute ceiling of Log2
    // assuming double and long long have the same size.
    double d = kMaxThreads - 1;
    long long ll;
    memcpy(&ll, &d, sizeof(d));
    const unsigned int kTargetDepth = (ll >> 52) - 1022ll;

    TraverseBreadthFirst(
            [&start_nodes, &kMaxThreads, &kTargetDepth](unsigned int node_index, unsigned int depth) {
                return (depth < kTargetDepth) && (start_nodes.size() < kMaxThreads);
            },
            [&](unsigned int node_index, unsigned int depth) {
                if ((depth == kTargetDepth) || (GetNode(node_index).IsLeaf()))
                    start_nodes.push_back({node_index, depth});
            });

#pragma omp parallel default(none)
    {
#pragma omp for schedule(static)
        for (int i = 0; i < start_nodes.size(); i++) {
            QueueItem const& qi = start_nodes[i];
            cb(qi.n, qi.d);
            TraverseDepthFirst(qi.n, qi.d, pred, cb, nullptr);
        }
    }
}

template <typename HullType>
unsigned int KDTree<HullType>::AddNode(unsigned int b, unsigned int n) {
    HullType hull;
    ComputeHull(b, n, hull);
    m_hulls_.push_back(hull);
    m_nodes_.push_back({b, n});
    return static_cast<unsigned int>(m_nodes_.size() - 1);
}

template <typename HullType>
void KDTree<HullType>::TraverseBreadthFirst(TraversalQueue& pending,
                                            TraversalPredicate const& pred,
                                            TraversalCallback const& cb,
                                            TraversalPriorityLess const& less) const {
    while (!pending.empty()) {
        auto n = pending.front().n;
        auto d = pending.front().d;
        auto const& node = m_nodes_[n];
        pending.pop();

        cb(n, d);
        auto is_pred = pred(n, d);
        if (!node.IsLeaf() && is_pred) {
            if (less && !less(node.children)) {
                pending.push({static_cast<unsigned int>(node.children[1]), d + 1});
                pending.push({static_cast<unsigned int>(node.children[0]), d + 1});
            } else {
                pending.push({static_cast<unsigned int>(node.children[0]), d + 1});
                pending.push({static_cast<unsigned int>(node.children[1]), d + 1});
            }
        }
    }
}

template <typename HullType>
void KDTree<HullType>::Update() {
    TraverseDepthFirst([&](unsigned int, unsigned int) { return true; },
                       [&](unsigned int node_index, unsigned int) {
                           auto const& nd = GetNode(node_index);
                           ComputeHullApprox(nd.begin, nd.n, m_hulls_[node_index]);
                       });
}

}  // namespace vox::force