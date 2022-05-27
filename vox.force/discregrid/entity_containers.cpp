//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/discregrid/entity_containers.h"

#include "vox.force/discregrid/triangle_mesh.h"

namespace vox::force::discregrid {

FaceIterator FaceContainer::end() const { return {static_cast<unsigned int>(m_mesh->nFaces()), m_mesh}; }

FaceConstIterator FaceConstContainer::end() const { return {static_cast<unsigned int>(m_mesh->nFaces()), m_mesh}; }

VertexIterator VertexContainer::end() const { return {static_cast<unsigned int>(m_mesh->nVertices()), m_mesh}; }

VertexConstIterator VertexConstContainer::end() const {
    return {static_cast<unsigned int>(m_mesh->nVertices()), m_mesh};
}

}  // namespace vox::force::discregrid
