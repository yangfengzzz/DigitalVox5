//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/time_step_controller.h"

#include "vox.force/time_integration.h"
#include "vox.force/time_manager.h"
#include "vox.force/timing.h"

namespace vox::force {

// int TimeStepController::SOLVER_ITERATIONS = -1;
// int TimeStepController::SOLVER_ITERATIONS_V = -1;
int TimeStepController::num_sub_steps_ = -1;
int TimeStepController::max_iterations_ = -1;
int TimeStepController::max_iterations_v_ = -1;
int TimeStepController::velocity_update_method_ = -1;
int TimeStepController::enum_vupdate_first_order_ = -1;
int TimeStepController::enum_vupdate_second_order_ = -1;

TimeStepController::TimeStepController() {
    m_velocity_update_method_ = 0;
    m_iterations_ = 0;
    m_iterations_v_ = 0;
    m_max_iterations_ = 1;
    m_max_iterations_v_ = 5;
    m_sub_steps_ = 5;
    m_collision_detection_ = nullptr;
}

TimeStepController::~TimeStepController() = default;

void TimeStepController::Step(SimulationModel &model) {
    START_TIMING("simulation step")
    TimeManager *tm = TimeManager::GetCurrent();
    const Real kHOld = tm->GetTimeStepSize();

    //////////////////////////////////////////////////////////////////////////
    // rigid body model
    //////////////////////////////////////////////////////////////////////////
    ClearAccelerations(model);
    SimulationModel::RigidBodyVector &rb = model.GetRigidBodies();
    ParticleData &pd = model.GetParticles();
    OrientationData &od = model.GetOrientations();

    const int kNumBodies = (int)rb.size();

    Real h = kHOld / (Real)m_sub_steps_;
    tm->SetTimeStepSize(h);
    for (unsigned int step = 0; step < m_sub_steps_; step++) {
#pragma omp parallel if (kNumBodies > MIN_PARALLEL_SIZE) shared(rb, h, pd, kNumBodies, od) default(none)
        {
#pragma omp for schedule(static) nowait
            for (int i = 0; i < kNumBodies; i++) {
                rb[i]->GetLastPosition() = rb[i]->GetOldPosition();
                rb[i]->GetOldPosition() = rb[i]->GetPosition();
                TimeIntegration::SemiImplicitEuler(h, rb[i]->GetMass(), rb[i]->GetPosition(), rb[i]->GetVelocity(),
                                                   rb[i]->GetAcceleration());
                rb[i]->GetLastRotation() = rb[i]->GetOldRotation();
                rb[i]->GetOldRotation() = rb[i]->GetRotation();
                TimeIntegration::SemiImplicitEulerRotation(h, rb[i]->GetMass(), rb[i]->GetInertiaTensorW(),
                                                           rb[i]->GetInertiaTensorInverseW(), rb[i]->GetRotation(),
                                                           rb[i]->GetAngularVelocity(), rb[i]->GetTorque());
                rb[i]->RotationUpdated();
            }

//////////////////////////////////////////////////////////////////////////
// particle model
//////////////////////////////////////////////////////////////////////////
#pragma omp for schedule(static)
            for (int i = 0; i < (int)pd.Size(); i++) {
                pd.GetLastPosition(i) = pd.GetOldPosition(i);
                pd.GetOldPosition(i) = pd.GetPosition(i);
                TimeIntegration::SemiImplicitEuler(h, pd.GetMass(i), pd.GetPosition(i), pd.GetVelocity(i),
                                                   pd.GetAcceleration(i));
            }

//////////////////////////////////////////////////////////////////////////
// orientation model
//////////////////////////////////////////////////////////////////////////
#pragma omp for schedule(static)
            for (int i = 0; i < (int)od.Size(); i++) {
                od.GetLastQuaternion(i) = od.GetOldQuaternion(i);
                od.GetOldQuaternion(i) = od.GetQuaternion(i);
                TimeIntegration::SemiImplicitEulerRotation(h, od.GetMass(i), od.GetMass(i) * Matrix3r::Identity(),
                                                           od.GetInvMass(i) * Matrix3r::Identity(), od.GetQuaternion(i),
                                                           od.GetVelocity(i), Vector3r(0, 0, 0));
            }
        }

        START_TIMING("position constraints projection")
        PositionConstraintProjection(model);
        STOP_TIMING_AVG
#pragma omp parallel if (kNumBodies > MIN_PARALLEL_SIZE) shared(kNumBodies, pd, rb, h, od) default(none)
        {
// Update velocities
#pragma omp for schedule(static) nowait
            for (int i = 0; i < kNumBodies; i++) {
                if (m_velocity_update_method_ == 0) {
                    TimeIntegration::VelocityUpdateFirstOrder(h, rb[i]->GetMass(), rb[i]->GetPosition(),
                                                              rb[i]->GetOldPosition(), rb[i]->GetVelocity());
                    TimeIntegration::AngularVelocityUpdateFirstOrder(h, rb[i]->GetMass(), rb[i]->GetRotation(),
                                                                     rb[i]->GetOldRotation(),
                                                                     rb[i]->GetAngularVelocity());
                } else {
                    TimeIntegration::VelocityUpdateSecondOrder(h, rb[i]->GetMass(), rb[i]->GetPosition(),
                                                               rb[i]->GetOldPosition(), rb[i]->GetLastPosition(),
                                                               rb[i]->GetVelocity());
                    TimeIntegration::AngularVelocityUpdateSecondOrder(h, rb[i]->GetMass(), rb[i]->GetRotation(),
                                                                      rb[i]->GetOldRotation(), rb[i]->GetLastRotation(),
                                                                      rb[i]->GetAngularVelocity());
                }
            }

// Update velocities
#pragma omp for schedule(static)
            for (int i = 0; i < (int)pd.Size(); i++) {
                if (m_velocity_update_method_ == 0)
                    TimeIntegration::VelocityUpdateFirstOrder(h, pd.GetMass(i), pd.GetPosition(i), pd.GetOldPosition(i),
                                                              pd.GetVelocity(i));
                else
                    TimeIntegration::VelocityUpdateSecondOrder(h, pd.GetMass(i), pd.GetPosition(i),
                                                               pd.GetOldPosition(i), pd.GetLastPosition(i),
                                                               pd.GetVelocity(i));
            }

// Update velocites of orientations
#pragma omp for schedule(static)
            for (int i = 0; i < (int)od.Size(); i++) {
                if (m_velocity_update_method_ == 0)
                    TimeIntegration::AngularVelocityUpdateFirstOrder(h, od.GetMass(i), od.GetQuaternion(i),
                                                                     od.GetOldQuaternion(i), od.GetVelocity(i));
                else
                    TimeIntegration::AngularVelocityUpdateSecondOrder(h, od.GetMass(i), od.GetQuaternion(i),
                                                                      od.GetOldQuaternion(i), od.GetLastQuaternion(i),
                                                                      od.GetVelocity(i));
            }
        }
    }
    h = kHOld;
    tm->SetTimeStepSize(kHOld);

#pragma omp parallel shared(kNumBodies, rb) default(none)
    {
#pragma omp for schedule(static) nowait
        for (int i = 0; i < kNumBodies; i++) {
            if (rb[i]->GetMass() != 0.0)
                rb[i]->GetGeometry().UpdateMeshTransformation(rb[i]->GetPosition(), rb[i]->GetRotationMatrix());
        }
    }

    if (m_collision_detection_) {
        START_TIMING("collision detection")
        m_collision_detection_->GetCollisionDetection(model);
        STOP_TIMING_AVG
    }

    VelocityConstraintProjection(model);

    //////////////////////////////////////////////////////////////////////////
    // update motor joint targets
    //////////////////////////////////////////////////////////////////////////
    SimulationModel::ConstraintVector &constraints = model.GetConstraints();
    for (auto &constraint : constraints) {
        if ((constraint->GetTypeId() == TargetAngleMotorHingeJoint::type_id) ||
            (constraint->GetTypeId() == TargetVelocityMotorHingeJoint::type_id) ||
            (constraint->GetTypeId() == TargetPositionMotorSliderJoint::type_id) ||
            (constraint->GetTypeId() == TargetVelocityMotorSliderJoint::type_id)) {
            auto *motor = (MotorJoint *)constraint;
            const std::vector<Real> kSequence = motor->GetTargetSequence();
            if (!kSequence.empty()) {
                Real time = tm->GetTime();
                const Real kSequenceDuration = kSequence[kSequence.size() - 2] - kSequence[0];
                if (motor->GetRepeatSequence()) {
                    while (time > kSequenceDuration) time -= kSequenceDuration;
                }
                unsigned int index = 0;
                while ((2 * index < kSequence.size()) && (kSequence[2 * index] <= time)) index++;

                // linear interpolation
                Real target = 0.0;
                if (2 * index < kSequence.size()) {
                    const Real kAlpha =
                            (time - kSequence[2 * (index - 1)]) / (kSequence[2 * index] - kSequence[2 * (index - 1)]);
                    target = (static_cast<Real>(1.0) - kAlpha) * kSequence[2 * index - 1] +
                             kAlpha * kSequence[2 * index + 1];
                } else
                    target = kSequence[kSequence.size() - 1];
                motor->SetTarget(target);
            }
        }
    }

    // compute new time
    tm->SetTime(tm->GetTime() + h);
    STOP_TIMING_AVG
}

void TimeStepController::Reset() {
    m_iterations_ = 0;
    m_iterations_v_ = 0;
    // m_maxIterations = 5;
    // m_maxIterationsV = 5;
}

void TimeStepController::PositionConstraintProjection(SimulationModel &model) {
    m_iterations_ = 0;

    // init constraint groups if necessary
    model.InitConstraintGroups();

    SimulationModel::RigidBodyVector &rb = model.GetRigidBodies();
    SimulationModel::ConstraintVector &constraints = model.GetConstraints();
    SimulationModel::ConstraintGroupVector &groups = model.GetConstraintGroups();
    SimulationModel::RigidBodyContactConstraintVector &contacts = model.GetRigidBodyContactConstraints();
    SimulationModel::ParticleSolidContactConstraintVector &particle_tet_contacts =
            model.GetParticleSolidContactConstraints();

    // init constraints for this time step if necessary
    for (auto &constraint : constraints) {
        constraint->InitConstraintBeforeProjection(model);
    }

    while (m_iterations_ < m_max_iterations_) {
        for (unsigned int group = 0; group < groups.size(); group++) {
            const int kGroupSize = (int)groups[group].size();
#pragma omp parallel if (kGroupSize > MIN_PARALLEL_SIZE) \
        shared(group, groups, constraints, model, kGroupSize) default(none)
            {
#pragma omp for schedule(static)
                for (int i = 0; i < kGroupSize; i++) {
                    const unsigned int kConstraintIndex = groups[group][i];

                    constraints[kConstraintIndex]->UpdateConstraint(model);
                    constraints[kConstraintIndex]->SolvePositionConstraint(model, m_iterations_);
                }
            }
        }

        for (auto &particle_tet_contact : particle_tet_contacts) {
            particle_tet_contact.SolvePositionConstraint(model, m_iterations_);
        }

        m_iterations_++;
    }
}

void TimeStepController::VelocityConstraintProjection(SimulationModel &model) {
    m_iterations_v_ = 0;

    // init constraint groups if necessary
    model.InitConstraintGroups();

    SimulationModel::RigidBodyVector &rb = model.GetRigidBodies();
    SimulationModel::ConstraintVector &constraints = model.GetConstraints();
    SimulationModel::ConstraintGroupVector &groups = model.GetConstraintGroups();
    SimulationModel::RigidBodyContactConstraintVector &rigid_body_contacts = model.GetRigidBodyContactConstraints();
    SimulationModel::ParticleRigidBodyContactConstraintVector &particle_rigid_body_contacts =
            model.GetParticleRigidBodyContactConstraints();
    SimulationModel::ParticleSolidContactConstraintVector &particle_tet_contacts =
            model.GetParticleSolidContactConstraints();

    for (unsigned int group = 0; group < groups.size(); group++) {
        const int kGroupSize = (int)groups[group].size();
#pragma omp parallel if (kGroupSize > MIN_PARALLEL_SIZE) \
        shared(kGroupSize, groups, group, model, constraints) default(none)
        {
#pragma omp for schedule(static)
            for (int i = 0; i < kGroupSize; i++) {
                const unsigned int kConstraintIndex = groups[group][i];
                constraints[kConstraintIndex]->UpdateConstraint(model);
            }
        }
    }

    while (m_iterations_v_ < m_max_iterations_v_) {
        for (unsigned int group = 0; group < groups.size(); group++) {
            const int kGroupSize = (int)groups[group].size();
#pragma omp parallel if (kGroupSize > MIN_PARALLEL_SIZE) \
        shared(kGroupSize, groups, group, constraints, model) default(none)
            {
#pragma omp for schedule(static)
                for (int i = 0; i < kGroupSize; i++) {
                    const unsigned int kConstraintIndex = groups[group][i];
                    constraints[kConstraintIndex]->SolveVelocityConstraint(model, m_iterations_v_);
                }
            }
        }

        // solve contacts
        for (auto &rigid_body_contact : rigid_body_contacts) {
            rigid_body_contact.SolveVelocityConstraint(model, m_iterations_v_);
        }
        for (auto &particle_rigid_body_contact : particle_rigid_body_contacts) {
            particle_rigid_body_contact.SolveVelocityConstraint(model, m_iterations_v_);
        }
        for (auto &particle_tet_contact : particle_tet_contacts) {
            particle_tet_contact.SolveVelocityConstraint(model, m_iterations_v_);
        }
        m_iterations_v_++;
    }
}

}  // namespace vox::force
