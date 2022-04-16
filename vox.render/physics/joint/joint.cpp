//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "joint.h"
#include "../collider.h"

namespace vox::physics {
void Joint::set_actors(Collider *actor_0, Collider *actor_1) {
    native_joint_->setActors(actor_0->handle(), actor_1->handle());
}

void Joint::set_local_pose(PxJointActorIndex::Enum actor, const Transform3F &local_pose) {
    const auto &p = local_pose.translation();
    const auto &q = local_pose.orientation();
    native_joint_->setLocalPose(actor, PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
}

Transform3F Joint::local_pose(PxJointActorIndex::Enum actor) const {
    const auto kPose = native_joint_->getLocalPose(actor);
    Transform3F trans;
    trans.setTranslation(Vector3F(kPose.p.x, kPose.p.y, kPose.p.z));
    trans.setOrientation(QuaternionF(kPose.q.x, kPose.q.y, kPose.q.z, kPose.q.w));
    return trans;
}

Transform3F Joint::relative_transform() const {
    const auto kPose = native_joint_->getRelativeTransform();
    Transform3F trans;
    trans.setTranslation(Vector3F(kPose.p.x, kPose.p.y, kPose.p.z));
    trans.setOrientation(QuaternionF(kPose.q.x, kPose.q.y, kPose.q.z, kPose.q.w));
    return trans;
}

Vector3F Joint::relative_linear_velocity() const {
    const auto kVel = native_joint_->getRelativeLinearVelocity();
    return {kVel.x, kVel.y, kVel.z};
}

Vector3F Joint::relative_angular_velocity() const {
    const auto kVel = native_joint_->getRelativeAngularVelocity();
    return {kVel.x, kVel.y, kVel.z};
}

void Joint::set_break_force(float force, float torque) {
    native_joint_->setBreakForce(force, torque);
}

void Joint::get_break_force(float &force, float &torque) const {
    native_joint_->getBreakForce(force, torque);
}

void Joint::set_constraint_flags(const PxConstraintFlags &flags) {
    native_joint_->setConstraintFlags(flags);
}

void Joint::set_constraint_flag(PxConstraintFlag::Enum flag, bool value) {
    native_joint_->setConstraintFlag(flag, value);
}

PxConstraintFlags Joint::constraint_flags() const {
    return native_joint_->getConstraintFlags();
}

void Joint::set_inv_mass_scale_0(float inv_mass_scale) {
    native_joint_->setInvMassScale0(inv_mass_scale);
}

float Joint::inv_mass_scale_0() const {
    return native_joint_->getInvMassScale0();
}

void Joint::set_inv_inertia_scale_0(float inv_inertia_scale) {
    native_joint_->setInvInertiaScale0(inv_inertia_scale);
}

float Joint::inv_inertia_scale_0() const {
    return native_joint_->getInvInertiaScale0();
}

void Joint::set_inv_mass_scale_1(float inv_mass_scale) {
    native_joint_->setInvMassScale1(inv_mass_scale);
}

float Joint::inv_mass_scale_1() const {
    return native_joint_->getInvMassScale1();
}

void Joint::set_inv_inertia_scale_1(float inv_inertia_scale) {
    native_joint_->setInvInertiaScale1(inv_inertia_scale);
}

float Joint::inv_inertia_scale_1() const {
    return native_joint_->getInvInertiaScale1();
}

}
