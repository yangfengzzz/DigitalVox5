//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

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
    float LinearDamping();

    void SetLinearDamping(float new_value);

    /**
     * The angular damping of the dynamic collider.
     */
    float AngularDamping();

    void SetAngularDamping(float new_value);

    /**
     * The linear velocity vector of the dynamic collider measured in world unit per second.
     */
    Vector3F LinearVelocity();

    void SetLinearVelocity(const Vector3F &new_value);

    /**
     * The angular velocity vector of the dynamic collider measured in radians per second.
     */
    Vector3F AngularVelocity();

    void SetAngularVelocity(const Vector3F &new_value);

    /**
     * The mass of the dynamic collider.
     */
    float Mass();

    void SetMass(float new_value);

    /**
     * The center of mass relative to the transform's origin.
     */
    Transform3F CenterOfMass();

    void SetCenterOfMass(const Transform3F &new_value);

    /**
     * The diagonal inertia tensor of mass relative to the center of mass.
     */
    Vector3F InertiaTensor();

    void SetInertiaTensor(const Vector3F &new_value);

    /**
     * The maximum angular velocity of the collider measured in radians per second. (Default 7) range { 0, infinity }.
     */
    float MaxAngularVelocity();

    void SetMaxAngularVelocity(float new_value);

    /**
     * Maximum velocity of a collider when moving out of penetrating state.
     */
    float MaxDepenetrationVelocity();

    void SetMaxDepenetrationVelocity(float new_value);

    /**
     * The mass-normalized energy threshold, below which objects start going to sleep.
     */
    float SleepThreshold();

    void SetSleepThreshold(float new_value);

    /**
     * The SolverIterations determines how accurately collider joints and collision contacts are resolved.
     */
    uint32_t SolverIterations();

    void SetSolverIterations(uint32_t new_value);

public:
    /**
     * Controls whether physics affects the dynamic collider.
     */
    bool IsKinematic();

    void SetIsKinematic(bool new_value);

    [[nodiscard]] PxRigidBodyFlags RigidBodyFlags() const;

    void SetRigidBodyFlag(PxRigidBodyFlag::Enum flag, bool value);

    void SetRigidBodyFlags(const PxRigidBodyFlags &in_flags);

public:
    /**
     * Controls whether physics will change the rotation of the object.
     */
    bool FreezeRotation();

    void SetFreezeRotation(bool new_value);

    /**
     * The particular rigid dynamic lock flag.
     */
    [[nodiscard]] PxRigidDynamicLockFlags RigidDynamicLockFlags() const;

    void SetRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum flag, bool value);

    void SetRigidDynamicLockFlags(const PxRigidDynamicLockFlags &flags);

public:
    /**
     * Apply a force to the DynamicCollider.
     * @param force - The force make the collider move
     */
    void ApplyForce(const Vector3F &force);

    /**
     * Apply a torque to the DynamicCollider.
     * @param torque - The force make the collider rotate
     */
    void ApplyTorque(const Vector3F &torque);

    /**
     * Moves kinematically controlled dynamic actors through the game world.
     * @param pose The desired pose for the kinematic actor
     */
    void SetKinematicTarget(const Transform3F &pose);

    /**
     * Forces a collider to sleep at least one frame.
     */
    void PutToSleep();

    /**
     * Forces a collider to wake up.
     */
    void WakeUp();

public:
    /**
     * Called when the serialization is asked
     */
    void OnSerialize(nlohmann::json &data) override;

    /**
     * Called when the deserialization is asked
     */
    void OnDeserialize(const nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void OnInspector(ui::WidgetContainer &p_root) override;

private:
    void OnLateUpdate() override;
};

}  // namespace vox::physics