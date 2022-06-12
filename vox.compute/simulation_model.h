//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.compute/common.h"
#include "vox.compute/line_model.h"
#include "vox.compute/particle_data.h"
#include "vox.compute/rigid_body.h"
#include "vox.compute/tet_model.h"
#include "vox.compute/triangle_model.h"

namespace vox::compute {
class Constraint;

class SimulationModel {
public:
    SimulationModel();
    SimulationModel(const SimulationModel &) = delete;
    SimulationModel &operator=(const SimulationModel &) = delete;
    virtual ~SimulationModel();

    void Init();

    typedef std::vector<Constraint *> ConstraintVector;
    typedef std::vector<RigidBodyContactConstraint> RigidBodyContactConstraintVector;
    typedef std::vector<ParticleRigidBodyContactConstraint> ParticleRigidBodyContactConstraintVector;
    typedef std::vector<ParticleTetContactConstraint> ParticleSolidContactConstraintVector;
    typedef std::vector<RigidBody *> RigidBodyVector;
    typedef std::vector<TriangleModel *> TriangleModelVector;
    typedef std::vector<TetModel *> TetModelVector;
    typedef std::vector<LineModel *> LineModelVector;
    typedef std::vector<unsigned int> ConstraintGroup;
    typedef std::vector<ConstraintGroup> ConstraintGroupVector;

protected:
    RigidBodyVector m_rigid_bodies_;
    TriangleModelVector m_triangle_models_;
    TetModelVector m_tet_models_;
    LineModelVector m_line_models_;
    ParticleData m_particles_;
    OrientationData m_orientations_;
    ConstraintVector m_constraints_;
    RigidBodyContactConstraintVector m_rigid_body_contact_constraints_;
    ParticleRigidBodyContactConstraintVector m_particle_rigid_body_contact_constraints_;
    ParticleSolidContactConstraintVector m_particle_solid_contact_constraints_;
    ConstraintGroupVector m_constraint_groups_;

    Real m_contact_stiffness_rigid_body_;
    Real m_contact_stiffness_particle_rigid_body_;

public:
    void Reset();
    void Cleanup();

    RigidBodyVector &GetRigidBodies();
    ParticleData &GetParticles();
    OrientationData &GetOrientations();
    TriangleModelVector &GetTriangleModels();
    TetModelVector &GetTetModels();
    LineModelVector &GetLineModels();
    ConstraintVector &GetConstraints();
    RigidBodyContactConstraintVector &GetRigidBodyContactConstraints();
    ParticleRigidBodyContactConstraintVector &GetParticleRigidBodyContactConstraints();
    ParticleSolidContactConstraintVector &GetParticleSolidContactConstraints();
    ConstraintGroupVector &GetConstraintGroups();
    bool m_groups_initialized_;

    void ResetContacts();

    void AddTriangleModel(unsigned int n_points,
                          unsigned int n_faces,
                          Vector3r *points,
                          unsigned int *indices,
                          const TriangleModel::ParticleMesh::UVIndices &uv_indices,
                          const TriangleModel::ParticleMesh::UVs &uvs);
    void AddRegularTriangleModel(int width,
                                 int height,
                                 const Vector3r &translation = Vector3r::Zero(),
                                 const Matrix3r &rotation = Matrix3r::Identity(),
                                 const Vector2r &scale = Vector2r::Ones());

    void AddTetModel(unsigned int n_points, unsigned int n_tets, Vector3r *points, unsigned int *indices);
    void AddRegularTetModel(int width,
                            int height,
                            int depth,
                            const Vector3r &translation = Vector3r::Zero(),
                            const Matrix3r &rotation = Matrix3r::Identity(),
                            const Vector3r &scale = Vector3r::Ones());

    void AddLineModel(unsigned int n_points,
                      unsigned int n_quaternions,
                      Vector3r *points,
                      Quaternionr *quaternions,
                      unsigned int *indices,
                      unsigned int *indices_quaternions);

