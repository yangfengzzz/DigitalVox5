//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/constraints.h"

#include <map>
#include <set>

#include "vox.force/id_factory.h"
#include "vox.force/position_based_dynamics.h"
#include "vox.force/position_based_elastic_rods.h"
#include "vox.force/position_based_rigid_body_dynamics.h"
#include "vox.force/simulation_model.h"
#include "vox.force/time_manager.h"
#include "vox.force/xpbd.h"

namespace vox::force {

int BallJoint::type_id = IDFactory::GetId();
int BallOnLineJoint::type_id = IDFactory::GetId();
int HingeJoint::type_id = IDFactory::GetId();
int UniversalJoint::type_id = IDFactory::GetId();
int RigidBodyParticleBallJoint::type_id = IDFactory::GetId();
int RigidBodySpring::type_id = IDFactory::GetId();
int DistanceJoint::type_id = IDFactory::GetId();
int DistanceConstraint::type_id = IDFactory::GetId();
int DistanceConstraint_XPBD::type_id = IDFactory::GetId();
int DihedralConstraint::type_id = IDFactory::GetId();
int IsometricBendingConstraint::type_id = IDFactory::GetId();
int IsometricBendingConstraint_XPBD::type_id = IDFactory::GetId();
int FEMTriangleConstraint::type_id = IDFactory::GetId();
int StrainTriangleConstraint::type_id = IDFactory::GetId();
int VolumeConstraint::type_id = IDFactory::GetId();
int VolumeConstraint_XPBD::type_id = IDFactory::GetId();
int FEMTetConstraint::type_id = IDFactory::GetId();
int StrainTetConstraint::type_id = IDFactory::GetId();
int ShapeMatchingConstraint::type_id = IDFactory::GetId();
int TargetAngleMotorHingeJoint::type_id = IDFactory::GetId();
int TargetVelocityMotorHingeJoint::type_id = IDFactory::GetId();
int SliderJoint::type_id = IDFactory::GetId();
int TargetPositionMotorSliderJoint::type_id = IDFactory::GetId();
int TargetVelocityMotorSliderJoint::type_id = IDFactory::GetId();
int DamperJoint::type_id = IDFactory::GetId();
int RigidBodyContactConstraint::type_id = IDFactory::GetId();
int ParticleRigidBodyContactConstraint::type_id = IDFactory::GetId();
int ParticleTetContactConstraint::type_id = IDFactory::GetId();
int StretchShearConstraint::type_id = IDFactory::GetId();
int BendTwistConstraint::type_id = IDFactory::GetId();
int StretchBendingTwistingConstraint::type_id = IDFactory::GetId();
int DirectPositionBasedSolverForStiffRodsConstraint::type_id = IDFactory::GetId();

//////////////////////////////////////////////////////////////////////////
// BallJoint
//////////////////////////////////////////////////////////////////////////
bool BallJoint::InitConstraint(SimulationModel &model,
                               unsigned int rb_index1,
                               unsigned int rb_index2,
                               const Vector3r &pos) {
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_BallJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                          rb2.getRotation(), pos, m_joint_info);
}

bool BallJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_BallJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                            rb2.getRotation(), m_joint_info);
}

bool BallJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_BallJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_joint_info, corr_x1, corr_q1,
            corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// BallOnLineJoint
//////////////////////////////////////////////////////////////////////////
bool BallOnLineJoint::InitConstraint(SimulationModel &model,
                                     const unsigned int rb_index1,
                                     const unsigned int rb_index2,
                                     const Vector3r &pos,
                                     const Vector3r &dir) {
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_BallOnLineJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                                rb2.getRotation(), pos, dir, m_joint_info);
}

bool BallOnLineJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_BallOnLineJoint(rb1.getPosition(), rb1.getRotation(),
                                                                  rb2.getPosition(), rb2.getRotation(), m_joint_info);
}

bool BallOnLineJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_BallOnLineJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_joint_info, corr_x1, corr_q1,
            corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// HingeJoint
//////////////////////////////////////////////////////////////////////////
bool HingeJoint::InitConstraint(SimulationModel &model,
                                const unsigned int rb_index1,
                                const unsigned int rb_index2,
                                const Vector3r &pos,
                                const Vector3r &axis) {
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_HingeJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                           rb2.getRotation(), pos, axis, m_joint_info);
}

bool HingeJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_HingeJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                             rb2.getRotation(), m_joint_info);
}

bool HingeJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_HingeJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_joint_info, corr_x1, corr_q1,
            corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// UniversalJoint
//////////////////////////////////////////////////////////////////////////
bool UniversalJoint::InitConstraint(SimulationModel &model,
                                    const unsigned int rb_index1,
                                    const unsigned int rb_index2,
                                    const Vector3r &pos,
                                    const Vector3r &axis1,
                                    const Vector3r &axis2) {
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_UniversalJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                               rb2.getRotation(), pos, axis1, axis2, m_joint_info);
}

bool UniversalJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_UniversalJoint(rb1.getPosition(), rb1.getRotation(),
                                                                 rb2.getPosition(), rb2.getRotation(), m_joint_info);
}

bool UniversalJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_UniversalJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_joint_info, corr_x1, corr_q1,
            corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// SliderJoint
//////////////////////////////////////////////////////////////////////////
bool SliderJoint::InitConstraint(SimulationModel &model,
                                 const unsigned int rb_index1,
                                 const unsigned int rb_index2,
                                 const Vector3r &axis) {
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_SliderJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                            rb2.getRotation(), axis, m_joint_info);
}

bool SliderJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_SliderJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                              rb2.getRotation(), m_joint_info);
}

bool SliderJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_SliderJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_joint_info, corr_x1, corr_q1,
            corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// TargetPositionMotorSliderJoint
//////////////////////////////////////////////////////////////////////////
bool TargetPositionMotorSliderJoint::InitConstraint(SimulationModel &model,
                                                    const unsigned int rb_index1,
                                                    const unsigned int rb_index2,
                                                    const Vector3r &axis) {
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_TargetPositionMotorSliderJoint(
            rb1.getPosition(), rb1.getRotation(), rb2.getPosition(), rb2.getRotation(), axis, m_joint_info);
}

bool TargetPositionMotorSliderJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_TargetPositionMotorSliderJoint(
            rb1.getPosition(), rb1.getRotation(), rb2.getPosition(), rb2.getRotation(), m_joint_info);
}

bool TargetPositionMotorSliderJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_TargetPositionMotorSliderJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_target, m_joint_info, corr_x1,
            corr_q1, corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// TargetVelocityMotorSliderJoint
//////////////////////////////////////////////////////////////////////////
bool TargetVelocityMotorSliderJoint::InitConstraint(SimulationModel &model,
                                                    const unsigned int rb_index1,
                                                    const unsigned int rb_index2,
                                                    const Vector3r &axis) {
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_TargetVelocityMotorSliderJoint(
            rb1.getPosition(), rb1.getRotation(), rb2.getPosition(), rb2.getRotation(), axis, m_joint_info);
}

bool TargetVelocityMotorSliderJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_TargetVelocityMotorSliderJoint(
            rb1.getPosition(), rb1.getRotation(), rb2.getPosition(), rb2.getRotation(), m_joint_info);
}

bool TargetVelocityMotorSliderJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_TargetVelocityMotorSliderJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_joint_info, corr_x1, corr_q1,
            corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

