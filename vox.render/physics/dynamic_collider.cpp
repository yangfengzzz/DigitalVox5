//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "dynamic_collider.h"
#include "physics_manager.h"

namespace vox::physics {
std::string DynamicCollider::name() {
    return "DynamicCollider";
}

DynamicCollider::DynamicCollider(Entity *entity) :
Collider(entity) {
    const auto &p = entity->transform_->world_position();
    auto q = entity->transform_->world_rotation_quaternion();
    q.normalize();
    
    native_actor_ = PhysicsManager::native_physics_()->createRigidDynamic(PxTransform(PxVec3(p.x, p.y, p.z),
                                                                                      PxQuat(q.x, q.y, q.z, q.w)));
}

float DynamicCollider::linear_damping() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getLinearDamping();
}

void DynamicCollider::set_linear_damping(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setLinearDamping(new_value);
}

float DynamicCollider::angular_damping() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getAngularDamping();
}

void DynamicCollider::set_angular_damping(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setAngularDamping(new_value);
}

Vector3F DynamicCollider::linear_velocity() {
    const auto &vel = static_cast<PxRigidDynamic *>(native_actor_)->getLinearVelocity();
    return {vel.x, vel.y, vel.z};
}

void DynamicCollider::set_linear_velocity(const Vector3F &new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setLinearVelocity(PxVec3(new_value.x, new_value.y, new_value.z));
}

Vector3F DynamicCollider::angular_velocity() {
    const auto &vel = static_cast<PxRigidDynamic *>(native_actor_)->getAngularVelocity();
    return {vel.x, vel.y, vel.z};
}

void DynamicCollider::set_angular_velocity(const Vector3F &new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setAngularVelocity(PxVec3(new_value.x, new_value.y, new_value.z));
}

float DynamicCollider::mass() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getMass();
}

void DynamicCollider::set_mass(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setMass(new_value);
}

Transform3F DynamicCollider::center_of_mass() {
    const auto &pose = static_cast<PxRigidDynamic *>(native_actor_)->getCMassLocalPose();
    Transform3F trans;
    trans.setTranslation(Vector3F(pose.p.x, pose.p.y, pose.p.z));
    trans.setOrientation(QuaternionF(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
    return trans;
}

void DynamicCollider::set_center_of_mass(const Transform3F &new_value) {
    const auto &p = new_value.translation();
    const auto &q = new_value.orientation();
    static_cast<PxRigidDynamic *>(native_actor_)->setCMassLocalPose(PxTransform(PxVec3(p.x, p.y, p.z),
                                                                                PxQuat(q.x, q.y, q.z, q.w)));
}

Vector3F DynamicCollider::inertia_tensor() {
    const auto &tensor = static_cast<PxRigidDynamic *>(native_actor_)->getMassSpaceInertiaTensor();
    return {tensor.x, tensor.y, tensor.z};
}

void DynamicCollider::set_inertia_tensor(const Vector3F &new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)
    ->setMassSpaceInertiaTensor(PxVec3(new_value.x, new_value.y, new_value.z));
}

float DynamicCollider::max_angular_velocity() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getMaxAngularVelocity();
}

void DynamicCollider::set_max_angular_velocity(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setMaxAngularVelocity(new_value);
}

float DynamicCollider::max_depenetration_velocity() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getMaxDepenetrationVelocity();
}

void DynamicCollider::set_max_depenetration_velocity(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setMaxDepenetrationVelocity(new_value);
}

float DynamicCollider::sleep_threshold() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getSleepThreshold();
}

void DynamicCollider::set_sleep_threshold(float new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setSleepThreshold(new_value);
}

uint32_t DynamicCollider::solver_iterations() {
    uint32_t pos_counts;
    uint32_t vel_counts;
    static_cast<PxRigidDynamic *>(native_actor_)->getSolverIterationCounts(pos_counts, vel_counts);
    return pos_counts;
}

void DynamicCollider::set_solver_iterations(uint32_t new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setSolverIterationCounts(new_value);
}

