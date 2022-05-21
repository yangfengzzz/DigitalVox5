//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "physics_manager.h"

#include <utility>

#include "../script.h"
#include "character_controller/character_controller.h"
#include "collider.h"
#include "shape/collider_shape.h"

namespace vox {
physics::PhysicsManager *physics::PhysicsManager::GetSingletonPtr() { return ms_singleton; }

physics::PhysicsManager &physics::PhysicsManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

namespace physics {
namespace {
class PxSimulationEventCallbackWrapper : public PxSimulationEventCallback {
public:
    std::function<void(PxShape *obj1, PxShape *obj2)> on_contact_enter_;
    std::function<void(PxShape *obj1, PxShape *obj2)> on_contact_exit_;
    std::function<void(PxShape *obj1, PxShape *obj2)> on_contact_stay_;

    std::function<void(PxShape *obj1, PxShape *obj2)> on_trigger_enter_;
    std::function<void(PxShape *obj1, PxShape *obj2)> on_trigger_exit_;
    std::function<void(PxShape *obj1, PxShape *obj2)> on_trigger_stay_;

    PxSimulationEventCallbackWrapper(std::function<void(PxShape *obj1, PxShape *obj2)> on_contact_enter,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> on_contact_exit,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> on_contact_stay,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> on_trigger_enter,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> on_trigger_exit,
                                     std::function<void(PxShape *obj1, PxShape *obj2)> on_trigger_stay)
        : on_contact_enter_(std::move(std::move(on_contact_enter))),
          on_contact_exit_(std::move(std::move(std::move(on_contact_exit)))),
          on_contact_stay_(std::move(on_contact_stay)),
          on_trigger_enter_(std::move(on_trigger_enter)),
          on_trigger_exit_(std::move(std::move(on_trigger_exit))),
          on_trigger_stay_(std::move(on_trigger_stay)) {}

    void onConstraintBreak(PxConstraintInfo *, PxU32) override {}

    void onWake(PxActor **, PxU32) override {}

    void onSleep(PxActor **, PxU32) override {}

    void onContact(const PxContactPairHeader &, const PxContactPair *pairs, PxU32 nb_pairs) override {
        for (PxU32 i = 0; i < nb_pairs; i++) {
            const PxContactPair &cp = pairs[i];

            if (cp.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD)) {
                on_contact_enter_(cp.shapes[0], cp.shapes[1]);
            } else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
                on_contact_exit_(cp.shapes[0], cp.shapes[1]);
            } else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) {
                on_contact_stay_(cp.shapes[0], cp.shapes[1]);
            }
        }
    }

    void onTrigger(PxTriggerPair *pairs, PxU32 count) override {
        for (PxU32 i = 0; i < count; i++) {
            const PxTriggerPair &tp = pairs[i];

            if (tp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                on_trigger_enter_(tp.triggerShape, tp.otherShape);
            } else if (tp.status & PxPairFlag::eNOTIFY_TOUCH_LOST) {
                on_trigger_exit_(tp.triggerShape, tp.otherShape);
            }
        }
    }

    void onAdvance(const PxRigidBody *const *, const PxTransform *, const PxU32) override {}
};
}  // namespace

uint32_t PhysicsManager::id_generator_ = 0;
Physics PhysicsManager::native_physics_ = Physics();