    void UpdateConstraints();
    void InitConstraintGroups();

    bool AddBallJoint(unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &pos);
    bool AddBallOnLineJoint(unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &pos, const Vector3r &dir);
    bool AddHingeJoint(unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &pos, const Vector3r &axis);
    bool AddTargetAngleMotorHingeJoint(unsigned int rb_index_1,
                                       unsigned int rb_index_2,
                                       const Vector3r &pos,
                                       const Vector3r &axis);
    bool AddTargetVelocityMotorHingeJoint(unsigned int rb_index_1,
                                          unsigned int rb_index_2,
                                          const Vector3r &pos,
                                          const Vector3r &axis);
    bool AddUniversalJoint(unsigned int rb_index_1,
                           unsigned int rb_index_2,
                           const Vector3r &pos,
                           const Vector3r &axis1,
                           const Vector3r &axis2);
    bool AddSliderJoint(unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &axis);
    bool AddTargetPositionMotorSliderJoint(unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &axis);
    bool AddTargetVelocityMotorSliderJoint(unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &axis);
    bool AddRigidBodyParticleBallJoint(unsigned int rb_index, unsigned int particle_index);
    bool AddRigidBodySpring(unsigned int rb_index_1,
                            unsigned int rb_index_2,
                            const Vector3r &pos1,
                            const Vector3r &pos2,
                            Real stiffness);
    bool AddDistanceJoint(unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &pos1, const Vector3r &pos2);
    bool AddDamperJoint(unsigned int rb_index_1, unsigned int rb_index_2, const Vector3r &axis, Real stiffness);
    bool AddRigidBodyContactConstraint(unsigned int rb_index_1,
                                       unsigned int rb_index_2,
                                       const Vector3r &cp1,
                                       const Vector3r &cp2,
                                       const Vector3r &normal,
                                       Real dist,
                                       Real restitution_coeff,
                                       Real friction_coeff);
    bool AddParticleRigidBodyContactConstraint(unsigned int particle_index,
                                               unsigned int rb_index,
                                               const Vector3r &cp1,
                                               const Vector3r &cp2,
                                               const Vector3r &normal,
                                               Real dist,
                                               Real restitution_coeff,
                                               Real friction_coeff);

    bool AddParticleSolidContactConstraint(unsigned int particle_index,
                                           unsigned int solid_index,
                                           unsigned int tet_index,
                                           const Vector3r &bary,
                                           const Vector3r &cp1,
                                           const Vector3r &cp2,
                                           const Vector3r &normal,
                                           Real dist,
                                           Real restitution_coeff,
                                           Real friction_coeff);

