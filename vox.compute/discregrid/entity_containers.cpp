//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/discregrid/entity_containers.h"

#include "vox.compute/discregrid/triangle_mesh.h"

namespace vox::compute::discregrid {

FaceIterator FaceContainer::End() const { return {static_cast<unsigned int>(m_mesh_->NFaces()), m_mesh_}; }

FaceConstIterator FaceConstContainer::End() const { return {static_cast<unsigned int>(m_mesh_->NFaces()), m_mesh_}; }

VertexIterator VertexContainer::End() const { return {static_cast<unsigned int>(m_mesh_->NVertices()), m_mesh_}; }

VertexConstIterator VertexConstContainer::End() const {
    return {static_cast<unsigned int>(m_mesh_->NVertices()), m_mesh_};
}

}  // namespace vox::compute::discregrid
