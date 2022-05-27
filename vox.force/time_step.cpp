//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/time_step.h"

#include "vox.force/simulation.h"
#include "vox.force/time_manager.h"

namespace vox::force {

TimeStep::TimeStep() = default;

TimeStep::~TimeStep(void) = default;

void TimeStep::init() {}

void TimeStep::clearAccelerations(SimulationModel &model) {
    //////////////////////////////////////////////////////////////////////////
    // rigid body model
    //////////////////////////////////////////////////////////////////////////

    SimulationModel::RigidBodyVector &rb = model.getRigidBodies();
    Simulation *sim = Simulation::getCurrent();
    const Vector3r grav(0, -9.8, 0);
    for (auto &i : rb) {
        // Clear accelerations of dynamic particles
        if (i->getMass() != 0.0) {
            Vector3r &a = i->getAcceleration();
            a = grav;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // particle model
    //////////////////////////////////////////////////////////////////////////

    ParticleData &pd = model.getParticles();
    const unsigned int count = pd.size();
    for (unsigned int i = 0; i < count; i++) {
        // Clear accelerations of dynamic particles
        if (pd.getMass(i) != 0.0) {
            Vector3r &a = pd.getAcceleration(i);
            a = grav;
        }
    }
}

void TimeStep::reset() {}

void TimeStep::setCollisionDetection(SimulationModel &model, CollisionDetection *cd) {
    m_collisionDetection = cd;
    m_collisionDetection->setContactCallback(contactCallbackFunction, &model);
    m_collisionDetection->setSolidContactCallback(solidContactCallbackFunction, &model);
}

CollisionDetection *TimeStep::getCollisionDetection() { return m_collisionDetection; }

void TimeStep::contactCallbackFunction(const unsigned int contactType,
                                       const unsigned int bodyIndex1,
                                       const unsigned int bodyIndex2,
                                       const Vector3r &cp1,
                                       const Vector3r &cp2,
                                       const Vector3r &normal,
                                       const Real dist,
                                       const Real restitutionCoeff,
                                       const Real frictionCoeff,
                                       void *userData) {
    auto *model = (SimulationModel *)userData;
    if (contactType == CollisionDetection::RigidBodyContactType)
        model->addRigidBodyContactConstraint(bodyIndex1, bodyIndex2, cp1, cp2, normal, dist, restitutionCoeff,
                                             frictionCoeff);
    else if (contactType == CollisionDetection::ParticleRigidBodyContactType)
        model->addParticleRigidBodyContactConstraint(bodyIndex1, bodyIndex2, cp1, cp2, normal, dist, restitutionCoeff,
                                                     frictionCoeff);
}

void TimeStep::solidContactCallbackFunction(const unsigned int contactType,
                                            const unsigned int bodyIndex1,
                                            const unsigned int bodyIndex2,
                                            const unsigned int tetIndex,
                                            const Vector3r &bary,
                                            const Vector3r &cp1,
                                            const Vector3r &cp2,
                                            const Vector3r &normal,
                                            const Real dist,
                                            const Real restitutionCoeff,
                                            const Real frictionCoeff,
                                            void *userData) {
    auto *model = (SimulationModel *)userData;
    if (contactType == CollisionDetection::ParticleSolidContactType)
        model->addParticleSolidContactConstraint(bodyIndex1, bodyIndex2, tetIndex, bary, cp1, cp2, normal, dist,
                                                 restitutionCoeff, frictionCoeff);
}

}  // namespace vox::force