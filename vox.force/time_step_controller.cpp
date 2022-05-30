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
int TimeStepController::NUM_SUB_STEPS = -1;
int TimeStepController::MAX_ITERATIONS = -1;
int TimeStepController::MAX_ITERATIONS_V = -1;
int TimeStepController::VELOCITY_UPDATE_METHOD = -1;
int TimeStepController::ENUM_VUPDATE_FIRST_ORDER = -1;
int TimeStepController::ENUM_VUPDATE_SECOND_ORDER = -1;

TimeStepController::TimeStepController() {
    m_velocityUpdateMethod = 0;
    m_iterations = 0;
    m_iterationsV = 0;
    m_maxIterations = 1;
    m_maxIterationsV = 5;
    m_subSteps = 5;
    m_collisionDetection = nullptr;
}

TimeStepController::~TimeStepController() = default;

void TimeStepController::step(SimulationModel &model) {
    START_TIMING("simulation step")
    TimeManager *tm = TimeManager::getCurrent();
    const Real hOld = tm->getTimeStepSize();

    //////////////////////////////////////////////////////////////////////////
    // rigid body model
    //////////////////////////////////////////////////////////////////////////
    clearAccelerations(model);
    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    ParticleData &pd = model.getParticles();
    OrientationData &od = model.getOrientations();

    const int numBodies = (int)rb.size();

    Real h = hOld / (Real)m_subSteps;
    tm->setTimeStepSize(h);
    for (unsigned int step = 0; step < m_subSteps; step++) {
#pragma omp parallel if (numBodies > MIN_PARALLEL_SIZE) shared(rb, h, pd, numBodies, od) default(none)
        {
#pragma omp for schedule(static) nowait
            for (int i = 0; i < numBodies; i++) {
                rb[i]->getLastPosition() = rb[i]->getOldPosition();
                rb[i]->getOldPosition() = rb[i]->getPosition();
                TimeIntegration::semiImplicitEuler(h, rb[i]->getMass(), rb[i]->getPosition(), rb[i]->getVelocity(),
                                                   rb[i]->getAcceleration());
                rb[i]->getLastRotation() = rb[i]->getOldRotation();
                rb[i]->getOldRotation() = rb[i]->getRotation();
                TimeIntegration::semiImplicitEulerRotation(h, rb[i]->getMass(), rb[i]->getInertiaTensorW(),
                                                           rb[i]->getInertiaTensorInverseW(), rb[i]->getRotation(),
                                                           rb[i]->getAngularVelocity(), rb[i]->getTorque());
                rb[i]->rotationUpdated();
            }

//////////////////////////////////////////////////////////////////////////
// particle model
//////////////////////////////////////////////////////////////////////////
#pragma omp for schedule(static)
            for (int i = 0; i < (int)pd.size(); i++) {
                pd.GetLastPosition(i) = pd.GetOldPosition(i);
                pd.GetOldPosition(i) = pd.GetPosition(i);
                TimeIntegration::semiImplicitEuler(h, pd.GetMass(i), pd.GetPosition(i), pd.GetVelocity(i),
                                                   pd.GetAcceleration(i));
            }

//////////////////////////////////////////////////////////////////////////
// orientation model
//////////////////////////////////////////////////////////////////////////
#pragma omp for schedule(static)
            for (int i = 0; i < (int)od.size(); i++) {
                od.GetLastQuaternion(i) = od.GetOldQuaternion(i);
                od.GetOldQuaternion(i) = od.GetQuaternion(i);
                TimeIntegration::semiImplicitEulerRotation(h, od.GetMass(i), od.GetMass(i) * Matrix3r::Identity(),
                                                           od.getInvMass(i) * Matrix3r::Identity(), od.GetQuaternion(i),
                                                           od.GetVelocity(i), Vector3r(0, 0, 0));
            }
        }

        START_TIMING("position constraints projection")
        positionConstraintProjection(model);
        STOP_TIMING_AVG
#pragma omp parallel if (numBodies > MIN_PARALLEL_SIZE) shared(numBodies, pd, rb, h, od) default(none)
        {
// Update velocities
#pragma omp for schedule(static) nowait
            for (int i = 0; i < numBodies; i++) {
                if (m_velocityUpdateMethod == 0) {
                    TimeIntegration::velocityUpdateFirstOrder(h, rb[i]->getMass(), rb[i]->getPosition(),
                                                              rb[i]->getOldPosition(), rb[i]->getVelocity());
                    TimeIntegration::angularVelocityUpdateFirstOrder(h, rb[i]->getMass(), rb[i]->getRotation(),
                                                                     rb[i]->getOldRotation(),
                                                                     rb[i]->getAngularVelocity());
                } else {
                    TimeIntegration::velocityUpdateSecondOrder(h, rb[i]->getMass(), rb[i]->getPosition(),
                                                               rb[i]->getOldPosition(), rb[i]->getLastPosition(),
                                                               rb[i]->getVelocity());
                    TimeIntegration::angularVelocityUpdateSecondOrder(h, rb[i]->getMass(), rb[i]->getRotation(),
                                                                      rb[i]->getOldRotation(), rb[i]->getLastRotation(),
                                                                      rb[i]->getAngularVelocity());
                }
            }

// Update velocities
#pragma omp for schedule(static)
            for (int i = 0; i < (int)pd.size(); i++) {
                if (m_velocityUpdateMethod == 0)
                    TimeIntegration::velocityUpdateFirstOrder(h, pd.GetMass(i), pd.GetPosition(i), pd.GetOldPosition(i),
                                                              pd.GetVelocity(i));
                else
                    TimeIntegration::velocityUpdateSecondOrder(h, pd.GetMass(i), pd.GetPosition(i),
                                                               pd.GetOldPosition(i), pd.GetLastPosition(i),
                                                               pd.GetVelocity(i));
            }

// Update velocites of orientations
#pragma omp for schedule(static)
            for (int i = 0; i < (int)od.size(); i++) {
                if (m_velocityUpdateMethod == 0)
                    TimeIntegration::angularVelocityUpdateFirstOrder(h, od.GetMass(i), od.GetQuaternion(i),
                                                                     od.GetOldQuaternion(i), od.GetVelocity(i));
                else
                    TimeIntegration::angularVelocityUpdateSecondOrder(h, od.GetMass(i), od.GetQuaternion(i),
                                                                      od.GetOldQuaternion(i), od.GetLastQuaternion(i),
                                                                      od.GetVelocity(i));
            }
        }
    }
    h = hOld;
    tm->setTimeStepSize(hOld);

#pragma omp parallel shared(numBodies, rb) default(none)
    {
#pragma omp for schedule(static) nowait
        for (int i = 0; i < numBodies; i++) {
            if (rb[i]->getMass() != 0.0)
                rb[i]->getGeometry().updateMeshTransformation(rb[i]->getPosition(), rb[i]->getRotationMatrix());
        }
    }

    if (m_collisionDetection) {
        START_TIMING("collision detection")
        m_collisionDetection->CollisionDetection(model);
        STOP_TIMING_AVG
    }

    velocityConstraintProjection(model);

    //////////////////////////////////////////////////////////////////////////
    // update motor joint targets
    //////////////////////////////////////////////////////////////////////////
    SimulationModel::ConstraintVector &constraints = model.getConstraints();
    for (auto &constraint : constraints) {
        if ((constraint->GetTypeId() == TargetAngleMotorHingeJoint::type_id) ||
            (constraint->GetTypeId() == TargetVelocityMotorHingeJoint::type_id) ||
            (constraint->GetTypeId() == TargetPositionMotorSliderJoint::type_id) ||
            (constraint->GetTypeId() == TargetVelocityMotorSliderJoint::type_id)) {
            auto *motor = (MotorJoint *)constraint;
            const std::vector<Real> sequence = motor->GetTargetSequence();
            if (!sequence.empty()) {
                Real time = tm->getTime();
                const Real sequenceDuration = sequence[sequence.size() - 2] - sequence[0];
                if (motor->GetRepeatSequence()) {
                    while (time > sequenceDuration) time -= sequenceDuration;
                }
                unsigned int index = 0;
                while ((2 * index < sequence.size()) && (sequence[2 * index] <= time)) index++;

                // linear interpolation
                Real target = 0.0;
                if (2 * index < sequence.size()) {
                    const Real alpha =
                            (time - sequence[2 * (index - 1)]) / (sequence[2 * index] - sequence[2 * (index - 1)]);
                    target = (static_cast<Real>(1.0) - alpha) * sequence[2 * index - 1] +
                             alpha * sequence[2 * index + 1];
                } else
                    target = sequence[sequence.size() - 1];
                motor->SetTarget(target);
            }
        }
    }

    // compute new time
    tm->setTime(tm->getTime() + h);
    STOP_TIMING_AVG
}