bool TargetVelocityMotorSliderJoint::SolveVelocityConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_v1, corr_v2;
    Vector3r corr_omega1, corr_omega2;
    const bool kRes = PositionBasedRigidBodyDynamics::velocitySolve_TargetVelocityMotorSliderJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getVelocity(), rb1.getInertiaTensorInverseW(), rb1.getRotation(),
            rb1.getAngularVelocity(), rb2.getInvMass(), rb2.getPosition(), rb2.getVelocity(),
            rb2.getInertiaTensorInverseW(), rb2.getRotation(), rb2.getAngularVelocity(), m_target, m_joint_info,
            corr_v1, corr_omega1, corr_v2, corr_omega2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getVelocity() += corr_v1;
            rb1.getAngularVelocity() += corr_omega1;
        }
        if (rb2.getMass() != 0.0) {
            rb2.getVelocity() += corr_v2;
            rb2.getAngularVelocity() += corr_omega2;
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// TargetAngleMotorHingeJoint
//////////////////////////////////////////////////////////////////////////
bool TargetAngleMotorHingeJoint::InitConstraint(SimulationModel &model,
                                                const unsigned int rb_index1,
                                                const unsigned int rb_index2,
                                                const Vector3r &pos,
                                                const Vector3r &axis) {
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_TargetAngleMotorHingeJoint(
            rb1.getPosition(), rb1.getRotation(), rb2.getPosition(), rb2.getRotation(), pos, axis, m_joint_info);
}

bool TargetAngleMotorHingeJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_TargetAngleMotorHingeJoint(
            rb1.getPosition(), rb1.getRotation(), rb2.getPosition(), rb2.getRotation(), m_joint_info);
}

bool TargetAngleMotorHingeJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_TargetAngleMotorHingeJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_target, m_joint_info, corr_x1,
            corr_q1, corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// TargetVelocityMotorHingeJoint
//////////////////////////////////////////////////////////////////////////
bool TargetVelocityMotorHingeJoint::InitConstraint(SimulationModel &model,
                                                   const unsigned int rb_index1,
                                                   const unsigned int rb_index2,
                                                   const Vector3r &pos,
                                                   const Vector3r &axis) {
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_TargetVelocityMotorHingeJoint(
            rb1.getPosition(), rb1.getRotation(), rb2.getPosition(), rb2.getRotation(), pos, axis, m_joint_info);
}

bool TargetVelocityMotorHingeJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_TargetVelocityMotorHingeJoint(
            rb1.getPosition(), rb1.getRotation(), rb2.getPosition(), rb2.getRotation(), m_joint_info);
}

bool TargetVelocityMotorHingeJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_TargetVelocityMotorHingeJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_joint_info, corr_x1, corr_q1,
            corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

bool TargetVelocityMotorHingeJoint::SolveVelocityConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_v1, corr_v2;
    Vector3r corr_omega1, corr_omega2;
    const bool kRes = PositionBasedRigidBodyDynamics::velocitySolve_TargetVelocityMotorHingeJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getVelocity(), rb1.getInertiaTensorInverseW(),
            rb1.getAngularVelocity(), rb2.getInvMass(), rb2.getPosition(), rb2.getVelocity(),
            rb2.getInertiaTensorInverseW(), rb2.getAngularVelocity(), m_target, m_joint_info, corr_v1, corr_omega1,
            corr_v2, corr_omega2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getVelocity() += corr_v1;
            rb1.getAngularVelocity() += corr_omega1;
        }
        if (rb2.getMass() != 0.0) {
            rb2.getVelocity() += corr_v2;
            rb2.getAngularVelocity() += corr_omega2;
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// DamperJoint
//////////////////////////////////////////////////////////////////////////
bool DamperJoint::InitConstraint(SimulationModel &model,
                                 const unsigned int rb_index1,
                                 const unsigned int rb_index2,
                                 const Vector3r &axis,
                                 const Real stiffness) {
    m_stiffness = stiffness;
    m_lambda = 0.0;
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_DamperJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                            rb2.getRotation(), axis, m_joint_info);
}

bool DamperJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_DamperJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                              rb2.getRotation(), m_joint_info);
}

bool DamperJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    const Real kDt = TimeManager::getCurrent()->getTimeStepSize();

    if (iter == 0) m_lambda = 0.0;

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_DamperJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_stiffness, kDt, m_joint_info,
            m_lambda, corr_x1, corr_q1, corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// RigidBodyParticleBallJoint
//////////////////////////////////////////////////////////////////////////
bool RigidBodyParticleBallJoint::InitConstraint(SimulationModel &model,
                                                const unsigned int rb_index,
                                                const unsigned int particle_index) {
    m_bodies[0] = rb_index;
    m_bodies[1] = particle_index;
    SimulationModel::RigidBodyVector &rbs = model.getRigidBodies();
    ParticleData &pd = model.getParticles();
    RigidBody &rb = *rbs[m_bodies[0]];
    return PositionBasedRigidBodyDynamics::init_RigidBodyParticleBallJoint(
            rb.getPosition(), rb.getRotation(), pd.GetPosition(particle_index), m_joint_info);
}

bool RigidBodyParticleBallJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    ParticleData &pd = model.getParticles();
    RigidBody &rb1 = *rb[m_bodies[0]];
    return PositionBasedRigidBodyDynamics::update_RigidBodyParticleBallJoint(rb1.getPosition(), rb1.getRotation(),
                                                                             pd.GetPosition(m_bodies[1]), m_joint_info);
}

bool RigidBodyParticleBallJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    ParticleData &pd = model.getParticles();

    RigidBody &rb1 = *rb[m_bodies[0]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_RigidBodyParticleBallJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(),
            pd.GetInvMass(m_bodies[1]), pd.GetPosition(m_bodies[1]), m_joint_info, corr_x1, corr_q1, corr_x2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (pd.GetMass(m_bodies[1]) != 0.0) {
            pd.GetPosition(m_bodies[1]) += corr_x2;
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// RigidBodySpring
//////////////////////////////////////////////////////////////////////////
bool RigidBodySpring::InitConstraint(SimulationModel &model,
                                     const unsigned int rb_index1,
                                     const unsigned int rb_index2,
                                     const Vector3r &pos1,
                                     const Vector3r &pos2,
                                     const Real stiffness) {
    m_stiffness = stiffness;
    m_lambda = 0.0;
    m_rest_length = (pos1 - pos2).norm();
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_DistanceJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                              rb2.getRotation(), pos1, pos2, m_joint_info);
}

bool RigidBodySpring::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_DistanceJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                                rb2.getRotation(), m_joint_info);
}

bool RigidBodySpring::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    const Real kDt = TimeManager::getCurrent()->getTimeStepSize();

    if (iter == 0) m_lambda = 0.0;

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_DistanceJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), m_stiffness, m_rest_length, kDt,
            m_joint_info, m_lambda, corr_x1, corr_q1, corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// DistanceJoint
//////////////////////////////////////////////////////////////////////////
bool DistanceJoint::InitConstraint(SimulationModel &model,
                                   const unsigned int rb_index1,
                                   const unsigned int rb_index2,
                                   const Vector3r &pos1,
                                   const Vector3r &pos2) {
    m_rest_length = (pos1 - pos2).norm();
    m_bodies[0] = rb_index1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::init_DistanceJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                              rb2.getRotation(), pos1, pos2, m_joint_info);
}

bool DistanceJoint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];
    return PositionBasedRigidBodyDynamics::update_DistanceJoint(rb1.getPosition(), rb1.getRotation(), rb2.getPosition(),
                                                                rb2.getRotation(), m_joint_info);
}

