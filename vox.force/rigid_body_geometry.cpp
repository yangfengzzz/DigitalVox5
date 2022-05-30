//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/rigid_body_geometry.h"

namespace vox::force {

RigidBodyGeometry::RigidBodyGeometry() : m_mesh() {}

RigidBodyGeometry::~RigidBodyGeometry() { m_mesh.Release(); }

RigidBodyGeometry::Mesh &RigidBodyGeometry::getMesh() { return m_mesh; }

void RigidBodyGeometry::initMesh(const unsigned int nVertices,
                                 const unsigned int nFaces,
                                 const Vector3r *vertices,
                                 const unsigned int *indices,
                                 const Mesh::UVIndices &uvIndices,
                                 const Mesh::UVs &uvs,
                                 const Vector3r &scale,
                                 const bool flatShading) {
    m_mesh.Release();
    m_mesh.InitMesh(nVertices, nFaces * 2, nFaces);
    m_vertexData_local.resize(nVertices);
    m_vertexData.resize(nVertices);
    m_mesh.SetFlatShading(flatShading);
    for (unsigned int i = 0; i < nVertices; i++) {
        m_vertexData_local.GetPosition(i) = vertices[i].cwiseProduct(scale);
        m_vertexData.GetPosition(i) = m_vertexData_local.GetPosition(i);
    }

    for (unsigned int i = 0; i < nFaces; i++) {
        m_mesh.AddFace(&indices[3 * i]);
    }
    m_mesh.CopyUVs(uvIndices, uvs);
    m_mesh.BuildNeighbors();
    updateMeshNormals(m_vertexData);
}

void RigidBodyGeometry::updateMeshNormals(const VertexData &vd) {
    m_mesh.UpdateNormals(vd, 0);
    m_mesh.UpdateVertexNormals(vd);
}

void RigidBodyGeometry::updateMeshTransformation(const Vector3r &x, const Matrix3r &R) {
    for (unsigned int i = 0; i < m_vertexData_local.size(); i++) {
        m_vertexData.GetPosition(i) = R * m_vertexData_local.GetPosition(i) + x;
    }
    updateMeshNormals(m_vertexData);
}

VertexData &RigidBodyGeometry::getVertexData() { return m_vertexData; }

const VertexData &RigidBodyGeometry::getVertexData() const { return m_vertexData; }

VertexData &RigidBodyGeometry::getVertexDataLocal() { return m_vertexData_local; }

const VertexData &RigidBodyGeometry::getVertexDataLocal() const { return m_vertexData_local; }
}  // namespace vox::force