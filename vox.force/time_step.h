//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/collision_detection.h"
#include "vox.force/common.h"
#include "vox.force/simulation_model.h"

namespace vox::force {
/** \brief Base class for the simulation methods.
 */
class TimeStep {
protected:
    CollisionDetection *m_collision_detection_{};

    /** Clear accelerations and add gravitation.
     */
    void ClearAccelerations(SimulationModel &model);

    static void ContactCallbackFunction(unsigned int contact_type,
                                        unsigned int body_index_1,
                                        unsigned int body_index_2,
                                        const Vector3r &cp1,
                                        const Vector3r &cp2,
                                        const Vector3r &normal,
                                        Real dist,
                                        Real restitution_coeff,
                                        Real friction_coeff,
                                        void *user_data);

    static void SolidContactCallbackFunction(unsigned int contact_type,
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
                                             void *user_data);

public:
    TimeStep();
    virtual ~TimeStep();

    virtual void Step(SimulationModel &model) = 0;
    virtual void Reset();

    virtual void Init();

    void SetCollisionDetection(SimulationModel &model, CollisionDetection *cd);
    CollisionDetection *GetCollisionDetection();
};
}  // namespace vox::force