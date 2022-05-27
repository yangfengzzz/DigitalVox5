//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ParameterObject.h"
#include "vox.force/collision_detection.h"
#include "vox.force/common.h"
#include "vox.force/simulation_model.h"

namespace vox::force {
/** \brief Base class for the simulation methods.
 */
class TimeStep : public GenParam::ParameterObject {
protected:
    CollisionDetection *m_collisionDetection;

    /** Clear accelerations and add gravitation.
     */
    void clearAccelerations(SimulationModel &model);

    virtual void initParameters();

    static void contactCallbackFunction(const unsigned int contactType,
                                        const unsigned int bodyIndex1,
                                        const unsigned int bodyIndex2,
                                        const Vector3r &cp1,
                                        const Vector3r &cp2,
                                        const Vector3r &normal,
                                        const Real dist,
                                        const Real restitutionCoeff,
                                        const Real frictionCoeff,
                                        void *userData);

    static void solidContactCallbackFunction(const unsigned int contactType,
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

public:
    TimeStep();
    virtual ~TimeStep(void);

    virtual void step(SimulationModel &model) = 0;
    virtual void reset();

    virtual void init();

    void setCollisionDetection(SimulationModel &model, CollisionDetection *cd);
    CollisionDetection *getCollisionDetection();
};
}  // namespace vox::force