    bool AddDistanceConstraint(unsigned int particle1, unsigned int particle2, Real stiffness);
    bool AddDistanceConstraintXPBD(unsigned int particle1, unsigned int particle2, Real stiffness);
    bool AddDihedralConstraint(unsigned int particle1,
                               unsigned int particle2,
                               unsigned int particle3,
                               unsigned int particle4,
                               Real stiffness);
    bool AddIsometricBendingConstraint(unsigned int particle1,
                                       unsigned int particle2,
                                       unsigned int particle3,
                                       unsigned int particle4,
                                       Real stiffness);
    bool AddIsometricBendingConstraintXpbd(unsigned int particle1,
                                           unsigned int particle2,
                                           unsigned int particle3,
                                           unsigned int particle4,
                                           Real stiffness);
    bool AddFemTriangleConstraint(unsigned int particle1,
                                  unsigned int particle2,
                                  unsigned int particle3,
                                  Real xx_stiffness,
                                  Real yy_stiffness,
                                  Real xy_stiffness,
                                  Real xy_poisson_ratio,
                                  Real yx_poisson_ratio);
    bool AddStrainTriangleConstraint(unsigned int particle1,
                                     unsigned int particle2,
                                     unsigned int particle3,
                                     Real xx_stiffness,
                                     Real yy_stiffness,
                                     Real xy_stiffness,
                                     bool normalize_stretch,
                                     bool normalize_shear);
    bool AddVolumeConstraint(unsigned int particle1,
                             unsigned int particle2,
                             unsigned int particle3,
                             unsigned int particle4,
                             Real stiffness);
    bool AddVolumeConstraintXPBD(unsigned int particle1,
                                 unsigned int particle2,
                                 unsigned int particle3,
                                 unsigned int particle4,
                                 Real stiffness);
    bool AddFemTetConstraint(unsigned int particle1,
                             unsigned int particle2,
                             unsigned int particle3,
                             unsigned int particle4,
                             Real stiffness,
                             Real poisson_ratio);
    bool AddStrainTetConstraint(unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stretch_stiffness,
                                Real shear_stiffness,
                                bool normalize_stretch,
                                bool normalize_shear);
    bool AddShapeMatchingConstraint(unsigned int number_of_particles,
                                    const unsigned int particle_indices[],
                                    const unsigned int num_clusters[],
                                    Real stiffness);
    bool AddStretchShearConstraint(unsigned int particle1,
                                   unsigned int particle2,
                                   unsigned int quaternion1,
                                   Real stretching_stiffness,
                                   Real shearing_stiffness_1,
                                   Real shearing_stiffness_2);
    bool AddBendTwistConstraint(unsigned int quaternion1,
                                unsigned int quaternion2,
                                Real twisting_stiffness,
                                Real bending_stiffness_1,
                                Real bending_stiffness_2);
    bool AddStretchBendingTwistingConstraint(unsigned int rb_index_1,
                                             unsigned int rb_index_2,
                                             const Vector3r &pos,
                                             Real average_radius,
                                             Real average_segment_length,
                                             Real youngs_modulus,
                                             Real torsion_modulus);
    bool AddDirectPositionBasedSolverForStiffRodsConstraint(
            const std::vector<std::pair<unsigned int, unsigned int>> &joint_segment_indices,
            const std::vector<Vector3r> &joint_positions,
            const std::vector<Real> &average_radii,
            const std::vector<Real> &average_segment_lengths,
            const std::vector<Real> &youngs_moduli,
            const std::vector<Real> &torsion_moduli);

    [[nodiscard]] Real GetContactStiffnessRigidBody() const { return m_contact_stiffness_rigid_body_; }
    void SetContactStiffnessRigidBody(Real val) { m_contact_stiffness_rigid_body_ = val; }
    [[nodiscard]] Real GetContactStiffnessParticleRigidBody() const { return m_contact_stiffness_particle_rigid_body_; }
    void SetContactStiffnessParticleRigidBody(Real val) { m_contact_stiffness_particle_rigid_body_ = val; }

    void AddClothConstraints(const TriangleModel *tm,
                             unsigned int cloth_method,
                             Real distance_stiffness,
                             Real xx_stiffness,
                             Real yy_stiffness,
                             Real xy_stiffness,
                             Real xy_poisson_ratio,
                             Real yx_poisson_ratio,
                             bool normalize_stretch,
                             bool normalize_shear);
    void AddBendingConstraints(const TriangleModel *tm, unsigned int bending_method, Real stiffness);
    void AddSolidConstraints(const TetModel *tm,
                             unsigned int solid_method,
                             Real stiffness,
                             Real poisson_ratio,
                             Real volume_stiffness,
                             bool normalize_stretch,
                             bool normalize_shear);

    template <typename ConstraintType, typename T, T ConstraintType::*MemPtr>
    void SetConstraintValue(const T v) {
        for (auto &m_constraint : m_constraints_) {
            auto *c = dynamic_cast<ConstraintType *>(m_constraint);
            if (c != nullptr) c->*MemPtr = v;
        }
    }
};
}  // namespace vox::compute
