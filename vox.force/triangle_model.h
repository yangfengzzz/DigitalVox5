//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.force/common.h"
#include "vox.force/constraints.h"
#include "vox.force/indexed_face_mesh.h"
#include "vox.force/particle_data.h"
#include "vox.force/rigid_body.h"

namespace vox::force {
class TriangleModel {
public:
    TriangleModel();
    virtual ~TriangleModel();

    typedef IndexedFaceMesh ParticleMesh;

protected:
    /** offset which must be added to get the correct index in the particles array */
    unsigned int m_indexOffset;
    /** Face mesh of particles which represents the simulation model */
    ParticleMesh m_particleMesh;
    Real m_restitutionCoeff;
    Real m_frictionCoeff;

public:
    ParticleMesh& getParticleMesh() { return m_particleMesh; }
    [[nodiscard]] const ParticleMesh& getParticleMesh() const { return m_particleMesh; }

    void cleanupModel();

    [[nodiscard]] unsigned int getIndexOffset() const;

    void initMesh(unsigned int nPoints,
                  unsigned int nFaces,
                  unsigned int indexOffset,
                  unsigned int* indices,
                  const ParticleMesh::UVIndices& uvIndices,
                  const ParticleMesh::UVs& uvs);
    void updateMeshNormals(const ParticleData& pd);

    [[nodiscard]] FORCE_INLINE Real getRestitutionCoeff() const { return m_restitutionCoeff; }

    FORCE_INLINE void setRestitutionCoeff(Real val) { m_restitutionCoeff = val; }

    [[nodiscard]] FORCE_INLINE Real getFrictionCoeff() const { return m_frictionCoeff; }

    FORCE_INLINE void setFrictionCoeff(Real val) { m_frictionCoeff = val; }
};
}  // namespace vox::force