bool DistanceJoint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Real lambda = 0.0;

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = PositionBasedRigidBodyDynamics::solve_DistanceJoint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getInertiaTensorInverseW(), rb1.getRotation(), rb2.getInvMass(),
            rb2.getPosition(), rb2.getInertiaTensorInverseW(), rb2.getRotation(), 0.0, m_rest_length, 0.0, m_joint_info,
            lambda, corr_x1, corr_q1, corr_x2, corr_q2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += corr_x1;
            rb1.getRotation().coeffs() += corr_q1.coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
        if (rb2.getMass() != 0.0) {
            rb2.getPosition() += corr_x2;
            rb2.getRotation().coeffs() += corr_q2.coeffs();
            rb2.getRotation().normalize();
            rb2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// DistanceConstraint
//////////////////////////////////////////////////////////////////////////
bool DistanceConstraint::InitConstraint(SimulationModel &model,
                                        const unsigned int particle1,
                                        const unsigned int particle2,
                                        const Real stiffness) {
    m_stiffness = stiffness;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    ParticleData &pd = model.getParticles();

    const Vector3r &x1_0 = pd.GetPosition0(particle1);
    const Vector3r &x2_0 = pd.GetPosition0(particle2);

    m_rest_length = (x2_0 - x1_0).norm();

    return true;
}

bool DistanceConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);

    Vector3r corr1, corr2;
    const bool kRes = PositionBasedDynamics::SolveDistanceConstraint(x1, kInvMass1, x2, kInvMass2, m_rest_length,
                                                                     m_stiffness, corr1, corr2);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// DistanceConstraint_XPBD
//////////////////////////////////////////////////////////////////////////
bool DistanceConstraint_XPBD::InitConstraint(SimulationModel &model,
                                             const unsigned int particle1,
                                             const unsigned int particle2,
                                             const Real stiffness) {
    m_stiffness = stiffness;
    m_lambda = 0.0;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    ParticleData &pd = model.getParticles();

    const Vector3r &x1_0 = pd.GetPosition0(particle1);
    const Vector3r &x2_0 = pd.GetPosition0(particle2);

    m_rest_length = (x2_0 - x1_0).norm();

    return true;
}

bool DistanceConstraint_XPBD::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);

    const Real kDt = TimeManager::getCurrent()->getTimeStepSize();

    if (iter == 0) m_lambda = 0.0;

    Vector3r corr1, corr2;
    const bool kRes = XPBD::solve_DistanceConstraint(x1, kInvMass1, x2, kInvMass2, m_rest_length, m_stiffness, kDt,
                                                     m_lambda, corr1, corr2);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// DihedralConstraint
//////////////////////////////////////////////////////////////////////////

bool DihedralConstraint::InitConstraint(SimulationModel &model,
                                        const unsigned int particle1,
                                        const unsigned int particle2,
                                        const unsigned int particle3,
                                        const unsigned int particle4,
                                        const Real stiffness) {
    m_stiffness = stiffness;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = particle3;
    m_bodies[3] = particle4;
    ParticleData &pd = model.getParticles();

    const Vector3r &p0 = pd.GetPosition0(particle1);
    const Vector3r &p1 = pd.GetPosition0(particle2);
    const Vector3r &p2 = pd.GetPosition0(particle3);
    const Vector3r &p3 = pd.GetPosition0(particle4);

    Vector3r e = p3 - p2;
    Real elen = e.norm();
    if (elen < 1e-6) return false;

    Real inv_elen = static_cast<Real>(1.0) / elen;

    Vector3r n1 = (p2 - p0).cross(p3 - p0);
    n1 /= n1.squaredNorm();
    Vector3r n2 = (p3 - p1).cross(p2 - p1);
    n2 /= n2.squaredNorm();

    n1.normalize();
    n2.normalize();
    Real dot = n1.dot(n2);

    if (dot < -1.0) dot = -1.0;
    if (dot > 1.0) dot = 1.0;

    m_rest_angle = acos(dot);

    return true;
}

bool DihedralConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kI3 = m_bodies[2];
    const unsigned kI4 = m_bodies[3];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Vector3r &x3 = pd.GetPosition(kI3);
    Vector3r &x4 = pd.GetPosition(kI4);

    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMass3 = pd.GetInvMass(kI3);
    const Real kInvMass4 = pd.GetInvMass(kI4);

    Vector3r corr1, corr2, corr3, corr4;
    const bool kRes =
            PositionBasedDynamics::SolveDihedralConstraint(x1, kInvMass1, x2, kInvMass2, x3, kInvMass3, x4, kInvMass4,
                                                           m_rest_angle, m_stiffness, corr1, corr2, corr3, corr4);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMass3 != 0.0) x3 += corr3;
        if (kInvMass4 != 0.0) x4 += corr4;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// IsometricBendingConstraint
//////////////////////////////////////////////////////////////////////////
bool IsometricBendingConstraint::InitConstraint(SimulationModel &model,
                                                const unsigned int particle1,
                                                const unsigned int particle2,
                                                const unsigned int particle3,
                                                const unsigned int particle4,
                                                const Real stiffness) {
    m_stiffness = stiffness;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = particle3;
    m_bodies[3] = particle4;

    ParticleData &pd = model.getParticles();

    const Vector3r &x1 = pd.GetPosition0(particle1);
    const Vector3r &x2 = pd.GetPosition0(particle2);
    const Vector3r &x3 = pd.GetPosition0(particle3);
    const Vector3r &x4 = pd.GetPosition0(particle4);

    return PositionBasedDynamics::InitIsometricBendingConstraint(x1, x2, x3, x4, m_Q);
}

bool IsometricBendingConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kI3 = m_bodies[2];
    const unsigned kI4 = m_bodies[3];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Vector3r &x3 = pd.GetPosition(kI3);
    Vector3r &x4 = pd.GetPosition(kI4);

    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMass3 = pd.GetInvMass(kI3);
    const Real kInvMass4 = pd.GetInvMass(kI4);

    Vector3r corr1, corr2, corr3, corr4;
    const bool kRes = PositionBasedDynamics::SolveIsometricBendingConstraint(
            x1, kInvMass1, x2, kInvMass2, x3, kInvMass3, x4, kInvMass4, m_Q, m_stiffness, corr1, corr2, corr3, corr4);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMass3 != 0.0) x3 += corr3;
        if (kInvMass4 != 0.0) x4 += corr4;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// IsometricBendingConstraint_XPBD
//////////////////////////////////////////////////////////////////////////
bool IsometricBendingConstraint_XPBD::InitConstraint(SimulationModel &model,
                                                     const unsigned int particle1,
                                                     const unsigned int particle2,
                                                     const unsigned int particle3,
                                                     const unsigned int particle4,
                                                     const Real stiffness) {
    m_lambda = 0.0;
    m_stiffness = stiffness;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = particle3;
    m_bodies[3] = particle4;

    ParticleData &pd = model.getParticles();

    const Vector3r &x1 = pd.GetPosition0(particle1);
    const Vector3r &x2 = pd.GetPosition0(particle2);
    const Vector3r &x3 = pd.GetPosition0(particle3);
    const Vector3r &x4 = pd.GetPosition0(particle4);

    return PositionBasedDynamics::InitIsometricBendingConstraint(x1, x2, x3, x4, m_Q);
}

bool IsometricBendingConstraint_XPBD::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kI3 = m_bodies[2];
    const unsigned kI4 = m_bodies[3];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Vector3r &x3 = pd.GetPosition(kI3);
    Vector3r &x4 = pd.GetPosition(kI4);

    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMass3 = pd.GetInvMass(kI3);
    const Real kInvMass4 = pd.GetInvMass(kI4);

    const Real kDt = TimeManager::getCurrent()->getTimeStepSize();

    if (iter == 0) m_lambda = 0.0;

    Vector3r corr1, corr2, corr3, corr4;
    const bool kRes =
            XPBD::solve_IsometricBendingConstraint(x1, kInvMass1, x2, kInvMass2, x3, kInvMass3, x4, kInvMass4, m_Q,
                                                   m_stiffness, kDt, m_lambda, corr1, corr2, corr3, corr4);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMass3 != 0.0) x3 += corr3;
        if (kInvMass4 != 0.0) x4 += corr4;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// FEMTriangleConstraint
