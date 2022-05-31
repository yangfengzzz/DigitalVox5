//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>
#include <Eigen/Core>
#include <iterator>

#include "vox.force/discregrid/halfedge.h"

namespace vox::force::discregrid {

class TriangleMesh;

class FaceContainer;
class FaceIterator : public std::iterator<std::random_access_iterator_tag, std::array<unsigned int, 3>> {
public:
    typedef FaceIterator Self;

    FaceIterator() = delete;

    reference operator*();

    bool operator<(Self const& other) const { return m_index_ < other.m_index_; }
    bool operator==(Self const& other) const { return m_index_ == other.m_index_; }

    bool operator!=(Self const& other) const { return !(*this == other); }

    inline Self& operator++() {
        ++m_index_;
        return *this;
    }
    inline Self& operator--() {
        --m_index_;
        return *this;
    }

    inline Self operator+(Self const& rhs) { return {m_index_ + rhs.m_index_, m_mesh_}; }
    inline difference_type operator-(Self const& rhs) const { return m_index_ - rhs.m_index_; }
    inline Self operator-(int const& rhs) { return {m_index_ - rhs, m_mesh_}; }

    [[nodiscard]] unsigned int Vertex(unsigned int i) const;
    unsigned int& Vertex(unsigned int i);

private:
    friend class FaceContainer;
    FaceIterator(unsigned int index, TriangleMesh* mesh) : m_index_(index), m_mesh_(mesh) {}

    unsigned int m_index_;
    TriangleMesh* m_mesh_;
};
class FaceConstIterator : public std::iterator<std::random_access_iterator_tag, std::array<unsigned int, 3> const> {
public:
    typedef FaceConstIterator Self;

    FaceConstIterator() = delete;

    reference operator*();

    bool operator<(Self const& other) const { return m_index_ < other.m_index_; }
    bool operator==(Self const& other) const { return m_index_ == other.m_index_; }

    bool operator!=(Self const& other) const { return !(*this == other); }

    inline Self& operator++() {
        ++m_index_;
        return *this;
    }
    inline Self& operator--() {
        --m_index_;
        return *this;
    }

    inline Self operator+(Self const& rhs) const { return {m_index_ + rhs.m_index_, m_mesh_}; }
    inline difference_type operator-(Self const& rhs) const { return m_index_ - rhs.m_index_; }
    inline Self operator-(int const& rhs) const { return {m_index_ - rhs, m_mesh_}; }

    [[nodiscard]] unsigned int Vertex(unsigned int i) const;
    unsigned int& Vertex(unsigned int i);

private:
    friend class FaceConstContainer;
    FaceConstIterator(unsigned int index, TriangleMesh const* mesh) : m_index_(index), m_mesh_(mesh) {}

    unsigned int m_index_;
    TriangleMesh const* m_mesh_;
};

class IncidentFaceContainer;
class IncidentFaceIterator : public std::iterator<std::forward_iterator_tag, Halfedge> {
public:
    typedef IncidentFaceIterator Self;

    value_type operator*() { return m_h_; }
    Self& operator++();
    bool operator==(Self const& other) const { return m_h_ == other.m_h_; }

    bool operator!=(Self const& other) const { return !(*this == other); }

private:
    friend class IncidentFaceContainer;
    IncidentFaceIterator(unsigned int v, TriangleMesh const* mesh);
    IncidentFaceIterator() : m_h_(), m_begin_(), m_mesh_(nullptr) {}

    Halfedge m_h_, m_begin_;
    TriangleMesh const* m_mesh_;
};

class VertexContainer;
class VertexIterator : public std::iterator<std::random_access_iterator_tag, Eigen::Vector3d> {
public:
    typedef VertexIterator Self;

    VertexIterator() = delete;

    reference operator*();

    bool operator<(Self const& other) const { return m_index_ < other.m_index_; }
    bool operator==(Self const& other) const { return m_index_ == other.m_index_; }

    bool operator!=(Self const& other) const { return !(*this == other); }

    inline Self& operator++() {
        ++m_index_;
        return *this;
    }
    inline Self& operator--() {
        --m_index_;
        return *this;
    }

    inline Self operator+(Self const& rhs) const { return {m_index_ + rhs.m_index_, m_mesh_}; }
    inline difference_type operator-(Self const& rhs) const { return m_index_ - rhs.m_index_; }
    inline Self operator-(int const& rhs) const { return {m_index_ - rhs, m_mesh_}; }

    [[nodiscard]] unsigned int Index() const;

private:
    friend class VertexContainer;
    VertexIterator(unsigned int index, TriangleMesh* mesh) : m_index_(index), m_mesh_(mesh) {}

    unsigned int m_index_;
    TriangleMesh* m_mesh_;
};

class VertexConstContainer;
class VertexConstIterator : public std::iterator<std::random_access_iterator_tag, Eigen::Vector3d const> {
public:
    typedef VertexConstIterator Self;

    VertexConstIterator() = delete;

    reference operator*();

    bool operator<(Self const& other) const { return m_index_ < other.m_index_; }
    bool operator==(Self const& other) const { return m_index_ == other.m_index_; }

    bool operator!=(Self const& other) const { return !(*this == other); }

    inline Self& operator++() {
        ++m_index_;
        return *this;
    }
    inline Self& operator--() {
        --m_index_;
        return *this;
    }

    inline Self operator+(Self const& rhs) const { return {m_index_ + rhs.m_index_, m_mesh_}; }
    inline difference_type operator-(Self const& rhs) const { return m_index_ - rhs.m_index_; }
    inline Self operator-(int const& rhs) const { return {m_index_ - rhs, m_mesh_}; }

    [[nodiscard]] unsigned int Index() const;

private:
    friend class VertexConstContainer;
    VertexConstIterator(unsigned int index, TriangleMesh const* mesh) : m_index_(index), m_mesh_(mesh) {}

    unsigned int m_index_;
    TriangleMesh const* m_mesh_;
};
}  // namespace vox::force::discregrid
