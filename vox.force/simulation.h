//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.force/common.h"
#include "vox.force/simulation_model.h"
#include "vox.force/time_step.h"

namespace vox::force {
/**
 * \brief Class to manage the current simulation time and the time step size.
 * This class is a singleton.
 */
class Simulation {
public:
    static int gravitation_;

protected:
    SimulationModel *m_model_{};
    TimeStep *m_time_step_;
    Vector3r m_gravitation_;

private:
    static Simulation *current_;

public:
    Simulation();
    Simulation(const Simulation &) = delete;
    Simulation &operator=(const Simulation &) = delete;
    ~Simulation();

    void Init();
    void Reset();

    // Singleton
    static Simulation *GetCurrent();
    static void SetCurrent(Simulation *tm);
    static bool HasCurrent();

    SimulationModel *GetModel() { return m_model_; }
    void SetModel(SimulationModel *model) { m_model_ = model; }

    TimeStep *GetTimeStep() { return m_time_step_; }
    void SetTimeStep(TimeStep *ts) { m_time_step_ = ts; }
};
}  // namespace vox::force