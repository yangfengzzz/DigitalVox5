//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.compute/common.h"
#include "vox.compute/indexed_face_mesh.h"
#include "vox.compute/indexed_tet_mesh.h"
#include "vox.compute/particle_data.h"

namespace vox::compute {
class TetModel {
public:
    TetModel();
    virtual ~TetModel();

    typedef IndexedFaceMesh SurfaceMesh;
    typedef IndexedTetMesh ParticleMesh;

    struct Attachment {
        unsigned int m_index;
        unsigned int m_tri_index;
        Real m_bary[3];
        Real m_dist;
        Real m_min_error;
    };

    Vector3r& GetInitialX() { return m_initial_x_; }
    void SetInitialX(const Vector3r& val) { m_initial_x_ = val; }
    Matrix3r& GetInitialR() { return m_initial_r_; }
    void SetInitialR(const Matrix3r& val) { m_initial_r_ = val; }
    Vector3r& GetInitialScale() { return m_initial_scale_; }
    void SetInitialScale(const Vector3r& val) { m_initial_scale_ = val; }

protected:
    /** offset which must be added to get the correct index in the particles array */
    unsigned int m_index_offset_{};
    /** Tet mesh of particles which represents the simulation model */
    ParticleMesh m_particle_mesh_;
    SurfaceMesh m_surface_mesh_;
    VertexData m_vis_vertices_;
    SurfaceMesh m_vis_mesh_;
    Real m_restitution_coeff_;
    Real m_friction_coeff_;
    std::vector<Attachment> m_attachments_;
    Vector3r m_initial_x_;
    Matrix3r m_initial_r_;
    Vector3r m_initial_scale_;

    void CreateSurfaceMesh();
    void SolveQuadraticForZero(const Vector3r& F,
                               const Vector3r& Fu,
                               const Vector3r& Fv,
                               const Vector3r& Fuu,
                               const Vector3r& Fuv,
                               const Vector3r& Fvv,
                               Real& u,
                               Real& v);
    bool PointInTriangle(const Vector3r& p0,
                         const Vector3r& p1,
                         const Vector3r& p2,
                         const Vector3r& p,
                         Vector3r& inter,
                         Vector3r& bary);

public:
    SurfaceMesh& GetSurfaceMesh();
    VertexData& GetVisVertices();
    SurfaceMesh& GetVisMesh();
    ParticleMesh& GetParticleMesh() { return m_particle_mesh_; }
    [[nodiscard]] const ParticleMesh& GetParticleMesh() const { return m_particle_mesh_; }
    void CleanupModel();

    [[nodiscard]] unsigned int GetIndexOffset() const;

    void InitMesh(unsigned int n_points, unsigned int n_tets, unsigned int index_offset, unsigned int* indices);
    void UpdateMeshNormals(const ParticleData& pd);

    /** Attach a visualization mesh to the surface of the body.
     * Important: The vertex normals have to be updated before
     * calling this function by calling UpdateMeshNormals().
     */
    void AttachVisMesh(const ParticleData& pd);

    /** Update the visualization mesh of the body.
     * Important: The vertex normals have to be updated before
     * calling this function by calling UpdateMeshNormals().
     */
    void UpdateVisMesh(const ParticleData& pd);

    [[nodiscard]] FORCE_INLINE Real GetRestitutionCoeff() const { return m_restitution_coeff_; }

    FORCE_INLINE void SetRestitutionCoeff(Real val) { m_restitution_coeff_ = val; }

    [[nodiscard]] FORCE_INLINE Real GetFrictionCoeff() const { return m_friction_coeff_; }

    FORCE_INLINE void SetFrictionCoeff(Real val) { m_friction_coeff_ = val; }
};
}  // namespace vox::compute
