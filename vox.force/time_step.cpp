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

void TimeStep::Init() {}

void TimeStep::ClearAccelerations(SimulationModel &model) {
    //////////////////////////////////////////////////////////////////////////
    // rigid body model
    //////////////////////////////////////////////////////////////////////////

    SimulationModel::RigidBodyVector &rb = model.GetRigidBodies();
    Simulation *sim = Simulation::GetCurrent();
    const Vector3r kGrav(0, -9.8, 0);
    for (auto &i : rb) {
        // Clear accelerations of dynamic particles
        if (i->GetMass() != 0.0) {
            Vector3r &a = i->GetAcceleration();
            a = kGrav;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // particle model
    //////////////////////////////////////////////////////////////////////////

    ParticleData &pd = model.GetParticles();
    const unsigned int kCount = pd.Size();
    for (unsigned int i = 0; i < kCount; i++) {
        // Clear accelerations of dynamic particles
        if (pd.GetMass(i) != 0.0) {
            Vector3r &a = pd.GetAcceleration(i);
            a = kGrav;
        }
    }
}

void TimeStep::Reset() {}

void TimeStep::SetCollisionDetection(SimulationModel &model, CollisionDetection *cd) {
    m_collision_detection_ = cd;
    m_collision_detection_->SetContactCallback(ContactCallbackFunction, &model);
    m_collision_detection_->SetSolidContactCallback(SolidContactCallbackFunction, &model);
}

CollisionDetection *TimeStep::GetCollisionDetection() { return m_collision_detection_; }

void TimeStep::ContactCallbackFunction(unsigned int contact_type,
                                       unsigned int body_index_1,
                                       unsigned int body_index_2,
                                       const Vector3r &cp1,
                                       const Vector3r &cp2,
                                       const Vector3r &normal,
                                       Real dist,
                                       Real restitution_coeff,
                                       Real friction_coeff,
                                       void *user_data) {
    auto *model = (SimulationModel *)user_data;
    if (contact_type == CollisionDetection::rigid_body_contact_type_)
        model->AddRigidBodyContactConstraint(body_index_1, body_index_2, cp1, cp2, normal, dist, restitution_coeff,
                                             friction_coeff);
    else if (contact_type == CollisionDetection::particle_rigid_body_contact_type_)
        model->AddParticleRigidBodyContactConstraint(body_index_1, body_index_2, cp1, cp2, normal, dist,
                                                     restitution_coeff, friction_coeff);
}

void TimeStep::SolidContactCallbackFunction(unsigned int contact_type,
                                            unsigned int body_index_1,
                                            unsigned int body_index_2,
                                            unsigned int tet_index,
                                            const Vector3r &bary,
                                            const Vector3r &cp1,
                                            const Vector3r &cp2,
                                            const Vector3r &normal,
                                            Real dist,
                                            Real restitution_coeff,
                                            Real friction_coeff,
                                            void *user_data) {
    auto *model = (SimulationModel *)user_data;
    if (contact_type == CollisionDetection::particle_solid_contact_type_)
        model->AddParticleSolidContactConstraint(body_index_1, body_index_2, tet_index, bary, cp1, cp2, normal, dist,
                                                 restitution_coeff, friction_coeff);
}

}  // namespace vox::force