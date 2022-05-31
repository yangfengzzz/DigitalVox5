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
    static int num_sub_steps_;
    static int max_iterations_;
    static int max_iterations_v_;
    static int velocity_update_method_;

    static int enum_vupdate_first_order_;
    static int enum_vupdate_second_order_;

protected:
    int m_velocity_update_method_;
    unsigned int m_iterations_;
    unsigned int m_iterations_v_;
    unsigned int m_sub_steps_;
    unsigned int m_max_iterations_;
    unsigned int m_max_iterations_v_;

    void PositionConstraintProjection(SimulationModel &model);
    void VelocityConstraintProjection(SimulationModel &model);

public:
    TimeStepController();
    ~TimeStepController() override;

    void Step(SimulationModel &model) override;
    void Reset() override;
};
}  // namespace vox::force