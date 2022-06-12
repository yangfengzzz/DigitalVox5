//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.compute/common.h"
#include "vox.compute/indexed_face_mesh.h"
#include "vox.compute/particle_data.h"

namespace vox::compute {
class RigidBodyGeometry {
public:
    RigidBodyGeometry();
    virtual ~RigidBodyGeometry();

    typedef IndexedFaceMesh Mesh;

protected:
    Mesh m_mesh_;
    VertexData m_vertex_data_local_;
    VertexData m_vertex_data_;

public:
    Mesh &GetMesh();
    VertexData &GetVertexData();
    [[nodiscard]] const VertexData &GetVertexData() const;
    VertexData &GetVertexDataLocal();
    [[nodiscard]] const VertexData &GetVertexDataLocal() const;

    void InitMesh(unsigned int n_vertices,
                  unsigned int n_faces,
                  const Vector3r *vertices,
                  const unsigned int *indices,
                  const Mesh::UVIndices &uv_indices,
                  const Mesh::UVs &uvs,
                  const Vector3r &scale = Vector3r(1.0, 1.0, 1.0),
                  bool flat_shading = false);
    void UpdateMeshTransformation(const Vector3r &x, const Matrix3r &r);
    void UpdateMeshNormals(const VertexData &vd);
};
}  // namespace vox::compute
