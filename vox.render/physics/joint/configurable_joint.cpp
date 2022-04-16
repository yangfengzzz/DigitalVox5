//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "configurable_joint.h"
#include "../physics_manager.h"
#include "../collider.h"

namespace vox::physics {
ConfigurableJoint::ConfigurableJoint(Collider *collider_0, Collider *collider_1) : Joint() {
    auto actor_0 = collider_0 ? collider_0->handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->handle() : nullptr;
    native_joint_ = PxD6JointCreate(*PhysicsManager::native_physics_(),
                                    actor_0, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)),
                                    actor_1, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

void ConfigurableJoint::set_motion(PxD6Axis::Enum axis, PxD6Motion::Enum type) {
    static_cast<PxD6Joint *>(native_joint_)->setMotion(axis, type);
}

PxD6Motion::Enum ConfigurableJoint::motion(PxD6Axis::Enum axis) const {
    return static_cast<PxD6Joint *>(native_joint_)->getMotion(axis);
}

float ConfigurableJoint::twist_angle() const {
    return static_cast<PxD6Joint *>(native_joint_)->getTwistAngle();
}

float ConfigurableJoint::swing_y_angle() const {
    return static_cast<PxD6Joint *>(native_joint_)->getSwingYAngle();
}

float ConfigurableJoint::swing_z_angle() const {
    return static_cast<PxD6Joint *>(native_joint_)->getSwingZAngle();
}

void ConfigurableJoint::set_distance_limit(const PxJointLinearLimit &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setDistanceLimit(limit);
}

PxJointLinearLimit ConfigurableJoint::distance_limit() const {
    return static_cast<PxD6Joint *>(native_joint_)->getDistanceLimit();
}

void ConfigurableJoint::set_linear_limit(PxD6Axis::Enum axis, const PxJointLinearLimitPair &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setLinearLimit(axis, limit);
}

PxJointLinearLimitPair ConfigurableJoint::linear_limit(PxD6Axis::Enum axis) const {
    return static_cast<PxD6Joint *>(native_joint_)->getLinearLimit(axis);
}

void ConfigurableJoint::set_twist_limit(const PxJointAngularLimitPair &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setTwistLimit(limit);
}

PxJointAngularLimitPair ConfigurableJoint::twist_limit() const {
    return static_cast<PxD6Joint *>(native_joint_)->getTwistLimit();
}

void ConfigurableJoint::set_swing_limit(const PxJointLimitCone &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setSwingLimit(limit);
}

PxJointLimitCone ConfigurableJoint::swing_limit() const {
    return static_cast<PxD6Joint *>(native_joint_)->getSwingLimit();
}

void ConfigurableJoint::pyramid_swing_limit(const PxJointLimitPyramid &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setPyramidSwingLimit(limit);
}

PxJointLimitPyramid ConfigurableJoint::pyramid_swing_limit() const {
    return static_cast<PxD6Joint *>(native_joint_)->getPyramidSwingLimit();
}

void ConfigurableJoint::set_drive(PxD6Drive::Enum index, const PxD6JointDrive &drive) {
    static_cast<PxD6Joint *>(native_joint_)->setDrive(index, drive);
}

PxD6JointDrive ConfigurableJoint::drive(PxD6Drive::Enum index) const {
    return static_cast<PxD6Joint *>(native_joint_)->getDrive(index);
}

void ConfigurableJoint::set_drive_position(const Transform3F &pose, bool autowake) {
    const auto &p = pose.translation();
    const auto &q = pose.orientation();
    
    static_cast<PxD6Joint *>(native_joint_)->setDrivePosition(PxTransform(PxVec3(p.x, p.y, p.z),
                                                                          PxQuat(q.x, q.y, q.z, q.w)), autowake);
}

Transform3F ConfigurableJoint::drive_position() const {
    const auto kPose = static_cast<PxD6Joint *>(native_joint_)->getDrivePosition();
    Transform3F trans;
    trans.setTranslation(Vector3F(kPose.p.x, kPose.p.y, kPose.p.z));
    trans.setOrientation(QuaternionF(kPose.q.x, kPose.q.y, kPose.q.z, kPose.q.w));
    return trans;
}

void ConfigurableJoint::set_drive_velocity(const Vector3F &linear, const Vector3F &angular, bool autowake) {
    static_cast<PxD6Joint *>(native_joint_)->setDriveVelocity(PxVec3(linear.x, linear.y, linear.z),
                                                              PxVec3(angular.x, angular.y, angular.z), autowake);
}

void ConfigurableJoint::drive_velocity(Vector3F &linear, Vector3F &angular) const {
    PxVec3 l, a;
    static_cast<PxD6Joint *>(native_joint_)->getDriveVelocity(l, a);
    linear = Vector3F(l.x, l.y, l.z);
    angular = Vector3F(a.x, a.y, a.z);
}

void ConfigurableJoint::set_projection_linear_tolerance(float tolerance) {
    static_cast<PxD6Joint *>(native_joint_)->setProjectionLinearTolerance(tolerance);
}

float ConfigurableJoint::projection_linear_tolerance() const {
    return static_cast<PxD6Joint *>(native_joint_)->getProjectionLinearTolerance();
}

void ConfigurableJoint::set_projection_angular_tolerance(float tolerance) {
    static_cast<PxD6Joint *>(native_joint_)->setProjectionAngularTolerance(tolerance);
}

float ConfigurableJoint::projection_angular_tolerance() const {
    return static_cast<PxD6Joint *>(native_joint_)->getProjectionAngularTolerance();
}

}
