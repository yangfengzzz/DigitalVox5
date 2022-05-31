//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/aabb.h"
#include "vox.force/common.h"
#include "vox.force/simulation_model.h"

namespace vox::force {
class CollisionDetection {
public:
    static const unsigned int rigid_body_contact_type_;           // = 0;
    static const unsigned int particle_contact_type_;             // = 1;
    static const unsigned int particle_rigid_body_contact_type_;  // = 2;
    static const unsigned int particle_solid_contact_type_;       // = 3;

    typedef void (*ContactCallbackFunction)(const unsigned int contact_type,
                                            const unsigned int body_index_1,
                                            const unsigned int body_index_2,
                                            const Vector3r &cp_1,
                                            const Vector3r &cp_2,
                                            const Vector3r &normal,
                                            const Real dist,
                                            const Real restitution_coeff,
                                            const Real friction_coeff,
                                            void *user_data);

    typedef void (*SolidContactCallbackFunction)(const unsigned int contact_type,
                                                 const unsigned int body_index_1,
                                                 const unsigned int body_index_2,
                                                 const unsigned int tet_index,
                                                 const Vector3r &bary,
                                                 const Vector3r &cp_1,
                                                 const Vector3r &cp_2,
                                                 const Vector3r &normal,
                                                 const Real dist,
                                                 const Real restitution_coeff,
                                                 const Real friction_coeff,
                                                 void *user_data);

    struct CollisionObject {
        static const unsigned int rigid_body_collision_object_type;      // = 0;
        static const unsigned int triangle_model_collision_object_type;  // = 1;
        static const unsigned int tet_model_collision_object_type;       // = 2;

        AABB m_aabb;
        unsigned int m_body_index;
        unsigned int m_body_type;

        virtual ~CollisionObject() = default;
        [[nodiscard]] virtual int &GetTypeId() const = 0;
    };

    struct CollisionObjectWithoutGeometry : public CollisionObject {
        static int type_id;
        [[nodiscard]] int &GetTypeId() const override { return type_id; }
        ~CollisionObjectWithoutGeometry() override = default;
    };

protected:
    Real m_tolerance_;
    ContactCallbackFunction m_contact_cb_;
    SolidContactCallbackFunction m_solid_contact_cb_;
    void *m_contact_cb_user_data_{};
    void *m_solid_contact_cb_user_data_{};
    std::vector<CollisionObject *> m_collision_objects_;

    static void UpdateAabb(const Vector3r &p, AABB &aabb);

public:
    CollisionDetection();
    virtual ~CollisionDetection();

    void Cleanup();

    [[nodiscard]] Real GetTolerance() const { return m_tolerance_; }
    void SetTolerance(Real val) { m_tolerance_ = val; }

    void AddRigidBodyContact(unsigned int rb_index_1,
                             unsigned int rb_index_2,
                             const Vector3r &cp_1,
                             const Vector3r &cp_2,
                             const Vector3r &normal,
                             Real dist,
                             Real restitution_coeff,
                             Real friction_coeff);

    void AddParticleRigidBodyContact(unsigned int particle_index,
                                     unsigned int rb_index,
                                     const Vector3r &cp_1,
                                     const Vector3r &cp_2,
                                     const Vector3r &normal,
                                     Real dist,
                                     Real restitution_coeff,
                                     Real friction_coeff);

    void AddParticleSolidContact(unsigned int particle_index,
                                 unsigned int solid_index,
                                 unsigned int tet_index,
                                 const Vector3r &bary,
                                 const Vector3r &cp_1,
                                 const Vector3r &cp_2,
                                 const Vector3r &normal,
                                 Real dist,
                                 Real restitution_coeff,
                                 Real friction_coeff);

    virtual void AddCollisionObject(unsigned int body_index, unsigned int body_type);

    std::vector<CollisionObject *> &GetCollisionObjects() { return m_collision_objects_; }

    virtual void GetCollisionDetection(SimulationModel &model) = 0;

    void SetContactCallback(CollisionDetection::ContactCallbackFunction val, void *user_data);
    void SetSolidContactCallback(CollisionDetection::SolidContactCallbackFunction val, void *user_data);
    void UpdateAabbs(SimulationModel &model);
    void UpdateAabb(SimulationModel &model, CollisionDetection::CollisionObject *co) const;
};
}  // namespace vox::force