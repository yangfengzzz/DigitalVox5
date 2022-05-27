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
    static const unsigned int RigidBodyContactType;          // = 0;
    static const unsigned int ParticleContactType;           // = 1;
    static const unsigned int ParticleRigidBodyContactType;  // = 2;
    static const unsigned int ParticleSolidContactType;      // = 3;

    typedef void (*ContactCallbackFunction)(const unsigned int contactType,
                                            const unsigned int bodyIndex1,
                                            const unsigned int bodyIndex2,
                                            const Vector3r &cp1,
                                            const Vector3r &cp2,
                                            const Vector3r &normal,
                                            const Real dist,
                                            const Real restitutionCoeff,
                                            const Real frictionCoeff,
                                            void *userData);

    typedef void (*SolidContactCallbackFunction)(const unsigned int contactType,
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
                                                 void *userData);

    struct CollisionObject {
        static const unsigned int RigidBodyCollisionObjectType;      // = 0;
        static const unsigned int TriangleModelCollisionObjectType;  // = 1;
        static const unsigned int TetModelCollisionObjectType;       // = 2;

        AABB m_aabb;
        unsigned int m_bodyIndex;
        unsigned int m_bodyType;

        virtual ~CollisionObject() {}
        virtual int &getTypeId() const = 0;
    };

    struct CollisionObjectWithoutGeometry : public CollisionObject {
        static int TYPE_ID;
        virtual int &getTypeId() const { return TYPE_ID; }
        virtual ~CollisionObjectWithoutGeometry() {}
    };

protected:
    Real m_tolerance;
    ContactCallbackFunction m_contactCB;
    SolidContactCallbackFunction m_solidContactCB;
    void *m_contactCBUserData;
    void *m_solidContactCBUserData;
    std::vector<CollisionObject *> m_collisionObjects;

    void updateAABB(const Vector3r &p, AABB &aabb);

public:
    CollisionDetection();
    virtual ~CollisionDetection();

    void cleanup();

    Real getTolerance() const { return m_tolerance; }
    void setTolerance(Real val) { m_tolerance = val; }

    void addRigidBodyContact(const unsigned int rbIndex1,
                             const unsigned int rbIndex2,
                             const Vector3r &cp1,
                             const Vector3r &cp2,
                             const Vector3r &normal,
                             const Real dist,
                             const Real restitutionCoeff,
                             const Real frictionCoeff);

    void addParticleRigidBodyContact(const unsigned int particleIndex,
                                     const unsigned int rbIndex,
                                     const Vector3r &cp1,
                                     const Vector3r &cp2,
                                     const Vector3r &normal,
                                     const Real dist,
                                     const Real restitutionCoeff,
                                     const Real frictionCoeff);

    void addParticleSolidContact(const unsigned int particleIndex,
                                 const unsigned int solidIndex,
                                 const unsigned int tetIndex,
                                 const Vector3r &bary,
                                 const Vector3r &cp1,
                                 const Vector3r &cp2,
                                 const Vector3r &normal,
                                 const Real dist,
                                 const Real restitutionCoeff,
                                 const Real frictionCoeff);

    virtual void addCollisionObject(const unsigned int bodyIndex, const unsigned int bodyType);

    std::vector<CollisionObject *> &getCollisionObjects() { return m_collisionObjects; }

    virtual void collisionDetection(SimulationModel &model) = 0;

    void setContactCallback(CollisionDetection::ContactCallbackFunction val, void *userData);
    void setSolidContactCallback(CollisionDetection::SolidContactCallbackFunction val, void *userData);
    void updateAABBs(SimulationModel &model);
    void updateAABB(SimulationModel &model, CollisionDetection::CollisionObject *co);
};
}  // namespace vox::force