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

Simulation* Simulation::current_ = nullptr;
int Simulation::gravitation_ = -1;

Simulation::Simulation() {
    m_gravitation_ = Vector3r(0.0, -9.81, 0.0);

    m_time_step_ = nullptr;
}

Simulation::~Simulation() {
    delete m_time_step_;
    delete TimeManager::getCurrent();

    current_ = nullptr;
}

Simulation* Simulation::GetCurrent() {
    if (current_ == nullptr) {
        current_ = new Simulation();
        current_->Init();
    }
    return current_;
}

void Simulation::SetCurrent(Simulation* tm) { current_ = tm; }

bool Simulation::HasCurrent() { return (current_ != nullptr); }

void Simulation::Init() {
    m_time_step_ = new TimeStepController();
    m_time_step_->init();
    TimeManager::getCurrent()->setTimeStepSize(static_cast<Real>(0.005));
}

void Simulation::Reset() {
    m_model_->reset();
    if (m_time_step_) m_time_step_->reset();

    TimeManager::getCurrent()->setTime(static_cast<Real>(0.0));
}

}  // namespace vox::force