//////////////////////////////////////////////////////////////////////////
bool FEMTriangleConstraint::InitConstraint(SimulationModel &model,
                                           const unsigned int particle1,
                                           const unsigned int particle2,
                                           const unsigned int particle3,
                                           const Real xx_stiffness,
                                           const Real yy_stiffness,
                                           const Real xy_stiffness,
                                           const Real xy_poisson_ratio,
                                           const Real yx_poisson_ratio) {
    m_xx_stiffness = xx_stiffness;
    m_yy_stiffness = yy_stiffness;
    m_xy_stiffness = xy_stiffness;
    m_xy_poisson_ratio = xy_poisson_ratio;
    m_yx_poisson_ratio = yx_poisson_ratio;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = particle3;

    ParticleData &pd = model.getParticles();

    Vector3r &x1 = pd.GetPosition0(particle1);
    Vector3r &x2 = pd.GetPosition0(particle2);
    Vector3r &x3 = pd.GetPosition0(particle3);

    return PositionBasedDynamics::InitFemTriangleConstraint(x1, x2, x3, m_area, m_inv_rest_mat);
}

bool FEMTriangleConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kI3 = m_bodies[2];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Vector3r &x3 = pd.GetPosition(kI3);

    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMass3 = pd.GetInvMass(kI3);

    Vector3r corr1, corr2, corr3;
    const bool kRes = PositionBasedDynamics::SolveFemTriangleConstraint(
            x1, kInvMass1, x2, kInvMass2, x3, kInvMass3, m_area, m_inv_rest_mat, m_xx_stiffness, m_yy_stiffness,
            m_xy_stiffness, m_xy_poisson_ratio, m_yx_poisson_ratio, corr1, corr2, corr3);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMass3 != 0.0) x3 += corr3;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// StrainTriangleConstraint
//////////////////////////////////////////////////////////////////////////
bool StrainTriangleConstraint::InitConstraint(SimulationModel &model,
                                              const unsigned int particle1,
                                              const unsigned int particle2,
                                              const unsigned int particle3,
                                              const Real xx_stiffness,
                                              const Real yy_stiffness,
                                              const Real xy_stiffness,
                                              const bool normalize_stretch,
                                              const bool normalize_shear) {
    m_xx_stiffness = xx_stiffness;
    m_yy_stiffness = yy_stiffness;
    m_xy_stiffness = xy_stiffness;
    m_normalize_stretch = normalize_stretch;
    m_normalize_shear = normalize_shear;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = particle3;

    ParticleData &pd = model.getParticles();

    Vector3r &x1 = pd.GetPosition0(particle1);
    Vector3r &x2 = pd.GetPosition0(particle2);
    Vector3r &x3 = pd.GetPosition0(particle3);

    // Bring triangles to xy plane
    const Vector3r kY1(x1[0], x1[2], 0.0);
    const Vector3r kY2(x2[0], x2[2], 0.0);
    const Vector3r kY3(x3[0], x3[2], 0.0);

    return PositionBasedDynamics::InitStrainTriangleConstraint(kY1, kY2, kY3, m_inv_rest_mat);
}

bool StrainTriangleConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kI3 = m_bodies[2];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Vector3r &x3 = pd.GetPosition(kI3);

    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMass3 = pd.GetInvMass(kI3);

    Vector3r corr1, corr2, corr3;
    const bool kRes = PositionBasedDynamics::SolveStrainTriangleConstraint(
            x1, kInvMass1, x2, kInvMass2, x3, kInvMass3, m_inv_rest_mat, m_xx_stiffness, m_yy_stiffness, m_xy_stiffness,
            m_normalize_stretch, m_normalize_shear, corr1, corr2, corr3);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMass3 != 0.0) x3 += corr3;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// VolumeConstraint
//////////////////////////////////////////////////////////////////////////

bool VolumeConstraint::InitConstraint(SimulationModel &model,
                                      const unsigned int particle1,
                                      const unsigned int particle2,
                                      const unsigned int particle3,
                                      const unsigned int particle4,
                                      const Real stiffness) {
    m_stiffness = stiffness;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = particle3;
    m_bodies[3] = particle4;
    ParticleData &pd = model.getParticles();

    const Vector3r &p0 = pd.GetPosition0(particle1);
    const Vector3r &p1 = pd.GetPosition0(particle2);
    const Vector3r &p2 = pd.GetPosition0(particle3);
    const Vector3r &p3 = pd.GetPosition0(particle4);

    m_rest_volume = fabs(static_cast<Real>(1.0 / 6.0) * (p3 - p0).dot((p2 - p0).cross(p1 - p0)));

    return true;
}

bool VolumeConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kI3 = m_bodies[2];
    const unsigned kI4 = m_bodies[3];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Vector3r &x3 = pd.GetPosition(kI3);
    Vector3r &x4 = pd.GetPosition(kI4);

    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMass3 = pd.GetInvMass(kI3);
    const Real kInvMass4 = pd.GetInvMass(kI4);

    Vector3r corr1, corr2, corr3, corr4;
    const bool kRes =
            PositionBasedDynamics::SolveVolumeConstraint(x1, kInvMass1, x2, kInvMass2, x3, kInvMass3, x4, kInvMass4,
                                                         m_rest_volume, m_stiffness, corr1, corr2, corr3, corr4);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMass3 != 0.0) x3 += corr3;
        if (kInvMass4 != 0.0) x4 += corr4;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// VolumeConstraint_XPBD
//////////////////////////////////////////////////////////////////////////

bool VolumeConstraint_XPBD::InitConstraint(SimulationModel &model,
                                           const unsigned int particle1,
                                           const unsigned int particle2,
                                           const unsigned int particle3,
                                           const unsigned int particle4,
                                           const Real stiffness) {
    m_stiffness = stiffness;
    m_lambda = 0.0;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = particle3;
    m_bodies[3] = particle4;
    ParticleData &pd = model.getParticles();

    const Vector3r &p0 = pd.GetPosition0(particle1);
    const Vector3r &p1 = pd.GetPosition0(particle2);
    const Vector3r &p2 = pd.GetPosition0(particle3);
    const Vector3r &p3 = pd.GetPosition0(particle4);

    m_rest_volume = fabs(static_cast<Real>(1.0 / 6.0) * (p3 - p0).dot((p2 - p0).cross(p1 - p0)));

    return true;
}

bool VolumeConstraint_XPBD::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kI3 = m_bodies[2];
    const unsigned kI4 = m_bodies[3];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Vector3r &x3 = pd.GetPosition(kI3);
    Vector3r &x4 = pd.GetPosition(kI4);

    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMass3 = pd.GetInvMass(kI3);
    const Real kInvMass4 = pd.GetInvMass(kI4);

    const Real kDt = TimeManager::getCurrent()->getTimeStepSize();

    if (iter == 0) m_lambda = 0.0;

    Vector3r corr1, corr2, corr3, corr4;
    const bool kRes =
            XPBD::solve_VolumeConstraint(x1, kInvMass1, x2, kInvMass2, x3, kInvMass3, x4, kInvMass4, m_rest_volume,
                                         m_stiffness, kDt, m_lambda, corr1, corr2, corr3, corr4);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMass3 != 0.0) x3 += corr3;
        if (kInvMass4 != 0.0) x4 += corr4;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// FEMTetConstraint