void TimeStepController::reset() {
    m_iterations = 0;
    m_iterationsV = 0;
    // m_maxIterations = 5;
    // m_maxIterationsV = 5;
}

void TimeStepController::positionConstraintProjection(SimulationModel &model) {
    m_iterations = 0;

    // init constraint groups if necessary
    model.initConstraintGroups();

    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    SimulationModel::ConstraintVector &constraints = model.getConstraints();
    SimulationModel::ConstraintGroupVector &groups = model.getConstraintGroups();
    SimulationModel::RigidBodyContactConstraintVector &contacts = model.getRigidBodyContactConstraints();
    SimulationModel::ParticleSolidContactConstraintVector &particleTetContacts =
            model.getParticleSolidContactConstraints();

    // init constraints for this time step if necessary
    for (auto &constraint : constraints) {
        constraint->InitConstraintBeforeProjection(model);
    }

    while (m_iterations < m_maxIterations) {
        for (unsigned int group = 0; group < groups.size(); group++) {
            const int groupSize = (int)groups[group].size();
#pragma omp parallel if (groupSize > MIN_PARALLEL_SIZE) \
        shared(group, groups, constraints, model, groupSize) default(none)
            {
#pragma omp for schedule(static)
                for (int i = 0; i < groupSize; i++) {
                    const unsigned int constraintIndex = groups[group][i];

                    constraints[constraintIndex]->UpdateConstraint(model);
                    constraints[constraintIndex]->SolvePositionConstraint(model, m_iterations);
                }
            }
        }

        for (auto &particleTetContact : particleTetContacts) {
            particleTetContact.SolvePositionConstraint(model, m_iterations);
        }

        m_iterations++;
    }
}

