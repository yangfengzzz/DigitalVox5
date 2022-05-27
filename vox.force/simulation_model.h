//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.force/common.h"
#include "vox.force/line_model.h"
#include "vox.force/particle_data.h"
#include "vox.force/rigid_body.h"
#include "vox.force/tet_model.h"
#include "vox.force/triangle_model.h"

namespace vox::force {
class Constraint;

class SimulationModel {
public:
    SimulationModel();
    SimulationModel(const SimulationModel &) = delete;
    SimulationModel &operator=(const SimulationModel &) = delete;
    virtual ~SimulationModel();

    void init();

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
    RigidBodyVector m_rigidBodies;
    TriangleModelVector m_triangleModels;
    TetModelVector m_tetModels;
    LineModelVector m_lineModels;
    ParticleData m_particles;
    OrientationData m_orientations;
    ConstraintVector m_constraints;
    RigidBodyContactConstraintVector m_rigidBodyContactConstraints;
    ParticleRigidBodyContactConstraintVector m_particleRigidBodyContactConstraints;
    ParticleSolidContactConstraintVector m_particleSolidContactConstraints;
    ConstraintGroupVector m_constraintGroups;

    Real m_contactStiffnessRigidBody;
    Real m_contactStiffnessParticleRigidBody;

public:
    void reset();
    void cleanup();

    RigidBodyVector &getRigidBodies();
    ParticleData &getParticles();
    OrientationData &getOrientations();
    TriangleModelVector &getTriangleModels();
    TetModelVector &getTetModels();
    LineModelVector &getLineModels();
    ConstraintVector &getConstraints();
    RigidBodyContactConstraintVector &getRigidBodyContactConstraints();
    ParticleRigidBodyContactConstraintVector &getParticleRigidBodyContactConstraints();
    ParticleSolidContactConstraintVector &getParticleSolidContactConstraints();
    ConstraintGroupVector &getConstraintGroups();
    bool m_groupsInitialized;

    void resetContacts();

    void addTriangleModel(unsigned int nPoints,
                          unsigned int nFaces,
                          Vector3r *points,
                          unsigned int *indices,
                          const TriangleModel::ParticleMesh::UVIndices &uvIndices,
                          const TriangleModel::ParticleMesh::UVs &uvs);
    void addRegularTriangleModel(int width,
                                 int height,
                                 const Vector3r &translation = Vector3r::Zero(),
                                 const Matrix3r &rotation = Matrix3r::Identity(),
                                 const Vector2r &scale = Vector2r::Ones());

    void addTetModel(unsigned int nPoints, unsigned int nTets, Vector3r *points, unsigned int *indices);
    void addRegularTetModel(int width,
                            int height,
                            int depth,
                            const Vector3r &translation = Vector3r::Zero(),
                            const Matrix3r &rotation = Matrix3r::Identity(),
                            const Vector3r &scale = Vector3r::Ones());

    void addLineModel(unsigned int nPoints,
                      unsigned int nQuaternions,
                      Vector3r *points,
                      Quaternionr *quaternions,
                      unsigned int *indices,
                      unsigned int *indicesQuaternions);

    void updateConstraints();
    void initConstraintGroups();

    bool addBallJoint(unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &pos);
    bool addBallOnLineJoint(unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &pos, const Vector3r &dir);
    bool addHingeJoint(unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &pos, const Vector3r &axis);
    bool addTargetAngleMotorHingeJoint(unsigned int rbIndex1,
                                       unsigned int rbIndex2,
                                       const Vector3r &pos,
                                       const Vector3r &axis);
    bool addTargetVelocityMotorHingeJoint(unsigned int rbIndex1,
                                          unsigned int rbIndex2,
                                          const Vector3r &pos,
                                          const Vector3r &axis);
    bool addUniversalJoint(unsigned int rbIndex1,
                           unsigned int rbIndex2,
                           const Vector3r &pos,
                           const Vector3r &axis1,
                           const Vector3r &axis2);
    bool addSliderJoint(unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &axis);
    bool addTargetPositionMotorSliderJoint(unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &axis);
    bool addTargetVelocityMotorSliderJoint(unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &axis);
    bool addRigidBodyParticleBallJoint(unsigned int rbIndex, unsigned int particleIndex);
    bool addRigidBodySpring(
            unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &pos1, const Vector3r &pos2, Real stiffness);
    bool addDistanceJoint(unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &pos1, const Vector3r &pos2);
    bool addDamperJoint(unsigned int rbIndex1, unsigned int rbIndex2, const Vector3r &axis, Real stiffness);
    bool addRigidBodyContactConstraint(unsigned int rbIndex1,
                                       unsigned int rbIndex2,
                                       const Vector3r &cp1,
                                       const Vector3r &cp2,
                                       const Vector3r &normal,
                                       Real dist,
                                       Real restitutionCoeff,
                                       Real frictionCoeff);
    bool addParticleRigidBodyContactConstraint(unsigned int particleIndex,
                                               unsigned int rbIndex,
                                               const Vector3r &cp1,
                                               const Vector3r &cp2,
                                               const Vector3r &normal,
                                               Real dist,
                                               Real restitutionCoeff,
                                               Real frictionCoeff);

