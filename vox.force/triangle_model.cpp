//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/triangle_model.h"

#include "vox.force/position_based_dynamics.h"

namespace vox::force {

TriangleModel::TriangleModel() : m_particle_mesh_() {
    m_restitution_coeff_ = static_cast<Real>(0.6);
    m_friction_coeff_ = static_cast<Real>(0.2);
}

TriangleModel::~TriangleModel() { CleanupModel(); }

void TriangleModel::CleanupModel() { m_particle_mesh_.Release(); }

void TriangleModel::UpdateMeshNormals(const ParticleData& pd) {
    m_particle_mesh_.UpdateNormals(pd, m_index_offset_);
    m_particle_mesh_.UpdateVertexNormals(pd);
}

void TriangleModel::InitMesh(unsigned int n_points,
                             unsigned int n_faces,
                             unsigned int index_offset,
                             unsigned int* indices,
                             const ParticleMesh::UVIndices& uv_indices,
                             const ParticleMesh::UVs& uvs) {
    m_index_offset_ = index_offset;
    m_particle_mesh_.Release();

    m_particle_mesh_.InitMesh(n_points, n_faces * 2, n_faces);

    for (unsigned int i = 0; i < n_faces; i++) {
        m_particle_mesh_.AddFace(&indices[3 * i]);
    }
    m_particle_mesh_.CopyUVs(uv_indices, uvs);
    m_particle_mesh_.BuildNeighbors();
}

unsigned int TriangleModel::GetIndexOffset() const { return m_index_offset_; }

}  // namespace vox::force