//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_DYNAMIC_COLLIDER_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_DYNAMIC_COLLIDER_H_

#include "collider.h"
#include "transform3.h"

namespace vox::physics {
class DynamicCollider : public Collider {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    explicit DynamicCollider(Entity *entity);
    
    /**
     * The linear damping of the dynamic collider.
     */
    float linear_damping();
    
    void set_linear_damping(float new_value);
    
    /**
     * The angular damping of the dynamic collider.
     */
    float angular_damping();
    
    void set_angular_damping(float new_value);
    
    /**
     * The linear velocity vector of the dynamic collider measured in world unit per second.
     */
    Vector3F linear_velocity();
    
    void set_linear_velocity(const Vector3F &new_value);
    
    /**
     * The angular velocity vector of the dynamic collider measured in radians per second.
     */
    Vector3F angular_velocity();
    
    void set_angular_velocity(const Vector3F &new_value);
    
    /**
     * The mass of the dynamic collider.
     */
    float mass();
    
    void set_mass(float new_value);
    
    /**
     * The center of mass relative to the transform's origin.
     */
    Transform3F center_of_mass();
    
    void set_center_of_mass(const Transform3F &new_value);
    
    /**
     * The diagonal inertia tensor of mass relative to the center of mass.
     */
    Vector3F inertia_tensor();
    
    void set_inertia_tensor(const Vector3F &new_value);
    
    /**
     * The maximum angular velocity of the collider measured in radians per second. (Default 7) range { 0, infinity }.
     */
    float max_angular_velocity();
    
    void set_max_angular_velocity(float new_value);
    
    /**
     * Maximum velocity of a collider when moving out of penetrating state.
     */
    float max_depenetration_velocity();
    
    void set_max_depenetration_velocity(float new_value);
    
    /**
     * The mass-normalized energy threshold, below which objects start going to sleep.
     */
    float sleep_threshold();
    
    void set_sleep_threshold(float new_value);
    
    /**
     * The solver_iterations determines how accurately collider joints and collision contacts are resolved.
     */
    uint32_t solver_iterations();
    
    void set_solver_iterations(uint32_t new_value);
    
public:
    /**
     * Controls whether physics affects the dynamic collider.
     */
    bool is_kinematic();
    
    void set_is_kinematic(bool new_value);
    
    [[nodiscard]] PxRigidBodyFlags rigid_body_flags() const;
    
    void set_rigid_body_flag(PxRigidBodyFlag::Enum flag, bool value);
    
    void set_rigid_body_flags(const PxRigidBodyFlags &in_flags);
    
public:
    /**
     * Controls whether physics will change the rotation of the object.
     */
    bool freeze_rotation();
    
    void set_freeze_rotation(bool new_value);
    
    /**
     * The particular rigid dynamic lock flag.
     */
    [[nodiscard]] PxRigidDynamicLockFlags rigid_dynamic_lock_flags() const;
    
    void set_rigid_dynamic_lock_flag(PxRigidDynamicLockFlag::Enum flag, bool value);
    
    void set_rigid_dynamic_lock_flags(const PxRigidDynamicLockFlags &flags);
    
public:
    /**
     * Apply a force to the DynamicCollider.
     * @param force - The force make the collider move
     */
    void apply_force(const Vector3F &force);
    
    /**
     * Apply a torque to the DynamicCollider.
     * @param torque - The force make the collider rotate
     */
    void apply_torque(const Vector3F &torque);
    
    /**
     * Moves kinematically controlled dynamic actors through the game world.
     * @param pose The desired pose for the kinematic actor
     */
    void set_kinematic_target(const Transform3F &pose);
    
    /**
     * Forces a collider to sleep at least one frame.
     */
    void put_to_sleep();
    
    /**
     * Forces a collider to wake up.
     */
    void wake_up();
    
public:
    /**
     * Called when the serialization is asked
     */
    void on_serialize(nlohmann::json &data) override;
    
    /**
     * Called when the deserialization is asked
     */
    void on_deserialize(const nlohmann::json &data) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void on_inspector(ui::WidgetContainer &p_root) override;
    
private:
    void on_late_update() override;
};

}

#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_DYNAMIC_COLLIDER_H_ */
