//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.force/common.h"
#include "vox.force/indexed_face_mesh.h"
#include "vox.force/particle_data.h"

namespace vox::force {
class RigidBodyGeometry {
public:
    RigidBodyGeometry();
    virtual ~RigidBodyGeometry();

    typedef IndexedFaceMesh Mesh;

protected:
    Mesh m_mesh;
    VertexData m_vertexData_local;
    VertexData m_vertexData;

public:
    Mesh &getMesh();
    VertexData &getVertexData();
    [[nodiscard]] const VertexData &getVertexData() const;
    VertexData &getVertexDataLocal();
    [[nodiscard]] const VertexData &getVertexDataLocal() const;

    void initMesh(unsigned int nVertices,
                  unsigned int nFaces,
                  const Vector3r *vertices,
                  const unsigned int *indices,
                  const Mesh::UVIndices &uvIndices,
                  const Mesh::UVs &uvs,
                  const Vector3r &scale = Vector3r(1.0, 1.0, 1.0),
                  bool flatShading = false);
    void updateMeshTransformation(const Vector3r &x, const Matrix3r &R);
    void updateMeshNormals(const VertexData &vd);
};
}  // namespace vox::force