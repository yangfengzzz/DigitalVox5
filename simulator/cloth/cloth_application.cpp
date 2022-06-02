//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simulator/cloth/cloth_application.h"

#include "vox.cloth/NvClothExt/ClothFabricCooker.h"

namespace vox::cloth {
using namespace physx;

ClothApplication::~ClothApplication() {
    // Remove all cloths from solvers
    for (auto it : cloth_solver_map_) {
        it.second->removeCloth(it.first->cloth);
    }
    cloth_solver_map_.clear();

    // Destroy all solvers
    for (auto it : solver_list_) {
        delete it;
    }
    solver_list_.clear();
    solver_helpers_.clear();

    // Destroy all cloths
    for (auto it : cloth_list_) {
        delete it->cloth;
    }
    cloth_list_.clear();

    // Destroy all fabrics
    for (auto it : fabric_list_) {
        it->decRefCount();
    }
    fabric_list_.clear();
}

ClothApplication::ClothApplication() : ForwardApplication() {
    NvClothEnvironment::AllocateEnv();
    factory_ = NvClothCreateFactoryCPU();
    if (!factory_) {
        // bug
    }
}

void ClothApplication::Update(float delta_time) {
    StartSimulationStep(delta_time);
    WaitForSimulationStep();
    UpdateSimulationGraphics();

    ForwardApplication::Update(delta_time);
}

void ClothApplication::StartSimulationStep(float dt) {
    for (auto it = solver_helpers_.begin(); it != solver_helpers_.end(); ++it) {
        it->second.StartSimulation(dt);
    }
}

void ClothApplication::WaitForSimulationStep() {
    for (auto it = solver_helpers_.begin(); it != solver_helpers_.end(); ++it) {
        it->second.WaitForSimulation();
    }
}

void ClothApplication::UpdateSimulationGraphics() {
    for (auto actor : cloth_list_) {
        nv::cloth::MappedRange<physx::PxVec4> particles = actor->cloth->getCurrentParticles();
        std::vector<PxVec3> particles3(particles.size());
        for (uint32_t i = 0; i < particles.size(); ++i) particles3[i] = particles[i].getXYZ();

        actor->cloth_renderer->update(particles3.data(), particles.size());
    }
}

namespace {
template <typename T>
void trackT(std::vector<T> &list, T object) {
    list.push_back(object);
}

template <typename T>
void untrackT(std::vector<T> &list, T object) {
    for (auto it = list.begin(); it != list.end(); ++it) {
        if (*it == object) {
            list.erase(it);
            break;
        }
    }
}
}  // namespace

void ClothApplication::TrackClothActor(ClothActor *cloth_actor) { trackT(cloth_list_, cloth_actor); }

void ClothApplication::UntrackClothActor(ClothActor *cloth_actor) { untrackT(cloth_list_, cloth_actor); }

void ClothApplication::TrackSolver(nv::cloth::Solver *solver) {
    trackT(solver_list_, solver);
    solver_helpers_[solver].Initialize(solver, &job_manager_);
}

void ClothApplication::UntrackSolver(nv::cloth::Solver *solver) {
    untrackT(solver_list_, solver);
    solver_helpers_.erase(solver);
}

void ClothApplication::TrackFabric(nv::cloth::Fabric *fabric) { trackT(fabric_list_, fabric); }

void ClothApplication::UntrackFabric(nv::cloth::Fabric *fabric) { untrackT(fabric_list_, fabric); }

void ClothApplication::AddClothToSolver(ClothActor *cloth_actor, nv::cloth::Solver *solver) {
    solver->addCloth(cloth_actor->cloth);
    assert(cloth_solver_map_.find(cloth_actor) == cloth_solver_map_.end());
    cloth_solver_map_[cloth_actor] = solver;
}

void ClothApplication::AddClothsToSolver(nv::cloth::Range<ClothActor *> cloth_actors, nv::cloth::Solver *solver) {
    // A temporary vector of Cloth*, to construct a Range from and pass to solver
    std::vector<nv::cloth::Cloth *> cloths;

    for (uint32_t i = 0; i < cloth_actors.size(); ++i) {
        auto *clothActor = cloth_actors[i];
        assert(cloth_solver_map_.find(clothActor) == cloth_solver_map_.end());
        cloth_solver_map_[clothActor] = solver;

        cloths.push_back(clothActor->cloth);
    }

    auto clothsRange = nv::cloth::Range<nv::cloth::Cloth *>(&cloths.front(), &cloths.back() + 1);
    solver->addCloths(clothsRange);
}

}  // namespace vox::cloth