void TimeStepController::velocityConstraintProjection(SimulationModel &model) {
    m_iterationsV = 0;

    // init constraint groups if necessary
    model.initConstraintGroups();

    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    SimulationModel::ConstraintVector &constraints = model.getConstraints();
    SimulationModel::ConstraintGroupVector &groups = model.getConstraintGroups();
    SimulationModel::RigidBodyContactConstraintVector &rigidBodyContacts = model.getRigidBodyContactConstraints();
    SimulationModel::ParticleRigidBodyContactConstraintVector &particleRigidBodyContacts =
            model.getParticleRigidBodyContactConstraints();
    SimulationModel::ParticleSolidContactConstraintVector &particleTetContacts =
            model.getParticleSolidContactConstraints();

    for (unsigned int group = 0; group < groups.size(); group++) {
        const int groupSize = (int)groups[group].size();
#pragma omp parallel if (groupSize > MIN_PARALLEL_SIZE) \
        shared(groupSize, groups, group, model, constraints) default(none)
        {
#pragma omp for schedule(static)
            for (int i = 0; i < groupSize; i++) {
                const unsigned int constraintIndex = groups[group][i];
                constraints[constraintIndex]->UpdateConstraint(model);
            }
        }
    }

    while (m_iterationsV < m_maxIterationsV) {
        for (unsigned int group = 0; group < groups.size(); group++) {
            const int groupSize = (int)groups[group].size();
#pragma omp parallel if (groupSize > MIN_PARALLEL_SIZE) \
        shared(groupSize, groups, group, constraints, model) default(none)
            {
#pragma omp for schedule(static)
                for (int i = 0; i < groupSize; i++) {
                    const unsigned int constraintIndex = groups[group][i];
                    constraints[constraintIndex]->SolveVelocityConstraint(model, m_iterationsV);
                }
            }
        }

        // solve contacts
        for (auto &rigidBodyContact : rigidBodyContacts) {
            rigidBodyContact.SolveVelocityConstraint(model, m_iterationsV);
        }
        for (auto &particleRigidBodyContact : particleRigidBodyContacts) {
            particleRigidBodyContact.SolveVelocityConstraint(model, m_iterationsV);
        }
        for (auto &particleTetContact : particleTetContacts) {
            particleTetContact.SolveVelocityConstraint(model, m_iterationsV);
        }
        m_iterationsV++;
    }
}

}  // namespace vox::force
