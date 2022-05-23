//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/dynamic_collider.h"

#include "vox.render/physics/physics_manager.h"

namespace vox::physics {
std::string DynamicCollider::name() { return "DynamicCollider"; }

DynamicCollider::DynamicCollider(Entity *entity) : Collider(entity) {
    const auto &p = entity->transform->WorldPosition();
    auto q = entity->transform->WorldRotationQuaternion();
    q.normalize();

    native_actor_ = PhysicsManager::native_physics_()->createRigidDynamic(
            PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
}

float DynamicCollider::LinearDamping() { return static_cast<PxRigidDynamic *>(native_actor_)->getLinearDamping(); }

void DynamicCollider::SetLinearDamping(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setLinearDamping(new_value);
}

float DynamicCollider::AngularDamping() { return static_cast<PxRigidDynamic *>(native_actor_)->getAngularDamping(); }

void DynamicCollider::SetAngularDamping(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setAngularDamping(new_value);
}

Vector3F DynamicCollider::LinearVelocity() {
    const auto &vel = static_cast<PxRigidDynamic *>(native_actor_)->getLinearVelocity();
    return {vel.x, vel.y, vel.z};
}

void DynamicCollider::SetLinearVelocity(const Vector3F &new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setLinearVelocity(PxVec3(new_value.x, new_value.y, new_value.z));
}

Vector3F DynamicCollider::AngularVelocity() {
    const auto &vel = static_cast<PxRigidDynamic *>(native_actor_)->getAngularVelocity();
    return {vel.x, vel.y, vel.z};
}

void DynamicCollider::SetAngularVelocity(const Vector3F &new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setAngularVelocity(PxVec3(new_value.x, new_value.y, new_value.z));
}

float DynamicCollider::Mass() { return static_cast<PxRigidDynamic *>(native_actor_)->getMass(); }

void DynamicCollider::SetMass(float new_value) { static_cast<PxRigidDynamic *>(native_actor_)->setMass(new_value); }

Transform3F DynamicCollider::CenterOfMass() {
    const auto &pose = static_cast<PxRigidDynamic *>(native_actor_)->getCMassLocalPose();
    Transform3F trans;
    trans.setTranslation(Vector3F(pose.p.x, pose.p.y, pose.p.z));
    trans.setOrientation(QuaternionF(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
    return trans;
}

void DynamicCollider::SetCenterOfMass(const Transform3F &new_value) {
    const auto &p = new_value.translation();
    const auto &q = new_value.orientation();
    static_cast<PxRigidDynamic *>(native_actor_)
            ->setCMassLocalPose(PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
}

Vector3F DynamicCollider::InertiaTensor() {
    const auto &tensor = static_cast<PxRigidDynamic *>(native_actor_)->getMassSpaceInertiaTensor();
    return {tensor.x, tensor.y, tensor.z};
}

void DynamicCollider::SetInertiaTensor(const Vector3F &new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)
            ->setMassSpaceInertiaTensor(PxVec3(new_value.x, new_value.y, new_value.z));
}

float DynamicCollider::MaxAngularVelocity() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getMaxAngularVelocity();
}

void DynamicCollider::SetMaxAngularVelocity(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setMaxAngularVelocity(new_value);
}

float DynamicCollider::MaxDepenetrationVelocity() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getMaxDepenetrationVelocity();
}

void DynamicCollider::SetMaxDepenetrationVelocity(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setMaxDepenetrationVelocity(new_value);
}

float DynamicCollider::SleepThreshold() { return static_cast<PxRigidDynamic *>(native_actor_)->getSleepThreshold(); }

void DynamicCollider::SetSleepThreshold(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setSleepThreshold(new_value);
}

uint32_t DynamicCollider::SolverIterations() {
    uint32_t pos_counts;
    uint32_t vel_counts;
    static_cast<PxRigidDynamic *>(native_actor_)->getSolverIterationCounts(pos_counts, vel_counts);
    return pos_counts;
}

void DynamicCollider::SetSolverIterations(uint32_t new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setSolverIterationCounts(new_value);
}

// MARK: - PxRigidBodyFlag
PxRigidBodyFlags DynamicCollider::RigidBodyFlags() const {
    return static_cast<PxRigidDynamic *>(native_actor_)->getRigidBodyFlags();
}

void DynamicCollider::SetRigidBodyFlag(PxRigidBodyFlag::Enum flag, bool value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidBodyFlag(flag, value);
}

void DynamicCollider::SetRigidBodyFlags(const PxRigidBodyFlags &in_flags) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidBodyFlags(in_flags);
}

bool DynamicCollider::IsKinematic() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
}

