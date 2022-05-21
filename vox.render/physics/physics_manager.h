//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_PHYSICS_MANAGER_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_PHYSICS_MANAGER_H_

#include <unordered_map>
#include <vector>

#include "hit_result.h"
#include "layer.h"
#include "physics.h"
#include "ray3.h"
#include "singleton.h"

namespace vox {
namespace physics {
/**
 * A physics manager is a collection of bodies and constraints which can interact.
 */
class PhysicsManager : public Singleton<PhysicsManager> {
public:
    static PhysicsManager &GetSingleton();

    static PhysicsManager *GetSingletonPtr();

    static uint32_t id_generator_;
    static Physics native_physics_;

    /** The fixed time step in seconds at which physics are performed. */
    static constexpr float fixed_time_step_ = 1.f / 60;

    /** The max sum of time step in seconds one frame. */
    static constexpr float max_sum_time_step_ = 1.f / 3;

    PhysicsManager();

    [[nodiscard]] Vector3F gravity() const;

    void set_gravity(const Vector3F &value);

public:
    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray);

    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param out_hit_result - If true is returned, outHitResult will contain more detailed collision information
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray, HitResult &out_hit_result);

    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param distance - The max distance the ray should check
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray, float distance);

    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param distance - The max distance the ray should check
     * @param out_hit_result - If true is returned, outHitResult will contain more detailed collision information
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray, float distance, HitResult &out_hit_result);

    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param distance - The max distance the ray should check
     * @param layer_mask - Layer mask that is used to selectively ignore Colliders when casting
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray, float distance, Layer layer_mask);

    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param distance - The max distance the ray should check
     * @param layer_mask - Layer mask that is used to selectively ignore Colliders when casting
     * @param out_hit_result - If true is returned, outHitResult will contain more detailed collision information
     * @returns Returns True if the ray intersects with a collider, otherwise false.
     */
    bool raycast(const Ray3F &ray, float distance, Layer layer_mask, HitResult &out_hit_result);

public:
    /**
     * Call on every frame to update pose of objects.
     */
    void update(float delta_time);

    void call_collider_on_update();

    void call_collider_on_late_update();

    void call_character_controller_on_late_update();

    void add_on_physics_update_script(Script *script);

    void remove_on_physics_update_script(Script *script);

private:
    friend class Collider;

    friend class CharacterController;

    friend class BoxCharacterController;

    friend class CapsuleCharacterController;

    /**
     * Add ColliderShape into the manager.
     * @param collider_shape - The Collider Shape.
     */
    void add_collider_shape(const ColliderShapePtr &collider_shape);

    /**
     * Remove ColliderShape.
     * @param collider_shape - The Collider Shape.
     */
    void remove_collider_shape(const ColliderShapePtr &collider_shape);

    /**
     * Add collider into the manager.
     * @param collider - StaticCollider or DynamicCollider.
     */
    void add_collider(Collider *collider);

    /**
     * Remove collider.
     * @param collider - StaticCollider or DynamicCollider.
     */
    void remove_collider(Collider *collider);

    /**
     * Add CharacterController into the manager.
     * @param character_controller The Character Controller.
     */
    void add_character_controller(CharacterController *character_controller);

    /**
     * Remove CharacterController.
     * @param character_controller The Character Controller.
     */
    void remove_character_controller(CharacterController *character_controller);

    bool raycast(const Ray3F &ray,
                 float distance,
                 const std::function<void(uint32_t, float, const Vector3F &, const Point3F &)> &out_hit_result);

private:
    PxControllerManager *native_character_controller_manager_;
    PxScene *native_physics_manager_;

    std::unordered_map<uint32_t, ColliderShapePtr> physical_objects_map_;
    std::vector<Collider *> colliders_;
    std::vector<CharacterController *> controllers_;
    std::vector<Script *> on_physics_update_scripts_;
    float rest_time_ = 0;

    std::function<void(PxShape *obj1, PxShape *obj2)> on_contact_enter_;
    std::function<void(PxShape *obj1, PxShape *obj2)> on_contact_exit_;
    std::function<void(PxShape *obj1, PxShape *obj2)> on_contact_stay_;

    std::function<void(PxShape *obj1, PxShape *obj2)> on_trigger_enter_;
    std::function<void(PxShape *obj1, PxShape *obj2)> on_trigger_exit_;
    std::function<void(PxShape *obj1, PxShape *obj2)> on_trigger_stay_;
};

}  // namespace physics
template <>
inline physics::PhysicsManager *Singleton<physics::PhysicsManager>::ms_singleton{nullptr};
}  // namespace vox
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_PHYSICS_MANAGER_H_ */
