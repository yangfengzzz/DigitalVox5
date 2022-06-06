//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <map>

#include "simulator/cloth/cloth_renderer.h"
#include "simulator/cloth/job_manager.h"
#include "vox.base/singleton.h"
#include "vox.cloth/NvCloth/Fabric.h"
#include "vox.cloth/NvCloth/Solver.h"
#include "vox.render/platform/input_events.h"

namespace vox::cloth {
class ClothController : public Singleton<ClothController> {
public:
    static ClothController &GetSingleton();

    static ClothController *GetSingletonPtr();

    ClothController();

    ~ClothController();

    nv::cloth::Factory *Factory();

    void Update(float delta_time);

    void HandlePickingEvent(Camera *main_camera, const InputEvent &input_event);

public:
    // Helper functions to enable automatic deinitialize
    // Tracking an object will delete it when autoDeinitialize is called
    // Untracking can be used if you delete it sooner than autoDeinitialize
    void TrackClothActor(cloth::ClothRenderer *cloth_actor);

    void UntrackClothActor(cloth::ClothRenderer *cloth_actor);

    void TrackSolver(nv::cloth::Solver *solver);

    void UntrackSolver(nv::cloth::Solver *solver);

    void TrackFabric(nv::cloth::Fabric *fabric);

    void UntrackFabric(nv::cloth::Fabric *fabric);

    // Help to detach cloths from solver at AutoDeinit.
    void AddClothToSolver(cloth::ClothRenderer *cloth_actor, nv::cloth::Solver *solver);

    void AddClothsToSolver(nv::cloth::Range<cloth::ClothRenderer *> cloth_actors, nv::cloth::Solver *solver);

private:
    void StartSimulationStep(float dt);

    void WaitForSimulationStep();

    void UpdateSimulationGraphics();

    void UpdateParticleDragging(const Ray3F &ray) const;

private:
    friend class ClothUI;

    nv::cloth::Factory *factory_{nullptr};
    std::vector<cloth::ClothRenderer *> cloth_list_;
    std::vector<nv::cloth::Solver *> solver_list_;
    std::map<nv::cloth::Solver *, cloth::MultithreadedSolverHelper> solver_helpers_;
    std::vector<nv::cloth::Fabric *> fabric_list_;
    std::map<cloth::ClothRenderer *, nv::cloth::Solver *> cloth_solver_map_;

    cloth::JobManager job_manager_;

    // Particle dragging
    struct DraggingParticle {
        cloth::ClothRenderer *tracked_cloth{nullptr};
        float dist = 0;
        float offset = 0;
        int particle_index = 0;
    };
    DraggingParticle dragging_particle_;
};

template <>
inline cloth::ClothController *Singleton<cloth::ClothController>::ms_singleton = 0;

}  // namespace vox::cloth