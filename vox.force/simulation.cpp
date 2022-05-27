//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.force/simulation.h"

#include "vox.force/time_manager.h"
#include "vox.force/time_step_controller.h"
#include "vox.force/timing.h"

namespace vox::force {

Simulation* Simulation::current = nullptr;
int Simulation::GRAVITATION = -1;

Simulation::Simulation() {
    m_gravitation = Vector3r(0.0, -9.81, 0.0);

    m_timeStep = nullptr;
}

Simulation::~Simulation() {
    delete m_timeStep;
    delete TimeManager::getCurrent();

    current = nullptr;
}

Simulation* Simulation::getCurrent() {
    if (current == nullptr) {
        current = new Simulation();
        current->init();
    }
    return current;
}

void Simulation::setCurrent(Simulation* tm) { current = tm; }

bool Simulation::hasCurrent() { return (current != nullptr); }

void Simulation::init() {
    m_timeStep = new TimeStepController();
    m_timeStep->init();
    TimeManager::getCurrent()->setTimeStepSize(static_cast<Real>(0.005));
}

void Simulation::reset() {
    m_model->reset();
    if (m_timeStep) m_timeStep->reset();

    TimeManager::getCurrent()->setTime(static_cast<Real>(0.0));
}

}  // namespace vox::force