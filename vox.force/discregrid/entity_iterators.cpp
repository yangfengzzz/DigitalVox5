//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/discregrid/entity_iterators.h"

#include "vox.force/discregrid/triangle_mesh.h"

namespace vox::force::discregrid {

unsigned int FaceIterator::Vertex(unsigned int i) const { return m_mesh_->faceVertex(m_index_, i); }

FaceIterator::reference FaceIterator::operator*() { return m_mesh_->face(m_index_); }

FaceConstIterator::reference FaceConstIterator::operator*() { return m_mesh_->face(m_index_); }

unsigned int& FaceIterator::Vertex(unsigned int i) { return m_mesh_->faceVertex(m_index_, i); }

VertexIterator::reference VertexIterator::operator*() { return m_mesh_->vertex(m_index_); }

VertexConstIterator::reference VertexConstIterator::operator*() { return m_mesh_->vertex(m_index_); }

unsigned int VertexIterator::Index() const { return m_index_; }

IncidentFaceIterator::Self& IncidentFaceIterator::operator++() {
    Halfedge o = m_mesh_->opposite(m_h_);
    if (o.isBoundary()) {
        m_h_ = Halfedge();
        return *this;
    }
    m_h_ = o.next();
    if (m_h_ == m_begin_) {
        m_h_ = Halfedge();
    }
    return *this;
}

IncidentFaceIterator::IncidentFaceIterator(unsigned int v, TriangleMesh const* mesh)
    : m_mesh_(mesh), m_h_(mesh->incident_halfedge(v)), m_begin_(mesh->incident_halfedge(v)) {
    if (m_h_.isBoundary()) m_h_ = mesh->opposite(m_h_).next();
}

}  // namespace vox::force::discregrid
