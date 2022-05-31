//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/rigid_body_geometry.h"

namespace vox::force {

RigidBodyGeometry::RigidBodyGeometry() : m_mesh_() {}

RigidBodyGeometry::~RigidBodyGeometry() { m_mesh_.Release(); }

RigidBodyGeometry::Mesh &RigidBodyGeometry::GetMesh() { return m_mesh_; }

void RigidBodyGeometry::InitMesh(unsigned int n_vertices,
                                 unsigned int n_faces,
                                 const Vector3r *vertices,
                                 const unsigned int *indices,
                                 const Mesh::UVIndices &uv_indices,
                                 const Mesh::UVs &uvs,
                                 const Vector3r &scale,
                                 bool flat_shading) {
    m_mesh_.Release();
    m_mesh_.InitMesh(n_vertices, n_faces * 2, n_faces);
    m_vertex_data_local_.Resize(n_vertices);
    m_vertex_data_.Resize(n_vertices);
    m_mesh_.SetFlatShading(flat_shading);
    for (unsigned int i = 0; i < n_vertices; i++) {
        m_vertex_data_local_.GetPosition(i) = vertices[i].cwiseProduct(scale);
        m_vertex_data_.GetPosition(i) = m_vertex_data_local_.GetPosition(i);
    }

    for (unsigned int i = 0; i < n_faces; i++) {
        m_mesh_.AddFace(&indices[3 * i]);
    }
    m_mesh_.CopyUVs(uv_indices, uvs);
    m_mesh_.BuildNeighbors();
    UpdateMeshNormals(m_vertex_data_);
}

void RigidBodyGeometry::UpdateMeshNormals(const VertexData &vd) {
    m_mesh_.UpdateNormals(vd, 0);
    m_mesh_.UpdateVertexNormals(vd);
}

void RigidBodyGeometry::UpdateMeshTransformation(const Vector3r &x, const Matrix3r &r) {
    for (unsigned int i = 0; i < m_vertex_data_local_.Size(); i++) {
        m_vertex_data_.GetPosition(i) = r * m_vertex_data_local_.GetPosition(i) + x;
    }
    UpdateMeshNormals(m_vertex_data_);
}

VertexData &RigidBodyGeometry::GetVertexData() { return m_vertex_data_; }

const VertexData &RigidBodyGeometry::GetVertexData() const { return m_vertex_data_; }

VertexData &RigidBodyGeometry::GetVertexDataLocal() { return m_vertex_data_local_; }

const VertexData &RigidBodyGeometry::GetVertexDataLocal() const { return m_vertex_data_local_; }
}  // namespace vox::force