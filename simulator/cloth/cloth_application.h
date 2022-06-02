//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <map>

#include "simulator/cloth/cloth_renderer.h"
#include "simulator/cloth/job_manager.h"
#include "vox.cloth/NvCloth/Cloth.h"
#include "vox.cloth/NvCloth/Fabric.h"
#include "vox.cloth/NvCloth/Solver.h"
#include "vox.render/forward_application.h"

namespace vox::cloth {
class ClothApplication : public ForwardApplication {
public:
    struct ClothActor {
        ClothRenderer *cloth_renderer{nullptr};
        nv::cloth::Cloth *cloth{nullptr};
    };

    ClothApplication();

    void Update(float delta_time) override;

    ~ClothApplication() override;

protected:
    nv::cloth::Factory *factory_{nullptr};

    // Helper functions to enable automatic deinitialize
    // Tracking an object will delete it when autoDeinitialize is called
    // Untracking can be used if you delete it sooner than autoDeinitialize
    void TrackClothActor(ClothActor *cloth_actor);

    void UntrackClothActor(ClothActor *cloth_actor);

    void TrackSolver(nv::cloth::Solver *solver);

    void UntrackSolver(nv::cloth::Solver *solver);

    void TrackFabric(nv::cloth::Fabric *fabric);

    void UntrackFabric(nv::cloth::Fabric *fabric);

    // Help to detach cloths from solver at AutoDeinit.
    void AddClothToSolver(ClothActor *cloth_actor, nv::cloth::Solver *solver);

    void AddClothsToSolver(nv::cloth::Range<ClothActor *> cloth_actors, nv::cloth::Solver *solver);

    void StartSimulationStep(float dt);

    void WaitForSimulationStep();

    void UpdateSimulationGraphics();

private:
    std::vector<ClothActor *> cloth_list_;
    std::vector<nv::cloth::Solver *> solver_list_;
    std::map<nv::cloth::Solver *, MultithreadedSolverHelper> solver_helpers_;
    std::vector<nv::cloth::Fabric *> fabric_list_;
    std::map<ClothActor *, nv::cloth::Solver *> cloth_solver_map_;

    JobManager job_manager_;
};

}  // namespace vox::cloth