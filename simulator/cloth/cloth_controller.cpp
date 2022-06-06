//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simulator/cloth/cloth_controller.h"

#include "vox.cloth/NvClothExt/ClothFabricCooker.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"

namespace vox {

//-----------------------------------------------------------------------
cloth::ClothController *cloth::ClothController::GetSingletonPtr() { return ms_singleton; }

cloth::ClothController &cloth::ClothController::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

namespace cloth {
ClothController::ClothController() {
    NvClothEnvironment::AllocateEnv();
    factory_ = NvClothCreateFactoryCPU();
    if (!factory_) {
        // bug
    }
}

ClothController::~ClothController() {
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

nv::cloth::Factory *ClothController::Factory() { return factory_; }

void ClothController::Update(float delta_time) {
    StartSimulationStep(delta_time);
    WaitForSimulationStep();
    UpdateSimulationGraphics();
}

void ClothController::StartSimulationStep(float dt) {
    for (auto &solver_helper : solver_helpers_) {
        solver_helper.second.StartSimulation(dt);
    }
}

void ClothController::WaitForSimulationStep() {
    for (auto &solver_helper : solver_helpers_) {
        solver_helper.second.WaitForSimulation();
    }
}

void ClothController::UpdateSimulationGraphics() {
    for (auto actor : cloth_list_) {
        nv::cloth::MappedRange<physx::PxVec4> particles = actor->cloth->getCurrentParticles();
        std::vector<physx::PxVec3> particles3(particles.size());
        for (uint32_t i = 0; i < particles.size(); ++i) particles3[i] = particles[i].getXYZ();

        actor->Update(particles3.data(), particles.size());
    }
}

void ClothController::HandlePickingEvent(Camera *main_camera, const InputEvent &input_event) {
    if (input_event.GetSource() == EventSource::MOUSE) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
        if (mouse_button.GetAction() == MouseAction::DOWN) {
            dragging_particle_.dist = 9999999.0f;
            dragging_particle_.offset = 9999999.0f;
            dragging_particle_.tracked_cloth = nullptr;

            Ray3F ray = main_camera->ScreenPointToRay(Vector2F(mouse_button.GetPosX(), mouse_button.GetPosY()));
            for (auto it : cloth_list_) {
                nv::cloth::Cloth *cloth = it->cloth;
                Matrix4x4F modelMatrix = it->GetEntity()->transform->WorldMatrix();
                nv::cloth::Range<physx::PxVec4> particles = cloth->getCurrentParticles();

                for (int i = 0; i < (int)particles.size(); i++) {
                    physx::PxVec4 p = particles[i];
                    Point3F point(p.x, p.y, p.z);
                    point = modelMatrix * point;

                    float dist = ray.direction.dot(point - ray.origin);
                    float offset = point.distanceTo(ray.origin + ray.direction * dist);

                    if (offset < 0.1f) {
                        if (dragging_particle_.dist + 0.5f * dragging_particle_.offset > dist + 0.5f * offset) {
                            dragging_particle_.tracked_cloth = it;
                            // mDraggingParticle.mOffset = offset;
                            dragging_particle_.dist = dist;
                            dragging_particle_.particle_index = i;
                        }
                    }
                }
            }
        }

        if (mouse_button.GetAction() == MouseAction::MOVE) {
            Ray3F ray = main_camera->ScreenPointToRay(Vector2F(mouse_button.GetPosX(), mouse_button.GetPosY()));
            UpdateParticleDragging(ray);
        }

        if (mouse_button.GetAction() == MouseAction::UP) {
            dragging_particle_.tracked_cloth = nullptr;
        }
    }
}

void ClothController::UpdateParticleDragging(const Ray3F &ray) const {
    if (dragging_particle_.tracked_cloth) {
        nv::cloth::Cloth *cloth = dragging_particle_.tracked_cloth->cloth;
        Matrix4x4F modelMatrix = dragging_particle_.tracked_cloth->GetEntity()->transform->WorldMatrix();
        nv::cloth::Range<physx::PxVec4> particles = cloth->getCurrentParticles();
        nv::cloth::Range<physx::PxVec4> prevParticles = cloth->getPreviousParticles();

        physx::PxVec3 particleLocal = particles[dragging_particle_.particle_index].getXYZ();
        Point3F particleWorld = modelMatrix * Point3F(particleLocal.x, particleLocal.y, particleLocal.z);

        float rayT = dragging_particle_.dist;
        Point3F mousePointPlane = ray.origin + ray.direction * rayT;
        Vector3F offset = mousePointPlane - particleWorld;
        if (offset.lengthSquared() > 2.5f * 2.5f) offset = offset.normalized() * 2.5f;

        offset = modelMatrix.inverse() * offset;

        for (int i = 0; i < (int)particles.size(); i++) {
            physx::PxVec4 pLocal = particles[i];
            Vector4F p = modelMatrix * Vector4F(pLocal.x, pLocal.y, pLocal.z, pLocal.w);
            float dist = Point3F(p.x, p.y, p.z).distanceTo(particleWorld);

            // Only move dynamic points
            if (p.w > 0.0f) {
                const float softSelectionRadius = 0.4f;
                const float maxWeight = 0.4f;
                float weight = std::max(0.f, std::min(1.f, 1.f - (dist / softSelectionRadius))) * maxWeight;
                if (weight <= 0.0f) continue;
                Point3F point0(prevParticles[i].x, prevParticles[i].y, prevParticles[i].z);
                point0 = point0 - weight * offset;
                point0 = point0 * 0.99f + Vector3F(p.x, p.y, p.z) * 0.01f;
                // move previous particle in the opposite direction to avoid invalid configurations in the next solver
                // iteration.
                prevParticles[i] = physx::PxVec4(point0.x, point0.y, point0.z, prevParticles[i].w);
            }
        }
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

void ClothController::TrackClothActor(cloth::ClothRenderer *cloth_actor) { trackT(cloth_list_, cloth_actor); }

void ClothController::UntrackClothActor(cloth::ClothRenderer *cloth_actor) { untrackT(cloth_list_, cloth_actor); }

void ClothController::TrackSolver(nv::cloth::Solver *solver) {
    trackT(solver_list_, solver);
    solver_helpers_[solver].Initialize(solver, &job_manager_);
}

void ClothController::UntrackSolver(nv::cloth::Solver *solver) {
    untrackT(solver_list_, solver);
    solver_helpers_.erase(solver);
}

void ClothController::TrackFabric(nv::cloth::Fabric *fabric) { trackT(fabric_list_, fabric); }

void ClothController::UntrackFabric(nv::cloth::Fabric *fabric) { untrackT(fabric_list_, fabric); }

void ClothController::AddClothToSolver(cloth::ClothRenderer *cloth_actor, nv::cloth::Solver *solver) {
    solver->addCloth(cloth_actor->cloth);
    assert(cloth_solver_map_.find(cloth_actor) == cloth_solver_map_.end());
    cloth_solver_map_[cloth_actor] = solver;
}

void ClothController::AddClothsToSolver(nv::cloth::Range<cloth::ClothRenderer *> cloth_actors,
                                        nv::cloth::Solver *solver) {
    // A temporary vector of Cloth*, to construct a Range from and pass to solver
    std::vector<nv::cloth::Cloth *> cloths;

    for (auto clothActor : cloth_actors) {
        assert(cloth_solver_map_.find(clothActor) == cloth_solver_map_.end());
        cloth_solver_map_[clothActor] = solver;

        cloths.push_back(clothActor->cloth);
    }

    auto clothsRange = nv::cloth::Range<nv::cloth::Cloth *>(&cloths.front(), &cloths.back() + 1);
    solver->addCloths(clothsRange);
}

}  // namespace cloth
}  // namespace vox