//////////////////////////////////////////////////////////////////////////
bool FEMTetConstraint::InitConstraint(SimulationModel &model,
                                      const unsigned int particle1,
                                      const unsigned int particle2,
                                      const unsigned int particle3,
                                      const unsigned int particle4,
                                      const Real stiffness,
                                      const Real poisson_ratio) {
    m_stiffness = stiffness;
    m_poisson_ratio = poisson_ratio;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = particle3;
    m_bodies[3] = particle4;

    ParticleData &pd = model.getParticles();

    Vector3r &x1 = pd.GetPosition0(particle1);
    Vector3r &x2 = pd.GetPosition0(particle2);
    Vector3r &x3 = pd.GetPosition0(particle3);
    Vector3r &x4 = pd.GetPosition0(particle4);

    return PositionBasedDynamics::InitFemTetraConstraint(x1, x2, x3, x4, m_volume, m_inv_rest_mat);
}

bool FEMTetConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kI3 = m_bodies[2];
    const unsigned kI4 = m_bodies[3];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Vector3r &x3 = pd.GetPosition(kI3);
    Vector3r &x4 = pd.GetPosition(kI4);

    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMass3 = pd.GetInvMass(kI3);
    const Real kInvMass4 = pd.GetInvMass(kI4);

    Real current_volume = -static_cast<Real>(1.0 / 6.0) * (x4 - x1).dot((x3 - x1).cross(x2 - x1));
    bool handle_inversion = false;
    if (current_volume / m_volume < 0.2)  // Only 20% of initial volume left
        handle_inversion = true;

    Vector3r corr1, corr2, corr3, corr4;
    const bool kRes = PositionBasedDynamics::SolveFemTetraConstraint(
            x1, kInvMass1, x2, kInvMass2, x3, kInvMass3, x4, kInvMass4, m_volume, m_inv_rest_mat, m_stiffness,
            m_poisson_ratio, handle_inversion, corr1, corr2, corr3, corr4);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMass3 != 0.0) x3 += corr3;
        if (kInvMass4 != 0.0) x4 += corr4;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// StrainTetConstraint
//////////////////////////////////////////////////////////////////////////
bool StrainTetConstraint::InitConstraint(SimulationModel &model,
                                         const unsigned int particle1,
                                         const unsigned int particle2,
                                         const unsigned int particle3,
                                         const unsigned int particle4,
                                         const Real stretch_stiffness,
                                         const Real shear_stiffness,
                                         const bool normalize_stretch,
                                         const bool normalize_shear) {
    m_stretch_stiffness = stretch_stiffness;
    m_shear_stiffness = shear_stiffness;
    m_normalize_stretch = normalize_stretch;
    m_normalize_shear = normalize_shear;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = particle3;
    m_bodies[3] = particle4;

    ParticleData &pd = model.getParticles();

    Vector3r &x1 = pd.GetPosition0(particle1);
    Vector3r &x2 = pd.GetPosition0(particle2);
    Vector3r &x3 = pd.GetPosition0(particle3);
    Vector3r &x4 = pd.GetPosition0(particle4);

    return PositionBasedDynamics::InitStrainTetraConstraint(x1, x2, x3, x4, m_inv_rest_mat);
}

bool StrainTetConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kI3 = m_bodies[2];
    const unsigned kI4 = m_bodies[3];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Vector3r &x3 = pd.GetPosition(kI3);
    Vector3r &x4 = pd.GetPosition(kI4);

    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMass3 = pd.GetInvMass(kI3);
    const Real kInvMass4 = pd.GetInvMass(kI4);

    Vector3r corr1, corr2, corr3, corr4;
    const bool kRes = PositionBasedDynamics::SolveStrainTetraConstraint(
            x1, kInvMass1, x2, kInvMass2, x3, kInvMass3, x4, kInvMass4, m_inv_rest_mat,
            m_stretch_stiffness * Vector3r::Ones(), m_shear_stiffness * Vector3r::Ones(), m_normalize_stretch,
            m_normalize_shear, corr1, corr2, corr3, corr4);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMass3 != 0.0) x3 += corr3;
        if (kInvMass4 != 0.0) x4 += corr4;
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// ShapeMatchingConstraint
//////////////////////////////////////////////////////////////////////////
bool ShapeMatchingConstraint::InitConstraint(SimulationModel &model,
                                             const unsigned int particle_indices[],
                                             const unsigned int num_clusters[],
                                             const Real stiffness) {
    m_stiffness = stiffness;
    ParticleData &pd = model.getParticles();
    for (unsigned int i = 0; i < NumberOfBodies(); i++) {
        m_bodies[i] = particle_indices[i];
        m_x0[i] = pd.GetPosition0(m_bodies[i]);
        m_w[i] = pd.GetInvMass(m_bodies[i]);
        m_num_clusters[i] = num_clusters[i];
    }

    const bool kRes = PositionBasedDynamics::InitShapeMatchingConstraint(m_x0, m_w, NumberOfBodies(), m_rest_cm);
    return kRes;
}

