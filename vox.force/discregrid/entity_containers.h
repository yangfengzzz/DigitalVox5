//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/discregrid/entity_iterators.h"

namespace vox::force::discregrid {

class TriangleMesh;

class FaceContainer {
public:
    [[nodiscard]] FaceIterator Begin() const { return {0, m_mesh_}; }
    [[nodiscard]] FaceIterator End() const;

private:
    friend class TriangleMesh;
    explicit FaceContainer(TriangleMesh* mesh) : m_mesh_(mesh) {}

    TriangleMesh* m_mesh_;
};

class FaceConstContainer {
public:
    [[nodiscard]] FaceConstIterator Begin() const { return {0, m_mesh_}; }
    [[nodiscard]] FaceConstIterator End() const;

private:
    friend class TriangleMesh;
    explicit FaceConstContainer(TriangleMesh const* mesh) : m_mesh_(mesh) {}

    TriangleMesh const* m_mesh_;
};

class IncidentFaceContainer {
public:
    [[nodiscard]] IncidentFaceIterator Begin() const { return {m_v_, m_mesh_}; }
    [[nodiscard]] IncidentFaceIterator End() const { return {}; }

private:
    friend class TriangleMesh;
    IncidentFaceContainer(unsigned int v, TriangleMesh const* mesh) : m_v_(v), m_mesh_(mesh) {}

    TriangleMesh const* m_mesh_;
    unsigned int m_v_;
};

class VertexContainer {
public:
    [[nodiscard]] VertexIterator Begin() const { return {0, m_mesh_}; }
    [[nodiscard]] VertexIterator End() const;

private:
    friend class TriangleMesh;
    explicit VertexContainer(TriangleMesh* mesh) : m_mesh_(mesh) {}

    TriangleMesh* m_mesh_;
};

class VertexConstContainer {
public:
    [[nodiscard]] VertexConstIterator Begin() const { return {0, m_mesh_}; }
    [[nodiscard]] VertexConstIterator End() const;

private:
    friend class TriangleMesh;
    explicit VertexConstContainer(TriangleMesh const* mesh) : m_mesh_(mesh) {}

    TriangleMesh const* m_mesh_;
};
}  // namespace vox::force::discregrid