PhysicsManager::PhysicsManager() {
    on_contact_enter_ = [&](PxShape *obj1, PxShape *obj2) {};
    on_contact_exit_ = [&](PxShape *obj1, PxShape *obj2) {};
    on_contact_stay_ = [&](PxShape *obj1, PxShape *obj2) {};

    on_trigger_enter_ = [&](PxShape *obj1, PxShape *obj2) {
        const auto kShape1 = physical_objects_map_[obj1->getQueryFilterData().word0];
        const auto kShape2 = physical_objects_map_[obj2->getQueryFilterData().word0];

        auto scripts = kShape1->GetCollider()->GetEntity()->Scripts();
        for (const auto &script : scripts) {
            script->OnTriggerEnter(kShape2);
        }

        scripts = kShape2->GetCollider()->GetEntity()->Scripts();
        for (const auto &script : scripts) {
            script->OnTriggerEnter(kShape1);
        }
    };
    on_trigger_exit_ = [&](PxShape *obj1, PxShape *obj2) {
        const auto kShape1 = physical_objects_map_[obj1->getQueryFilterData().word0];
        const auto kShape2 = physical_objects_map_[obj2->getQueryFilterData().word0];

        auto scripts = kShape1->GetCollider()->GetEntity()->Scripts();
        for (const auto &script : scripts) {
            script->OnTriggerExit(kShape2);
        }

        scripts = kShape2->GetCollider()->GetEntity()->Scripts();
        for (const auto &script : scripts) {
            script->OnTriggerExit(kShape1);
        }
    };
    on_trigger_stay_ = [&](PxShape *obj1, PxShape *obj2) {};

    auto *simulation_event_callback =
            new PxSimulationEventCallbackWrapper(on_contact_enter_, on_contact_exit_, on_contact_stay_,
                                                 on_trigger_enter_, on_trigger_exit_, on_trigger_stay_);

    PxSceneDesc scene_desc(native_physics_()->getTolerancesScale());
    scene_desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    scene_desc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
    scene_desc.filterShader = PxDefaultSimulationFilterShader;
    scene_desc.simulationEventCallback = simulation_event_callback;
    scene_desc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
    scene_desc.staticKineFilteringMode = PxPairFilteringMode::eKEEP;
    scene_desc.flags |= PxSceneFlag::eENABLE_CCD;

    native_physics_manager_ = native_physics_()->createScene(scene_desc);
    native_character_controller_manager_ = PxCreateControllerManager(*native_physics_manager_);
}

Vector3F PhysicsManager::Gravity() const {
    auto g = native_physics_manager_->getGravity();
    return {g.x, g.y, g.z};
}

void PhysicsManager::SetGravity(const Vector3F &value) {
    native_physics_manager_->setGravity(PxVec3(value.x, value.y, value.z));
}

void PhysicsManager::Update(float delta_time) {
    auto simulate_time = delta_time + rest_time_;
    auto step = static_cast<uint32_t>(std::floor(std::min(max_sum_time_step_, simulate_time) / fixed_time_step_));
    rest_time_ = simulate_time - static_cast<float>(step) * fixed_time_step_;
    for (uint32_t i = 0; i < step; i++) {
        for (auto &script : on_physics_update_scripts_) {
            script->OnPhysicsUpdate();
        }
        CallColliderOnUpdate();
        native_physics_manager_->simulate(fixed_time_step_);
        native_physics_manager_->fetchResults(true);
        CallColliderOnLateUpdate();
        CallCharacterControllerOnLateUpdate();
    }
}

void PhysicsManager::CallColliderOnUpdate() {
    for (auto &collider : colliders_) {
        collider->OnUpdate();
    }
}

void PhysicsManager::CallColliderOnLateUpdate() {
    for (auto &collider : colliders_) {
        collider->OnLateUpdate();
    }
}

void PhysicsManager::CallCharacterControllerOnLateUpdate() {
    for (auto &controller : controllers_) {
        controller->OnLateUpdate();
    }
}

void PhysicsManager::AddColliderShape(const ColliderShapePtr &collider_shape) {
    physical_objects_map_[collider_shape->UniqueId()] = (collider_shape);
}

void PhysicsManager::RemoveColliderShape(const ColliderShapePtr &collider_shape) {
    physical_objects_map_.erase(collider_shape->UniqueId());
}

void PhysicsManager::AddCollider(Collider *collider) {
    colliders_.push_back(collider);
    native_physics_manager_->addActor(*collider->native_actor_);
}

void PhysicsManager::RemoveCollider(Collider *collider) {
    auto iter = std::find(colliders_.begin(), colliders_.end(), collider);
    if (iter != colliders_.end()) {
        colliders_.erase(iter);
    }

    native_physics_manager_->removeActor(*collider->native_actor_);
}

void PhysicsManager::AddCharacterController(CharacterController *character_controller) {
    controllers_.push_back(character_controller);
}

void PhysicsManager::RemoveCharacterController(CharacterController *character_controller) {
    auto iter = std::find(controllers_.begin(), controllers_.end(), character_controller);
    if (iter != controllers_.end()) {
        controllers_.erase(iter);
    }
}

void PhysicsManager::AddOnPhysicsUpdateScript(Script *script) { on_physics_update_scripts_.emplace_back(script); }

void PhysicsManager::RemoveOnPhysicsUpdateScript(Script *script) {
    auto iter = std::find(on_physics_update_scripts_.begin(), on_physics_update_scripts_.end(), script);
    if (iter != on_physics_update_scripts_.end()) {
        on_physics_update_scripts_.erase(iter);
    }
}

