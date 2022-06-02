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

namespace vox {
namespace cloth {
class ClothController : public Singleton<ClothController> {
public:
    static ClothController &getSingleton(void);

    static ClothController *getSingletonPtr(void);

    ClothController();

    ~ClothController();

    nv::cloth::Factory *factory();

    void update(float deltaTime);

    void handlePickingEvent(Camera *mainCamera, const InputEvent &inputEvent);

public:
    // Helper functions to enable automatic deinitialize
    // Tracking an object will delete it when autoDeinitialize is called
    // Untracking can be used if you delete it sooner than autoDeinitialize
    void trackClothActor(cloth::ClothRenderer *clothActor);

    void untrackClothActor(cloth::ClothRenderer *clothActor);

    void trackSolver(nv::cloth::Solver *solver);

    void untrackSolver(nv::cloth::Solver *solver);

    void trackFabric(nv::cloth::Fabric *fabric);

    void untrackFabric(nv::cloth::Fabric *fabric);

    // Help to detach cloths from solver at AutoDeinit.
    void addClothToSolver(cloth::ClothRenderer *clothActor, nv::cloth::Solver *solver);

    void addClothsToSolver(nv::cloth::Range<cloth::ClothRenderer *> clothActors, nv::cloth::Solver *solver);

private:
    void startSimulationStep(float dt);

    void waitForSimulationStep();

    void updateSimulationGraphics();

    void updateParticleDragging(const Ray3F &ray);

private:
    friend class ClothUI;

    nv::cloth::Factory *_factory{nullptr};
    std::vector<cloth::ClothRenderer *> _clothList;
    std::vector<nv::cloth::Solver *> _solverList;
    std::map<nv::cloth::Solver *, cloth::MultithreadedSolverHelper> _solverHelpers;
    std::vector<nv::cloth::Fabric *> _fabricList;
    std::map<cloth::ClothRenderer *, nv::cloth::Solver *> _clothSolverMap;

    cloth::JobManager _jobManager;

    // Particle dragging
    struct DraggingParticle {
        cloth::ClothRenderer *trackedCloth{nullptr};
        float dist = 0;
        float offset = 0;
        int particleIndex = 0;
    };
    DraggingParticle _draggingParticle;
};

}  // namespace cloth
}  // namespace vox