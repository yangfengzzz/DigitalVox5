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

    void addTriangleModel(const unsigned int nPoints,
                          const unsigned int nFaces,
                          Vector3r *points,
                          unsigned int *indices,
                          const TriangleModel::ParticleMesh::UVIndices &uvIndices,
                          const TriangleModel::ParticleMesh::UVs &uvs);
    void addRegularTriangleModel(const int width,
                                 const int height,
                                 const Vector3r &translation = Vector3r::Zero(),
                                 const Matrix3r &rotation = Matrix3r::Identity(),
                                 const Vector2r &scale = Vector2r::Ones());

    void addTetModel(const unsigned int nPoints, const unsigned int nTets, Vector3r *points, unsigned int *indices);
    void addRegularTetModel(const int width,
                            const int height,
                            const int depth,
                            const Vector3r &translation = Vector3r::Zero(),
                            const Matrix3r &rotation = Matrix3r::Identity(),
                            const Vector3r &scale = Vector3r::Ones());

    void addLineModel(const unsigned int nPoints,
                      const unsigned int nQuaternions,
                      Vector3r *points,
                      Quaternionr *quaternions,
                      unsigned int *indices,
                      unsigned int *indicesQuaternions);

    void updateConstraints();
    void initConstraintGroups();

    bool addBallJoint(const unsigned int rbIndex1, const unsigned int rbIndex2, const Vector3r &pos);
    bool addBallOnLineJoint(const unsigned int rbIndex1,
                            const unsigned int rbIndex2,
                            const Vector3r &pos,
                            const Vector3r &dir);
    bool addHingeJoint(const unsigned int rbIndex1,
                       const unsigned int rbIndex2,
                       const Vector3r &pos,
                       const Vector3r &axis);
    bool addTargetAngleMotorHingeJoint(const unsigned int rbIndex1,
                                       const unsigned int rbIndex2,
                                       const Vector3r &pos,
                                       const Vector3r &axis);
    bool addTargetVelocityMotorHingeJoint(const unsigned int rbIndex1,
                                          const unsigned int rbIndex2,
                                          const Vector3r &pos,
                                          const Vector3r &axis);
    bool addUniversalJoint(const unsigned int rbIndex1,
                           const unsigned int rbIndex2,
                           const Vector3r &pos,
                           const Vector3r &axis1,
                           const Vector3r &axis2);
    bool addSliderJoint(const unsigned int rbIndex1, const unsigned int rbIndex2, const Vector3r &axis);
    bool addTargetPositionMotorSliderJoint(const unsigned int rbIndex1,
                                           const unsigned int rbIndex2,
                                           const Vector3r &axis);
    bool addTargetVelocityMotorSliderJoint(const unsigned int rbIndex1,
                                           const unsigned int rbIndex2,
                                           const Vector3r &axis);
    bool addRigidBodyParticleBallJoint(const unsigned int rbIndex, const unsigned int particleIndex);
    bool addRigidBodySpring(const unsigned int rbIndex1,
                            const unsigned int rbIndex2,
                            const Vector3r &pos1,
                            const Vector3r &pos2,
                            const Real stiffness);
    bool addDistanceJoint(const unsigned int rbIndex1,
                          const unsigned int rbIndex2,
                          const Vector3r &pos1,
                          const Vector3r &pos2);
    bool addDamperJoint(const unsigned int rbIndex1,
                        const unsigned int rbIndex2,
                        const Vector3r &axis,
                        const Real stiffness);
    bool addRigidBodyContactConstraint(const unsigned int rbIndex1,
                                       const unsigned int rbIndex2,
                                       const Vector3r &cp1,
                                       const Vector3r &cp2,
                                       const Vector3r &normal,
                                       const Real dist,
                                       const Real restitutionCoeff,
                                       const Real frictionCoeff);
    bool addParticleRigidBodyContactConstraint(const unsigned int particleIndex,
                                               const unsigned int rbIndex,
                                               const Vector3r &cp1,
                                               const Vector3r &cp2,
                                               const Vector3r &normal,
                                               const Real dist,
                                               const Real restitutionCoeff,
                                               const Real frictionCoeff);

    bool addParticleSolidContactConstraint(const unsigned int particleIndex,
                                           const unsigned int solidIndex,
                                           const unsigned int tetIndex,
                                           const Vector3r &bary,
                                           const Vector3r &cp1,
                                           const Vector3r &cp2,
                                           const Vector3r &normal,
                                           const Real dist,
                                           const Real restitutionCoeff,
                                           const Real frictionCoeff);

    bool addDistanceConstraint(const unsigned int particle1, const unsigned int particle2, const Real stiffness);
    bool addDistanceConstraint_XPBD(const unsigned int particle1, const unsigned int particle2, const Real stiffness);
    bool addDihedralConstraint(const unsigned int particle1,
                               const unsigned int particle2,
                               const unsigned int particle3,
                               const unsigned int particle4,
                               const Real stiffness);
    bool addIsometricBendingConstraint(const unsigned int particle1,
                                       const unsigned int particle2,
                                       const unsigned int particle3,
                                       const unsigned int particle4,
                                       const Real stiffness);
    bool addIsometricBendingConstraint_XPBD(const unsigned int particle1,
                                            const unsigned int particle2,
                                            const unsigned int particle3,
                                            const unsigned int particle4,
                                            const Real stiffness);
    bool addFEMTriangleConstraint(const unsigned int particle1,
                                  const unsigned int particle2,
                                  const unsigned int particle3,
                                  const Real xxStiffness,
                                  const Real yyStiffness,
                                  const Real xyStiffness,
                                  const Real xyPoissonRatio,
                                  const Real yxPoissonRatio);
    bool addStrainTriangleConstraint(const unsigned int particle1,
                                     const unsigned int particle2,
                                     const unsigned int particle3,
                                     const Real xxStiffness,
                                     const Real yyStiffness,
                                     const Real xyStiffness,
                                     const bool normalizeStretch,
                                     const bool normalizeShear);
    bool addVolumeConstraint(const unsigned int particle1,
                             const unsigned int particle2,
                             const unsigned int particle3,
                             const unsigned int particle4,
                             const Real stiffness);
    bool addVolumeConstraint_XPBD(const unsigned int particle1,
                                  const unsigned int particle2,
                                  const unsigned int particle3,
                                  const unsigned int particle4,
                                  const Real stiffness);
    bool addFEMTetConstraint(const unsigned int particle1,
                             const unsigned int particle2,
                             const unsigned int particle3,
                             const unsigned int particle4,
                             const Real stiffness,
                             const Real poissonRatio);
    bool addStrainTetConstraint(const unsigned int particle1,
                                const unsigned int particle2,
                                const unsigned int particle3,
                                const unsigned int particle4,
                                const Real stretchStiffness,
                                const Real shearStiffness,
                                const bool normalizeStretch,
                                const bool normalizeShear);
    bool addShapeMatchingConstraint(const unsigned int numberOfParticles,
                                    const unsigned int particleIndices[],
                                    const unsigned int numClusters[],
                                    const Real stiffness);
    bool addStretchShearConstraint(const unsigned int particle1,
                                   const unsigned int particle2,
                                   const unsigned int quaternion1,
                                   const Real stretchingStiffness,
                                   const Real shearingStiffness1,
                                   const Real shearingStiffness2);
    bool addBendTwistConstraint(const unsigned int quaternion1,
                                const unsigned int quaternion2,
                                const Real twistingStiffness,
                                const Real bendingStiffness1,
                                const Real bendingStiffness2);
    bool addStretchBendingTwistingConstraint(const unsigned int rbIndex1,
                                             const unsigned int rbIndex2,
                                             const Vector3r &pos,
                                             const Real averageRadius,
                                             const Real averageSegmentLength,
                                             const Real youngsModulus,
                                             const Real torsionModulus);
    bool addDirectPositionBasedSolverForStiffRodsConstraint(
            const std::vector<std::pair<unsigned int, unsigned int>> &jointSegmentIndices,
            const std::vector<Vector3r> &jointPositions,
            const std::vector<Real> &averageRadii,
            const std::vector<Real> &averageSegmentLengths,
            const std::vector<Real> &youngsModuli,
            const std::vector<Real> &torsionModuli);

    Real getContactStiffnessRigidBody() const { return m_contactStiffnessRigidBody; }
    void setContactStiffnessRigidBody(Real val) { m_contactStiffnessRigidBody = val; }
    Real getContactStiffnessParticleRigidBody() const { return m_contactStiffnessParticleRigidBody; }
    void setContactStiffnessParticleRigidBody(Real val) { m_contactStiffnessParticleRigidBody = val; }

    void addClothConstraints(const TriangleModel *tm,
                             const unsigned int clothMethod,
                             const Real distanceStiffness,
                             const Real xxStiffness,
                             const Real yyStiffness,
                             const Real xyStiffness,
                             const Real xyPoissonRatio,
                             const Real yxPoissonRatio,
                             const bool normalizeStretch,
                             const bool normalizeShear);
    void addBendingConstraints(const TriangleModel *tm, const unsigned int bendingMethod, const Real stiffness);
    void addSolidConstraints(const TetModel *tm,
                             const unsigned int solidMethod,
                             const Real stiffness,
                             const Real poissonRatio,
                             const Real volumeStiffness,
                             const bool normalizeStretch,
                             const bool normalizeShear);

    template <typename ConstraintType, typename T, T ConstraintType::*MemPtr>
    void setConstraintValue(const T v) {
        for (auto i = 0; i < m_constraints.size(); i++) {
            ConstraintType *c = dynamic_cast<ConstraintType *>(m_constraints[i]);
            if (c != nullptr) c->*MemPtr = v;
        }
    }
};
}  // namespace vox::force