// MARK: - Raycast
bool PhysicsManager::Raycast(const Ray3F &ray) { return Raycast(ray, std::numeric_limits<float>::infinity(), nullptr); }

bool PhysicsManager::Raycast(const Ray3F &ray, HitResult &out_hit_result) {
    const auto kLayerMask = Layer::EVERYTHING;

    bool result = false;
    Raycast(ray, std::numeric_limits<float>::infinity(),
            [&](uint32_t idx, float distance, const Vector3F &normal, const Point3F &point) {
                if (physical_objects_map_[idx]->GetCollider()->GetEntity()->layer & kLayerMask) {
                    result = true;

                    out_hit_result.entity = physical_objects_map_[idx]->GetCollider()->GetEntity();
                    out_hit_result.distance = distance;
                    out_hit_result.normal = normal;
                    out_hit_result.point = point;
                }
            });

    if (!result) {
        out_hit_result.entity = nullptr;
        out_hit_result.distance = 0;
        out_hit_result.point = Point3F(0, 0, 0);
        out_hit_result.normal = Vector3F(0, 0, 0);
    }

    return result;
}

bool PhysicsManager::Raycast(const Ray3F &ray, float distance) { return Raycast(ray, distance, nullptr); }

bool PhysicsManager::Raycast(const Ray3F &ray, float distance, HitResult &out_hit_result) {
    const auto kLayerMask = Layer::EVERYTHING;

    bool result = false;
    Raycast(ray, distance, [&](uint32_t idx, float distance, const Vector3F &normal, const Point3F &point) {
        if (physical_objects_map_[idx]->GetCollider()->GetEntity()->layer & kLayerMask) {
            result = true;

            out_hit_result.entity = physical_objects_map_[idx]->GetCollider()->GetEntity();
            out_hit_result.distance = distance;
            out_hit_result.normal = normal;
            out_hit_result.point = point;
        }
    });

    if (!result) {
        out_hit_result.entity = nullptr;
        out_hit_result.distance = 0;
        out_hit_result.point = Point3F(0, 0, 0);
        out_hit_result.normal = Vector3F(0, 0, 0);
    }

    return result;
}

bool PhysicsManager::Raycast(const Ray3F &ray, float distance, Layer layer_mask) {
    bool result = false;
    Raycast(ray, distance, [&](uint32_t idx, float, const Vector3F &, const Point3F &) {
        if (physical_objects_map_[idx]->GetCollider()->GetEntity()->layer & layer_mask) {
            result = true;
        }
    });
    return result;
}

bool PhysicsManager::Raycast(const Ray3F &ray, float distance, Layer layer_mask, HitResult &out_hit_result) {
    bool result = false;
    Raycast(ray, distance, [&](uint32_t idx, float distance, const Vector3F &normal, const Point3F &point) {
        if (physical_objects_map_[idx]->GetCollider()->GetEntity()->layer & layer_mask) {
            result = true;

            out_hit_result.entity = physical_objects_map_[idx]->GetCollider()->GetEntity();
            out_hit_result.distance = distance;
            out_hit_result.normal = normal;
            out_hit_result.point = point;
        }
    });

    if (!result) {
        out_hit_result.entity = nullptr;
        out_hit_result.distance = 0;
        out_hit_result.point = Point3F(0, 0, 0);
        out_hit_result.normal = Vector3F(0, 0, 0);
    }

    return result;
}

bool PhysicsManager::Raycast(
        const Ray3F &ray,
        float distance,
        const std::function<void(uint32_t, float, const Vector3F &, const Point3F &)> &out_hit_result) {
    PxRaycastHit hit = PxRaycastHit();
    PxSceneQueryFilterData filter_data = PxSceneQueryFilterData();
    filter_data.flags = PxQueryFlags(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);

    const auto &origin = ray.origin;
    const auto &direction = ray.direction;
    bool result = PxSceneQueryExt::raycastSingle(*native_physics_manager_, PxVec3(origin.x, origin.y, origin.z),
                                                 PxVec3(direction.x, direction.y, direction.z), distance,
                                                 PxHitFlags(PxHitFlag::eDEFAULT), hit, filter_data);

    if (result && out_hit_result != nullptr) {
        out_hit_result(hit.shape->getQueryFilterData().word0, hit.distance,
                       Vector3F(hit.normal.x, hit.normal.y, hit.normal.z),
                       Point3F(hit.position.x, hit.position.y, hit.position.z));
    }

    return result;
}

}  // namespace physics
}  // namespace vox
