//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.compute/common.h"
#include "vox.compute/constraints.h"
#include "vox.compute/indexed_face_mesh.h"
#include "vox.compute/particle_data.h"
#include "vox.compute/rigid_body.h"

namespace vox::compute {
class TriangleModel {
public:
    TriangleModel();
    virtual ~TriangleModel();

    typedef IndexedFaceMesh ParticleMesh;

protected:
    /** offset which must be added to get the correct index in the particles array */
    unsigned int m_index_offset_{};
    /** Face mesh of particles which represents the simulation model */
    ParticleMesh m_particle_mesh_;
    Real m_restitution_coeff_;
    Real m_friction_coeff_;

public:
    ParticleMesh& GetParticleMesh() { return m_particle_mesh_; }
    [[nodiscard]] const ParticleMesh& GetParticleMesh() const { return m_particle_mesh_; }

    void CleanupModel();

    [[nodiscard]] unsigned int GetIndexOffset() const;

    void InitMesh(unsigned int n_points,
                  unsigned int n_faces,
                  unsigned int index_offset,
                  unsigned int* indices,
                  const ParticleMesh::UVIndices& uv_indices,
                  const ParticleMesh::UVs& uvs);
    void UpdateMeshNormals(const ParticleData& pd);

    [[nodiscard]] FORCE_INLINE Real GetRestitutionCoeff() const { return m_restitution_coeff_; }

    FORCE_INLINE void SetRestitutionCoeff(Real val) { m_restitution_coeff_ = val; }

    [[nodiscard]] FORCE_INLINE Real GetFrictionCoeff() const { return m_friction_coeff_; }

    FORCE_INLINE void SetFrictionCoeff(Real val) { m_friction_coeff_ = val; }
};
}  // namespace vox::compute