    bool addParticleSolidContactConstraint(unsigned int particleIndex,
                                           unsigned int solidIndex,
                                           unsigned int tetIndex,
                                           const Vector3r &bary,
                                           const Vector3r &cp1,
                                           const Vector3r &cp2,
                                           const Vector3r &normal,
                                           Real dist,
                                           Real restitutionCoeff,
                                           Real frictionCoeff);

    bool addDistanceConstraint(unsigned int particle1, unsigned int particle2, Real stiffness);
    bool addDistanceConstraint_XPBD(unsigned int particle1, unsigned int particle2, Real stiffness);
    bool addDihedralConstraint(unsigned int particle1,
                               unsigned int particle2,
                               unsigned int particle3,
                               unsigned int particle4,
                               Real stiffness);
    bool addIsometricBendingConstraint(unsigned int particle1,
                                       unsigned int particle2,
                                       unsigned int particle3,
                                       unsigned int particle4,
                                       Real stiffness);
    bool addIsometricBendingConstraint_XPBD(unsigned int particle1,
                                            unsigned int particle2,
                                            unsigned int particle3,
                                            unsigned int particle4,
                                            Real stiffness);
    bool addFEMTriangleConstraint(unsigned int particle1,
                                  unsigned int particle2,
                                  unsigned int particle3,
                                  Real xxStiffness,
                                  Real yyStiffness,
                                  Real xyStiffness,
                                  Real xyPoissonRatio,
                                  Real yxPoissonRatio);
    bool addStrainTriangleConstraint(unsigned int particle1,
                                     unsigned int particle2,
                                     unsigned int particle3,
                                     Real xxStiffness,
                                     Real yyStiffness,
                                     Real xyStiffness,
                                     bool normalizeStretch,
                                     bool normalizeShear);
    bool addVolumeConstraint(unsigned int particle1,
                             unsigned int particle2,
                             unsigned int particle3,
                             unsigned int particle4,
                             Real stiffness);
    bool addVolumeConstraint_XPBD(unsigned int particle1,
                                  unsigned int particle2,
                                  unsigned int particle3,
                                  unsigned int particle4,
                                  Real stiffness);
    bool addFEMTetConstraint(unsigned int particle1,
                             unsigned int particle2,
                             unsigned int particle3,
                             unsigned int particle4,
                             Real stiffness,
                             Real poissonRatio);
    bool addStrainTetConstraint(unsigned int particle1,
                                unsigned int particle2,
                                unsigned int particle3,
                                unsigned int particle4,
                                Real stretchStiffness,
                                Real shearStiffness,
                                bool normalizeStretch,
                                bool normalizeShear);
    bool addShapeMatchingConstraint(unsigned int numberOfParticles,
                                    const unsigned int particleIndices[],
                                    const unsigned int numClusters[],
                                    Real stiffness);
    bool addStretchShearConstraint(unsigned int particle1,
                                   unsigned int particle2,
                                   unsigned int quaternion1,
                                   Real stretchingStiffness,
                                   Real shearingStiffness1,
                                   Real shearingStiffness2);
    bool addBendTwistConstraint(unsigned int quaternion1,
                                unsigned int quaternion2,
                                Real twistingStiffness,
                                Real bendingStiffness1,
                                Real bendingStiffness2);
    bool addStretchBendingTwistingConstraint(unsigned int rbIndex1,
                                             unsigned int rbIndex2,
                                             const Vector3r &pos,
                                             Real averageRadius,
                                             Real averageSegmentLength,
                                             Real youngsModulus,
                                             Real torsionModulus);
    bool addDirectPositionBasedSolverForStiffRodsConstraint(
            const std::vector<std::pair<unsigned int, unsigned int>> &jointSegmentIndices,
            const std::vector<Vector3r> &jointPositions,
            const std::vector<Real> &averageRadii,
            const std::vector<Real> &averageSegmentLengths,
            const std::vector<Real> &youngsModuli,
            const std::vector<Real> &torsionModuli);

    [[nodiscard]] Real getContactStiffnessRigidBody() const { return m_contactStiffnessRigidBody; }
    void setContactStiffnessRigidBody(Real val) { m_contactStiffnessRigidBody = val; }
    [[nodiscard]] Real getContactStiffnessParticleRigidBody() const { return m_contactStiffnessParticleRigidBody; }
    void setContactStiffnessParticleRigidBody(Real val) { m_contactStiffnessParticleRigidBody = val; }

    void addClothConstraints(const TriangleModel *tm,
                             unsigned int clothMethod,
                             Real distanceStiffness,
                             Real xxStiffness,
                             Real yyStiffness,
                             Real xyStiffness,
                             Real xyPoissonRatio,
                             Real yxPoissonRatio,
                             bool normalizeStretch,
                             bool normalizeShear);
    void addBendingConstraints(const TriangleModel *tm, unsigned int bendingMethod, const Real stiffness);
    void addSolidConstraints(const TetModel *tm,
                             unsigned int solidMethod,
                             Real stiffness,
                             Real poissonRatio,
                             Real volumeStiffness,
                             bool normalizeStretch,
                             bool normalizeShear);

    template <typename ConstraintType, typename T, T ConstraintType::*MemPtr>
    void setConstraintValue(const T v) {
        for (auto &m_constraint : m_constraints) {
            auto *c = dynamic_cast<ConstraintType *>(m_constraint);
            if (c != nullptr) c->*MemPtr = v;
        }
    }
};
}  // namespace vox::force