//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <queue>
#include <vector>

#include "vox.compute/common.h"

namespace vox::compute {

template <typename HullType>
class KDTree {
public:
    using TraversalPredicate = std::function<bool(unsigned int node_index, unsigned int depth)>;
    using TraversalCallback = std::function<void(unsigned int node_index, unsigned int depth)>;
    using TraversalPriorityLess = std::function<bool(std::array<int, 2> const& nodes)>;

    struct Node {
        Node(unsigned int b, unsigned int n) : children({{-1, -1}}), begin(b), n(n) {}

        Node() = default;

        [[nodiscard]] bool IsLeaf() const { return children[0] < 0 && children[1] < 0; }

        // Index of child nodes in nodes array.
        // -1 if child does not exist.
        std::array<int, 2> children;

        // Index according entries in entity list.
        unsigned int begin;

        // Number of owned entries.
        unsigned int n;
    };

    struct QueueItem {
        unsigned int n, d;
    };
    using TraversalQueue = std::queue<QueueItem>;

    explicit KDTree(std::size_t n, unsigned int max_primitives_per_leaf = 1)
        : m_lst_(n), m_max_primitives_per_leaf_(max_primitives_per_leaf) {}

    virtual ~KDTree() = default;

    Node const& GetNode(unsigned int i) const { return m_nodes_[i]; }
    HullType const& GetHull(unsigned int i) const { return m_hulls_[i]; }
    [[nodiscard]] unsigned int GetEntity(unsigned int i) const { return m_lst_[i]; }

    void Construct();
    void TraverseDepthFirst(TraversalPredicate pred,
                            TraversalCallback cb,
                            TraversalPriorityLess const& less = nullptr) const;
    void TraverseBreadthFirst(TraversalPredicate const& pred,
                              TraversalCallback const& cb,
                              unsigned int start_node = 0,
                              TraversalPriorityLess const& less = nullptr,
                              TraversalQueue& pending = TraversalQueue()) const;
    void TraverseBreadthFirstParallel(TraversalPredicate pred, TraversalCallback cb) const;
    void Update();

protected:
    void Construct(unsigned int node, AlignedBox3r const& box, unsigned int b, unsigned int n);
    void TraverseDepthFirst(unsigned int node,
                            unsigned int depth,
                            TraversalPredicate pred,
                            TraversalCallback cb,
                            TraversalPriorityLess const& less) const;
    void TraverseBreadthFirst(TraversalQueue& pending,
                              TraversalPredicate const& pred,
                              TraversalCallback const& cb,
                              TraversalPriorityLess const& less = nullptr) const;

    unsigned int AddNode(unsigned int b, unsigned int n);

    [[nodiscard]] virtual Vector3r const& EntityPosition(unsigned int i) const = 0;
    virtual void ComputeHull(unsigned int b, unsigned int n, HullType& hull) const = 0;
    virtual void ComputeHullApprox(unsigned int b, unsigned int n, HullType& hull) const { ComputeHull(b, n, hull); }

protected:
    std::vector<unsigned int> m_lst_;

    std::vector<Node> m_nodes_;
    std::vector<HullType> m_hulls_;
    unsigned int m_max_primitives_per_leaf_;
};

}  // namespace vox::compute

#include "kdTree.inl"