//MARK: - PxRigidBodyFlag
PxRigidBodyFlags DynamicCollider::rigid_body_flags() const {
    return static_cast<PxRigidDynamic *>(native_actor_)->getRigidBodyFlags();
}

void DynamicCollider::set_rigid_body_flag(PxRigidBodyFlag::Enum flag, bool value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidBodyFlag(flag, value);
}

void DynamicCollider::set_rigid_body_flags(const PxRigidBodyFlags &in_flags) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidBodyFlags(in_flags);
}

bool DynamicCollider::is_kinematic() {
    return static_cast<PxRigidDynamic *>(native_actor_)->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
}

void DynamicCollider::set_is_kinematic(bool new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, new_value);
}

//MARK: - RigidDynamicLockFlag
void DynamicCollider::set_rigid_dynamic_lock_flag(PxRigidDynamicLockFlag::Enum flag, bool value) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidDynamicLockFlag(flag, value);
}

void DynamicCollider::set_rigid_dynamic_lock_flags(const PxRigidDynamicLockFlags &flags) {
    static_cast<PxRigidDynamic *>(native_actor_)->setRigidDynamicLockFlags(flags);
}

PxRigidDynamicLockFlags DynamicCollider::rigid_dynamic_lock_flags() const {
    return static_cast<PxRigidDynamic *>(native_actor_)->getRigidDynamicLockFlags();
}

bool DynamicCollider::freeze_rotation() {
    const auto kXLock = static_cast<PxRigidDynamic *>(native_actor_)->getRigidDynamicLockFlags()
        .isSet(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X);
    const auto kYLock = static_cast<PxRigidDynamic *>(native_actor_)->getRigidDynamicLockFlags()
        .isSet(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y);
    const auto kZLock = static_cast<PxRigidDynamic *>(native_actor_)->getRigidDynamicLockFlags()
        .isSet(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);
    return kXLock && kYLock && kZLock;
}

void DynamicCollider::set_freeze_rotation(bool new_value) {
    static_cast<PxRigidDynamic *>(native_actor_)
    ->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, true);
    static_cast<PxRigidDynamic *>(native_actor_)
    ->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, true);
    static_cast<PxRigidDynamic *>(native_actor_)
    ->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, true);
}

void DynamicCollider::apply_force(const Vector3F &force) {
    static_cast<PxRigidDynamic *>(native_actor_)->addForce(PxVec3(force.x, force.y, force.z));
}

void DynamicCollider::apply_torque(const Vector3F &torque) {
    static_cast<PxRigidDynamic *>(native_actor_)->addTorque(PxVec3(torque.x, torque.y, torque.z));
}

void DynamicCollider::set_kinematic_target(const Transform3F &pose) {
    const auto &p = pose.translation();
    const auto &q = pose.orientation();
    static_cast<PxRigidDynamic *>(native_actor_)->setKinematicTarget(PxTransform(PxVec3(p.x, p.y, p.z),
                                                                                 PxQuat(q.x, q.y, q.z, q.w)));
}

void DynamicCollider::put_to_sleep() {
    static_cast<PxRigidDynamic *>(native_actor_)->putToSleep();
}

void DynamicCollider::wake_up() {
    static_cast<PxRigidDynamic *>(native_actor_)->wakeUp();
}

void DynamicCollider::on_late_update() {
    const auto &transform = entity()->transform_;
    
    PxTransform pose = native_actor_->getGlobalPose();
    transform->set_world_position(Point3F(pose.p.x, pose.p.y, pose.p.z));
    transform->set_world_rotation_quaternion(QuaternionF(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
    update_flag_->flag_ = false;
    
#ifdef DEBUG
    if (debug_entity_) {
        debug_entity_->transform_->set_position(Point3F(pose.p.x, pose.p.y, pose.p.z));
        debug_entity_->transform_->set_rotation_quaternion(QuaternionF(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
    }
#endif
}

//MARK: - Reflection
void DynamicCollider::on_serialize(nlohmann::json &data) {
    
}

void DynamicCollider::on_deserialize(const nlohmann::json &data) {
    
}

void DynamicCollider::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
