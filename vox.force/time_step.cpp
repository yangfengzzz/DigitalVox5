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

    SimulationModel::RigidBodyVector &rb = model.GetRigidBodies();
    Simulation *sim = Simulation::GetCurrent();
    const Vector3r grav(0, -9.8, 0);
    for (auto &i : rb) {
        // Clear accelerations of dynamic particles
        if (i->GetMass() != 0.0) {
            Vector3r &a = i->GetAcceleration();
            a = grav;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // particle model
    //////////////////////////////////////////////////////////////////////////

    ParticleData &pd = model.GetParticles();
    const unsigned int count = pd.size();
    for (unsigned int i = 0; i < count; i++) {
        // Clear accelerations of dynamic particles
        if (pd.GetMass(i) != 0.0) {
            Vector3r &a = pd.GetAcceleration(i);
            a = grav;
        }
    }
}

void TimeStep::reset() {}

void TimeStep::setCollisionDetection(SimulationModel &model, CollisionDetection *cd) {
    m_collisionDetection = cd;
    m_collisionDetection->SetContactCallback(contactCallbackFunction, &model);
    m_collisionDetection->SetSolidContactCallback(solidContactCallbackFunction, &model);
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
    if (contactType == CollisionDetection::rigid_body_contact_type_)
        model->AddRigidBodyContactConstraint(bodyIndex1, bodyIndex2, cp1, cp2, normal, dist, restitutionCoeff,
                                             frictionCoeff);
    else if (contactType == CollisionDetection::particle_rigid_body_contact_type_)
        model->AddParticleRigidBodyContactConstraint(bodyIndex1, bodyIndex2, cp1, cp2, normal, dist, restitutionCoeff,
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
    if (contactType == CollisionDetection::particle_solid_contact_type_)
        model->AddParticleSolidContactConstraint(bodyIndex1, bodyIndex2, tetIndex, bary, cp1, cp2, normal, dist,
                                                 restitutionCoeff, frictionCoeff);
}

}  // namespace vox::force