bool ShapeMatchingConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();
    for (unsigned int i = 0; i < NumberOfBodies(); i++) {
        m_x[i] = pd.GetPosition(m_bodies[i]);
    }

    const bool kRes = PositionBasedDynamics::SolveShapeMatchingConstraint(m_x0, m_x, m_w, NumberOfBodies(), m_rest_cm,
                                                                          m_stiffness, false, m_corr);

    if (kRes) {
        for (unsigned int i = 0; i < NumberOfBodies(); i++) {
            // Important: Divide position correction by the number of clusters
            // which contain the vertex.
            if (m_w[i] != 0.0) pd.GetPosition(m_bodies[i]) += (1.0 / m_num_clusters[i]) * m_corr[i];
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// RigidBodyContactConstraint
//////////////////////////////////////////////////////////////////////////
bool RigidBodyContactConstraint::InitConstraint(SimulationModel &model,
                                                const unsigned int rb_index_1,
                                                const unsigned int rb_index2,
                                                const Vector3r &cp_1,
                                                const Vector3r &cp2,
                                                const Vector3r &normal,
                                                const Real dist,
                                                const Real restitution_coeff,
                                                const Real stiffness,
                                                const Real friction_coeff) {
    m_stiffness = stiffness;
    m_friction_coeff = friction_coeff;

    m_bodies[0] = rb_index_1;
    m_bodies[1] = rb_index2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    m_sum_impulses = 0.0;

    return PositionBasedRigidBodyDynamics::init_RigidBodyContactConstraint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getVelocity(), rb1.getInertiaTensorInverseW(), rb1.getRotation(),
            rb1.getAngularVelocity(), rb2.getInvMass(), rb2.getPosition(), rb2.getVelocity(),
            rb2.getInertiaTensorInverseW(), rb2.getRotation(), rb2.getAngularVelocity(), cp_1, cp2, normal,
            restitution_coeff, m_constraint_info);
}

bool RigidBodyContactConstraint::SolveVelocityConstraint(SimulationModel &model, unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &rb1 = *rb[m_bodies[0]];
    RigidBody &rb2 = *rb[m_bodies[1]];

    Vector3r corr_v1, corr_v2;
    Vector3r corr_omega1, corr_omega2;
    const bool kRes = PositionBasedRigidBodyDynamics::velocitySolve_RigidBodyContactConstraint(
            rb1.getInvMass(), rb1.getPosition(), rb1.getVelocity(), rb1.getInertiaTensorInverseW(),
            rb1.getAngularVelocity(), rb2.getInvMass(), rb2.getPosition(), rb2.getVelocity(),
            rb2.getInertiaTensorInverseW(), rb2.getAngularVelocity(), m_stiffness, m_friction_coeff, m_sum_impulses,
            m_constraint_info, corr_v1, corr_omega1, corr_v2, corr_omega2);

    if (kRes) {
        if (rb1.getMass() != 0.0) {
            rb1.getVelocity() += corr_v1;
            rb1.getAngularVelocity() += corr_omega1;
        }
        if (rb2.getMass() != 0.0) {
            rb2.getVelocity() += corr_v2;
            rb2.getAngularVelocity() += corr_omega2;
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// ParticleRigidBodyContactConstraint
//////////////////////////////////////////////////////////////////////////
bool ParticleRigidBodyContactConstraint::InitConstraint(SimulationModel &model,
                                                        const unsigned int particle_index,
                                                        const unsigned int rb_index,
                                                        const Vector3r &cp1,
                                                        const Vector3r &cp2,
                                                        const Vector3r &normal,
                                                        const Real dist,
                                                        const Real restitution_coeff,
                                                        const Real stiffness,
                                                        const Real friction_coeff) {
    m_stiffness = stiffness;
    m_friction_coeff = friction_coeff;

    m_bodies[0] = particle_index;
    m_bodies[1] = rb_index;
    SimulationModel::RigidBodyVector &rbs = model.getRigidBodies();
    ParticleData &pd = model.getParticles();

    RigidBody &rb = *rbs[m_bodies[1]];

    m_sum_impulses = 0.0;

    return PositionBasedRigidBodyDynamics::init_ParticleRigidBodyContactConstraint(
            pd.GetInvMass(particle_index), pd.GetPosition(particle_index), pd.GetVelocity(particle_index),
            rb.getInvMass(), rb.getPosition(), rb.getVelocity(), rb.getInertiaTensorInverseW(), rb.getRotation(),
            rb.getAngularVelocity(), cp1, cp2, normal, restitution_coeff, m_constraint_info);
}

bool ParticleRigidBodyContactConstraint::SolveVelocityConstraint(SimulationModel &model, unsigned int iter) {
    SimulationModel::RigidBodyVector &rbs = model.getRigidBodies();
    ParticleData &pd = model.getParticles();

    RigidBody &rb = *rbs[m_bodies[1]];

    Vector3r corr_v1, corr_v2;
    Vector3r corr_omega_2;
    const bool kRes = PositionBasedRigidBodyDynamics::velocitySolve_ParticleRigidBodyContactConstraint(
            pd.GetInvMass(m_bodies[0]), pd.GetPosition(m_bodies[0]), pd.GetVelocity(m_bodies[0]), rb.getInvMass(),
            rb.getPosition(), rb.getVelocity(), rb.getInertiaTensorInverseW(), rb.getAngularVelocity(), m_stiffness,
            m_friction_coeff, m_sum_impulses, m_constraint_info, corr_v1, corr_v2, corr_omega_2);

    if (kRes) {
        if (pd.GetMass(m_bodies[0]) != 0.0) {
            pd.GetVelocity(m_bodies[0]) += corr_v1;
        }
        if (rb.getMass() != 0.0) {
            rb.getVelocity() += corr_v2;
            rb.getAngularVelocity() += corr_omega_2;
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// ParticleSolidContactConstraint
//////////////////////////////////////////////////////////////////////////
bool ParticleTetContactConstraint::InitConstraint(SimulationModel &model,
                                                  const unsigned int particle_index,
                                                  const unsigned int solid_index,
                                                  const unsigned int tet_index,
                                                  const Vector3r &bary,
                                                  const Vector3r &cp1,
                                                  const Vector3r &cp2,
                                                  const Vector3r &normal,
                                                  const Real dist,
                                                  const Real friction_coeff) {
    m_friction_coeff = friction_coeff;

    m_bodies[0] = particle_index;
    m_bodies[1] = solid_index;
    m_tet_index = tet_index;
    m_solid_index = solid_index;
    m_bary = bary;
    ParticleData &pd = model.getParticles();

    const SimulationModel::TetModelVector &tet_models = model.getTetModels();
    TetModel *tm = tet_models[solid_index];
    const unsigned int kOffset = tm->getIndexOffset();
    const unsigned int *indices = tm->getParticleMesh().GetTets().data();
    m_x[0] = pd.GetPosition(indices[4 * tet_index] + kOffset);
    m_x[1] = pd.GetPosition(indices[4 * tet_index + 1] + kOffset);
    m_x[2] = pd.GetPosition(indices[4 * tet_index + 2] + kOffset);
    m_x[3] = pd.GetPosition(indices[4 * tet_index + 3] + kOffset);
    m_v[0] = pd.GetVelocity(indices[4 * tet_index] + kOffset);
    m_v[1] = pd.GetVelocity(indices[4 * tet_index + 1] + kOffset);
    m_v[2] = pd.GetVelocity(indices[4 * tet_index + 2] + kOffset);
    m_v[3] = pd.GetVelocity(indices[4 * tet_index + 3] + kOffset);
    m_inv_masses[0] = pd.GetInvMass(indices[4 * tet_index] + kOffset);
    m_inv_masses[1] = pd.GetInvMass(indices[4 * tet_index + 1] + kOffset);
    m_inv_masses[2] = pd.GetInvMass(indices[4 * tet_index + 2] + kOffset);
    m_inv_masses[3] = pd.GetInvMass(indices[4 * tet_index + 3] + kOffset);

    return PositionBasedDynamics::InitParticleTetContactConstraint(
            pd.GetInvMass(particle_index), pd.GetPosition(particle_index), pd.GetVelocity(particle_index), m_inv_masses,
            m_x.data(), m_v.data(), bary, normal, m_constraint_info);
}

bool ParticleTetContactConstraint::SolvePositionConstraint(SimulationModel &model, unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const SimulationModel::TetModelVector &tet_models = model.getTetModels();
    TetModel *tm = tet_models[m_solid_index];
    const unsigned int kOffset = tm->getIndexOffset();
    const unsigned int *indices = tm->getParticleMesh().GetTets().data();
    Vector3r &x0 = pd.GetPosition(indices[4 * m_tet_index] + kOffset);
    Vector3r &x1 = pd.GetPosition(indices[4 * m_tet_index + 1] + kOffset);
    Vector3r &x2 = pd.GetPosition(indices[4 * m_tet_index + 2] + kOffset);
    Vector3r &x3 = pd.GetPosition(indices[4 * m_tet_index + 3] + kOffset);

    Vector3r corr0;
    Vector3r corr[4];
    const bool kRes = PositionBasedDynamics::SolveParticleTetContactConstraint(
            pd.GetInvMass(m_bodies[0]), pd.GetPosition(m_bodies[0]), m_inv_masses, m_x.data(), m_bary,
            m_constraint_info, m_lambda, corr0, corr);

    if (kRes) {
        if (pd.GetMass(m_bodies[0]) != 0.0) pd.GetPosition(m_bodies[0]) += corr0;
        if (m_inv_masses[0] != 0.0) x0 += corr[0];
        if (m_inv_masses[1] != 0.0) x1 += corr[1];
        if (m_inv_masses[2] != 0.0) x2 += corr[2];
        if (m_inv_masses[3] != 0.0) x3 += corr[3];
    }
    return kRes;
}

bool ParticleTetContactConstraint::SolveVelocityConstraint(SimulationModel &model, unsigned int iter) {
    ParticleData &pd = model.getParticles();

    const SimulationModel::TetModelVector &tet_models = model.getTetModels();
    TetModel *tm = tet_models[m_solid_index];
    const unsigned int kOffset = tm->getIndexOffset();
    const unsigned int *indices = tm->getParticleMesh().GetTets().data();
    Vector3r &v0 = pd.GetVelocity(indices[4 * m_tet_index] + kOffset);
    Vector3r &v1 = pd.GetVelocity(indices[4 * m_tet_index + 1] + kOffset);
    Vector3r &v2 = pd.GetVelocity(indices[4 * m_tet_index + 2] + kOffset);
    Vector3r &v3 = pd.GetVelocity(indices[4 * m_tet_index + 3] + kOffset);
    m_v[0] = v0;
    m_v[1] = v1;
    m_v[2] = v2;
    m_v[3] = v3;

    Vector3r corr_v0;
    Vector3r corr_v[4];
    const bool kRes = PositionBasedDynamics::VelocitySolveParticleTetContactConstraint(
            pd.GetInvMass(m_bodies[0]), pd.GetPosition(m_bodies[0]), pd.GetVelocity(m_bodies[0]), m_inv_masses,
            m_x.data(), m_v.data(), m_bary, m_lambda, m_friction_coeff, m_constraint_info, corr_v0, corr_v);

    if (kRes) {
        if (pd.GetMass(m_bodies[0]) != 0.0) pd.GetVelocity(m_bodies[0]) += corr_v0;
        if (m_inv_masses[0] != 0.0) v0 += corr_v[0];
        if (m_inv_masses[1] != 0.0) v1 += corr_v[1];
        if (m_inv_masses[2] != 0.0) v2 += corr_v[2];
        if (m_inv_masses[3] != 0.0) v3 += corr_v[3];
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// StretchShearConstraint
//////////////////////////////////////////////////////////////////////////
bool StretchShearConstraint::InitConstraint(SimulationModel &model,
                                            const unsigned int particle1,
                                            const unsigned int particle2,
                                            const unsigned int quaternion1,
                                            const Real stretching_stiffness,
                                            const Real shearing_stiffness_1,
                                            const Real shearing_stiffness_2) {
    m_stretching_stiffness = stretching_stiffness;
    m_shearing_stiffness_1 = shearing_stiffness_1;
    m_shearing_stiffness_2 = shearing_stiffness_2;
    m_bodies[0] = particle1;
    m_bodies[1] = particle2;
    m_bodies[2] = quaternion1;
    ParticleData &pd = model.getParticles();

    const Vector3r &x1_0 = pd.GetPosition0(particle1);
    const Vector3r &x2_0 = pd.GetPosition0(particle2);

    m_rest_length = (x2_0 - x1_0).norm();

    return true;
}

bool StretchShearConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    ParticleData &pd = model.getParticles();
    OrientationData &od = model.getOrientations();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];
    const unsigned kIq1 = m_bodies[2];

    Vector3r &x1 = pd.GetPosition(kI1);
    Vector3r &x2 = pd.GetPosition(kI2);
    Quaternionr &q1 = od.GetQuaternion(kIq1);
    const Real kInvMass1 = pd.GetInvMass(kI1);
    const Real kInvMass2 = pd.GetInvMass(kI2);
    const Real kInvMassq1 = od.getInvMass(kIq1);
    Vector3r stiffness(m_shearing_stiffness_1, m_shearing_stiffness_2, m_stretching_stiffness);

    Vector3r corr1, corr2;
    Quaternionr corrq1;
    const bool kRes = PositionBasedCosseratRods::solve_StretchShearConstraint(
            x1, kInvMass1, x2, kInvMass2, q1, kInvMassq1, stiffness, m_rest_length, corr1, corr2, corrq1);

    if (kRes) {
        if (kInvMass1 != 0.0) x1 += corr1;
        if (kInvMass2 != 0.0) x2 += corr2;
        if (kInvMassq1 != 0.0) {
            q1.coeffs() += corrq1.coeffs();
            q1.normalize();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// BendTwistConstraint
//////////////////////////////////////////////////////////////////////////
bool BendTwistConstraint::InitConstraint(SimulationModel &model,
                                         const unsigned int quaternion1,
                                         const unsigned int quaternion2,
                                         const Real twisting_stiffness,
                                         const Real bending_stiffness_1,
                                         const Real bending_stiffness_2) {
    m_twisting_stiffness = twisting_stiffness;
    m_bending_stiffness_1 = bending_stiffness_1;
    m_bending_stiffness_2 = bending_stiffness_2;
    m_bodies[0] = quaternion1;
    m_bodies[1] = quaternion2;
    OrientationData &od = model.getOrientations();

    const Quaternionr &q1_0 = od.GetQuaternion(quaternion1);
    const Quaternionr &q2_0 = od.GetQuaternion(quaternion2);

    m_rest_darboux_vector = q1_0.conjugate() * q2_0;
    Quaternionr omega_plus, omega_minus;
    omega_plus.coeffs() = m_rest_darboux_vector.coeffs() + Quaternionr(1, 0, 0, 0).coeffs();
    omega_minus.coeffs() = m_rest_darboux_vector.coeffs() - Quaternionr(1, 0, 0, 0).coeffs();
    if (omega_minus.squaredNorm() > omega_plus.squaredNorm()) m_rest_darboux_vector.coeffs() *= -1.0;

    return true;
}

bool BendTwistConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    OrientationData &od = model.getOrientations();

    const unsigned kI1 = m_bodies[0];
    const unsigned kI2 = m_bodies[1];

    Quaternionr &q1 = od.GetQuaternion(kI1);
    Quaternionr &q2 = od.GetQuaternion(kI2);
    const Real kInvMass1 = od.getInvMass(kI1);
    const Real kInvMass2 = od.getInvMass(kI2);
    Vector3r stiffness(m_bending_stiffness_1, m_bending_stiffness_2, m_twisting_stiffness);

    Quaternionr corr1, corr2;
    const bool kRes = PositionBasedCosseratRods::solve_BendTwistConstraint(q1, kInvMass1, q2, kInvMass2, stiffness,
                                                                           m_rest_darboux_vector, corr1, corr2);

    if (kRes) {
        if (kInvMass1 != 0.0) {
            q1.coeffs() += corr1.coeffs();
            q1.normalize();
        }

        if (kInvMass2 != 0.0) {
            q2.coeffs() += corr2.coeffs();
            q2.normalize();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// StretchBendingTwistingConstraint
//////////////////////////////////////////////////////////////////////////

bool StretchBendingTwistingConstraint::InitConstraint(SimulationModel &model,
                                                      const unsigned int segment_index_1,
                                                      const unsigned int segment_index_2,
                                                      const Vector3r &pos,
                                                      const Real average_radius,
                                                      const Real average_segment_length,
                                                      Real youngs_modulus,
                                                      Real torsion_modulus) {
    m_bodies[0] = segment_index_1;
    m_bodies[1] = segment_index_2;
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    const RigidBody &segment1 = *rb[m_bodies[0]];
    const RigidBody &segment2 = *rb[m_bodies[1]];

    m_lambda_sum.setZero();
    m_average_radius = average_radius;
    m_average_segment_length = average_segment_length;

    return DirectPositionBasedSolverForStiffRods::init_StretchBendingTwistingConstraint(
            segment1.getPosition(), segment1.getRotation(), segment2.getPosition(), segment2.getRotation(), pos,
            m_average_radius, m_average_segment_length, youngs_modulus, torsion_modulus, m_constraint_info,
            m_stiffness_coefficient_k, m_rest_darboux_vector);
}

bool StretchBendingTwistingConstraint::InitConstraintBeforeProjection(SimulationModel &model) {
    DirectPositionBasedSolverForStiffRods::initBeforeProjection_StretchBendingTwistingConstraint(
            m_stiffness_coefficient_k, static_cast<Real>(1.0) / TimeManager::getCurrent()->getTimeStepSize(),
            m_average_segment_length, m_stretch_compliance, m_bending_and_torsion_compliance, m_lambda_sum);
    return true;
}

bool StretchBendingTwistingConstraint::UpdateConstraint(SimulationModel &model) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    const RigidBody &segment1 = *rb[m_bodies[0]];
    const RigidBody &segment2 = *rb[m_bodies[1]];
    return DirectPositionBasedSolverForStiffRods::update_StretchBendingTwistingConstraint(
            segment1.getPosition(), segment1.getRotation(), segment2.getPosition(), segment2.getRotation(),
            m_constraint_info);
}

bool StretchBendingTwistingConstraint::SolvePositionConstraint(SimulationModel &model, const unsigned int iter) {
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();

    RigidBody &segment1 = *rb[m_bodies[0]];
    RigidBody &segment2 = *rb[m_bodies[1]];

    Vector3r corr_x1, corr_x2;
    Quaternionr corr_q1, corr_q2;
    const bool kRes = DirectPositionBasedSolverForStiffRods::solve_StretchBendingTwistingConstraint(
            segment1.getInvMass(), segment1.getPosition(), segment1.getInertiaTensorInverseW(), segment1.getRotation(),
            segment2.getInvMass(), segment2.getPosition(), segment2.getInertiaTensorInverseW(), segment2.getRotation(),
            m_rest_darboux_vector, m_average_segment_length, m_stretch_compliance, m_bending_and_torsion_compliance,
            m_constraint_info, corr_x1, corr_q1, corr_x2, corr_q2, m_lambda_sum);

    if (kRes) {
        if (segment1.getMass() != 0.0) {
            segment1.getPosition() += corr_x1;
            segment1.getRotation().coeffs() += corr_q1.coeffs();
            segment1.getRotation().normalize();
            segment1.rotationUpdated();
        }
        if (segment2.getMass() != 0.0) {
            segment2.getPosition() += corr_x2;
            segment2.getRotation().coeffs() += corr_q2.coeffs();
            segment2.getRotation().normalize();
            segment2.rotationUpdated();
        }
    }
    return kRes;
}

//////////////////////////////////////////////////////////////////////////
// DirectPositionBasedSolverForStiffRodsConstraint
//////////////////////////////////////////////////////////////////////////

DirectPositionBasedSolverForStiffRodsConstraint::~DirectPositionBasedSolverForStiffRodsConstraint() {
    DeleteNodes();
    delete[] intervals;
    delete[] forward;
    delete[] backward;
    delete[] root;
    root = nullptr;
    forward = nullptr;
    backward = nullptr;
    intervals = nullptr;
    number_of_intervals = 0;
}

void DirectPositionBasedSolverForStiffRodsConstraint::DeleteNodes() {
    std::list<Node *>::iterator node_iter;
    for (int i = 0; i < number_of_intervals; i++) {
        for (node_iter = forward[i].begin(); node_iter != forward[i].end(); node_iter++) {
            Node *node = *node_iter;

            // Root node does not have to be deleted
            if (node->parent != nullptr) delete node;
        }
    }
}

bool DirectPositionBasedSolverForStiffRodsConstraint::InitConstraint(
        SimulationModel &model,
        const std::vector<std::pair<unsigned int, unsigned int>> &constraint_segment_indices,
        const std::vector<Vector3r> &constraint_positions,
        const std::vector<Real> &average_radii,
        const std::vector<Real> &average_segment_lengths,
        const std::vector<Real> &youngs_moduli,
        const std::vector<Real> &torsion_moduli) {
    // create unique segment indices from joints

    std::set<unsigned int> unique_segment_indices;
    for (auto &idxPair : constraint_segment_indices) {
        unique_segment_indices.insert(idxPair.first);
        unique_segment_indices.insert(idxPair.second);
    }

    m_bodies.resize(unique_segment_indices.size());

    // initialize m_bodies for constraint colouring algorithm of multi threading implementation

    size_t segment_idx(0);

    for (auto idx : unique_segment_indices) {
        m_bodies[segment_idx] = idx;
        ++segment_idx;
    }

    // create RodSegment instances and map simulation model body indices to RodSegment indices

    std::map<unsigned int, unsigned int> idx_map;
    unsigned int idx(0);

    m_segments.reserve(unique_segment_indices.size());
    m_rod_segments.reserve(unique_segment_indices.size());
    for (auto body_idx : unique_segment_indices) {
        idx = (unsigned int)m_segments.size();
        idx_map[body_idx] = idx;
        m_segments.emplace_back(model, body_idx);
        m_rod_segments.push_back(&m_segments.back());
    }

    // create rod constraints

    m_constraints.resize(constraint_positions.size());
    m_rod_constraints.resize(constraint_positions.size());

    for (size_t idx(0); idx < constraint_positions.size(); ++idx) {
        const std::pair<unsigned int, unsigned int> &body_indices(constraint_segment_indices[idx]);
        unsigned int first_segment_index(idx_map.find(body_indices.first)->second);
        unsigned int second_segment_index(idx_map.find(body_indices.second)->second);

        m_constraints[idx].m_segments.push_back(first_segment_index);
        m_constraints[idx].m_segments.push_back(second_segment_index);
        m_constraints[idx].m_average_segment_length = average_segment_lengths[idx];
        m_rod_constraints[idx] = &m_constraints[idx];
    }

    // initialize data of the sparse direct solver
    DeleteNodes();
    DirectPositionBasedSolverForStiffRods::init_DirectPositionBasedSolverForStiffRodsConstraint(
            m_rod_constraints, m_rod_segments, intervals, number_of_intervals, forward, backward, root,
            constraint_positions, average_radii, youngs_moduli, torsion_moduli, m_right_hand_side, m_lambda_sums,
            m_bending_and_torsion_jacobians, m_corr_x, m_corr_q);

    return true;
}

bool DirectPositionBasedSolverForStiffRodsConstraint::InitConstraintBeforeProjection(SimulationModel &model) {
    DirectPositionBasedSolverForStiffRods::initBeforeProjection_DirectPositionBasedSolverForStiffRodsConstraint(
            m_rod_constraints, static_cast<Real>(1.0) / TimeManager::getCurrent()->getTimeStepSize(), m_lambda_sums);
    return true;
}

bool DirectPositionBasedSolverForStiffRodsConstraint::UpdateConstraint(SimulationModel &model) {
    DirectPositionBasedSolverForStiffRods::update_DirectPositionBasedSolverForStiffRodsConstraint(m_rod_constraints,
                                                                                                  m_rod_segments);
    return true;
}

bool DirectPositionBasedSolverForStiffRodsConstraint::SolvePositionConstraint(SimulationModel &model,
                                                                              const unsigned int iter) {
    const bool kRes = DirectPositionBasedSolverForStiffRods::solve_DirectPositionBasedSolverForStiffRodsConstraint(
            m_rod_constraints, m_rod_segments, intervals, number_of_intervals, forward, backward, m_right_hand_side,
            m_lambda_sums, m_bending_and_torsion_jacobians, m_corr_x, m_corr_q);

    // apply corrections to bodies
    SimulationModel::RigidBodyVector &rbs = model.getRigidBodies();

    for (size_t i(0); i < m_rod_segments.size(); ++i) {
        RodSegmentImpl &segment = m_segments[i];
        RigidBody &rb1 = *rbs[segment.m_segment_idx];
        if (rb1.getMass() != 0.0) {
            rb1.getPosition() += m_corr_x[i];
            rb1.getRotation().coeffs() += m_corr_q[i].coeffs();
            rb1.getRotation().normalize();
            rb1.rotationUpdated();
        }
    }

    return kRes;
}

bool DirectPositionBasedSolverForStiffRodsConstraint::RodSegmentImpl::IsDynamic() {
    return 0 != (m_model.getRigidBodies())[m_segment_idx]->getMass();
}

Real DirectPositionBasedSolverForStiffRodsConstraint::RodSegmentImpl::Mass() {
    return (m_model.getRigidBodies())[m_segment_idx]->getMass();
}

const Vector3r &DirectPositionBasedSolverForStiffRodsConstraint::RodSegmentImpl::InertiaTensor() {
    return (m_model.getRigidBodies())[m_segment_idx]->getInertiaTensor();
}

const Vector3r &DirectPositionBasedSolverForStiffRodsConstraint::RodSegmentImpl::Position() {
    return (m_model.getRigidBodies())[m_segment_idx]->getPosition();
}

const Quaternionr &DirectPositionBasedSolverForStiffRodsConstraint::RodSegmentImpl::Rotation() {
    return (m_model.getRigidBodies())[m_segment_idx]->getRotation();
}

}  // namespace vox::force