void DynamicCollider::SetIsKinematic(bool new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, new_value);
}

// MARK: - RigidDynamicLockFlag
void DynamicCollider::SetRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum flag, bool value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidDynamicLockFlag(flag, value);
}

void DynamicCollider::SetRigidDynamicLockFlags(const PxRigidDynamicLockFlags &flags) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidDynamicLockFlags(flags);
}

PxRigidDynamicLockFlags DynamicCollider::RigidDynamicLockFlags() const {
    return static_cast<PxRigidDynamic *>(native_actor_)->getRigidDynamicLockFlags();
}

bool DynamicCollider::FreezeRotation() {
    const auto kXLock = static_cast<PxRigidDynamic *>(native_actor_)
                                ->getRigidDynamicLockFlags()
                                .isSet(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X);
    const auto kYLock = static_cast<PxRigidDynamic *>(native_actor_)
                                ->getRigidDynamicLockFlags()
                                .isSet(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y);
    const auto kZLock = static_cast<PxRigidDynamic *>(native_actor_)
                                ->getRigidDynamicLockFlags()
                                .isSet(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);
    return kXLock && kYLock && kZLock;
}

void DynamicCollider::SetFreezeRotation(bool new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)
            ->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, true);
    static_cast<PxRigidDynamic *>(native_actor_)
            ->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, true);
    static_cast<PxRigidDynamic *>(native_actor_)
            ->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, true);
}

void DynamicCollider::ApplyForce(const Vector3F &force) {
    static_cast<PxRigidDynamic *>(native_actor_)->addForce(PxVec3(force.x, force.y, force.z));
}

void DynamicCollider::ApplyTorque(const Vector3F &torque) {
    static_cast<PxRigidDynamic *>(native_actor_)->addTorque(PxVec3(torque.x, torque.y, torque.z));
}

void DynamicCollider::SetKinematicTarget(const Transform3F &pose) {
    const auto &p = pose.translation();
    const auto &q = pose.orientation();
    static_cast<PxRigidDynamic *>(native_actor_)
            ->setKinematicTarget(PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
}

void DynamicCollider::PutToSleep() { static_cast<PxRigidDynamic *>(native_actor_)->putToSleep(); }

void DynamicCollider::WakeUp() { static_cast<PxRigidDynamic *>(native_actor_)->wakeUp(); }

void DynamicCollider::OnLateUpdate() {
    const auto &transform = GetEntity()->transform;

    PxTransform pose = native_actor_->getGlobalPose();
    transform->SetWorldPosition(Point3F(pose.p.x, pose.p.y, pose.p.z));
    transform->SetWorldRotationQuaternion(QuaternionF(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
    update_flag_->flag_ = false;

#ifdef DEBUG
    if (debug_entity_) {
        debug_entity_->transform->SetPosition(Point3F(pose.p.x, pose.p.y, pose.p.z));
        debug_entity_->transform->SetRotationQuaternion(QuaternionF(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
    }
#endif
}

// MARK: - Reflection
void DynamicCollider::OnSerialize(nlohmann::json &data) {}

void DynamicCollider::OnDeserialize(const nlohmann::json &data) {}

void DynamicCollider::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox::physics
