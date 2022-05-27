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
/** \brief Class to manage the current simulation time and the time step size.
 * This class is a singleton.
 */
class Simulation {
public:
    static int GRAVITATION;

protected:
    SimulationModel *m_model{};
    TimeStep *m_timeStep;
    Vector3r m_gravitation;

private:
    static Simulation *current;

public:
    Simulation();
    Simulation(const Simulation &) = delete;
    Simulation &operator=(const Simulation &) = delete;
    ~Simulation();

    void init();
    void reset();

    // Singleton
    static Simulation *getCurrent();
    static void setCurrent(Simulation *tm);
    static bool hasCurrent();

    SimulationModel *getModel() { return m_model; }
    void setModel(SimulationModel *model) { m_model = model; }

    TimeStep *getTimeStep() { return m_timeStep; }
    void setTimeStep(TimeStep *ts) { m_timeStep = ts; }
};
}  // namespace vox::force