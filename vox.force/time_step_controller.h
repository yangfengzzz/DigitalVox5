//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/collision_detection.h"
#include "vox.force/common.h"
#include "vox.force/simulation_model.h"
#include "vox.force/time_step.h"

namespace vox::force {
class TimeStepController : public TimeStep {
public:
    // 		static int SOLVER_ITERATIONS;
    // 		static int SOLVER_ITERATIONS_V;
    static int NUM_SUB_STEPS;
    static int MAX_ITERATIONS;
    static int MAX_ITERATIONS_V;
    static int VELOCITY_UPDATE_METHOD;

    static int ENUM_VUPDATE_FIRST_ORDER;
    static int ENUM_VUPDATE_SECOND_ORDER;

protected:
    int m_velocityUpdateMethod;
    unsigned int m_iterations;
    unsigned int m_iterationsV;
    unsigned int m_subSteps;
    unsigned int m_maxIterations;
    unsigned int m_maxIterationsV;

    virtual void initParameters();

    void positionConstraintProjection(SimulationModel &model);
    void velocityConstraintProjection(SimulationModel &model);

public:
    TimeStepController();
    virtual ~TimeStepController(void);

    virtual void step(SimulationModel &model);
    virtual void reset();
};
}  // namespace vox::force