//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/discregrid/entity_iterators.h"

#include "vox.compute/discregrid/triangle_mesh.h"

namespace vox::compute::discregrid {

unsigned int FaceIterator::Vertex(unsigned int i) const { return m_mesh_->FaceVertex(m_index_, i); }

FaceIterator::reference FaceIterator::operator*() { return m_mesh_->Face(m_index_); }

FaceConstIterator::reference FaceConstIterator::operator*() { return m_mesh_->Face(m_index_); }

unsigned int& FaceIterator::Vertex(unsigned int i) { return m_mesh_->FaceVertex(m_index_, i); }

VertexIterator::reference VertexIterator::operator*() { return m_mesh_->Vertex(m_index_); }

VertexConstIterator::reference VertexConstIterator::operator*() { return m_mesh_->Vertex(m_index_); }

unsigned int VertexIterator::Index() const { return m_index_; }

IncidentFaceIterator::Self& IncidentFaceIterator::operator++() {
    Halfedge o = m_mesh_->Opposite(m_h_);
    if (o.IsBoundary()) {
        m_h_ = Halfedge();
        return *this;
    }
    m_h_ = o.Next();
    if (m_h_ == m_begin_) {
        m_h_ = Halfedge();
    }
    return *this;
}

IncidentFaceIterator::IncidentFaceIterator(unsigned int v, TriangleMesh const* mesh)
    : m_mesh_(mesh), m_h_(mesh->IncidentHalfedge(v)), m_begin_(mesh->IncidentHalfedge(v)) {
    if (m_h_.IsBoundary()) m_h_ = mesh->Opposite(m_h_).Next();
}

}  // namespace vox::compute::discregrid
