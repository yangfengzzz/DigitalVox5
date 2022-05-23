//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/joint/configurable_joint.h"

#include "vox.render/physics/collider.h"
#include "vox.render/physics/physics_manager.h"

namespace vox::physics {
ConfigurableJoint::ConfigurableJoint(Collider *collider_0, Collider *collider_1) : Joint() {
    auto actor_0 = collider_0 ? collider_0->Handle() : nullptr;
    auto actor_1 = collider_1 ? collider_1->Handle() : nullptr;
    native_joint_ =
            PxD6JointCreate(*PhysicsManager::native_physics_(), actor_0, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)),
                            actor_1, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

void ConfigurableJoint::SetMotion(PxD6Axis::Enum axis, PxD6Motion::Enum type) {
    static_cast<PxD6Joint *>(native_joint_)->setMotion(axis, type);
}

PxD6Motion::Enum ConfigurableJoint::Motion(PxD6Axis::Enum axis) const {
    return static_cast<PxD6Joint *>(native_joint_)->getMotion(axis);
}

float ConfigurableJoint::TwistAngle() const { return static_cast<PxD6Joint *>(native_joint_)->getTwistAngle(); }

float ConfigurableJoint::SwingYAngle() const { return static_cast<PxD6Joint *>(native_joint_)->getSwingYAngle(); }

float ConfigurableJoint::SwingZAngle() const { return static_cast<PxD6Joint *>(native_joint_)->getSwingZAngle(); }

void ConfigurableJoint::SetDistanceLimit(const PxJointLinearLimit &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setDistanceLimit(limit);
}

PxJointLinearLimit ConfigurableJoint::DistanceLimit() const {
    return static_cast<PxD6Joint *>(native_joint_)->getDistanceLimit();
}

void ConfigurableJoint::SetLinearLimit(PxD6Axis::Enum axis, const PxJointLinearLimitPair &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setLinearLimit(axis, limit);
}

PxJointLinearLimitPair ConfigurableJoint::LinearLimit(PxD6Axis::Enum axis) const {
    return static_cast<PxD6Joint *>(native_joint_)->getLinearLimit(axis);
}

void ConfigurableJoint::SetTwistLimit(const PxJointAngularLimitPair &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setTwistLimit(limit);
}

PxJointAngularLimitPair ConfigurableJoint::TwistLimit() const {
    return static_cast<PxD6Joint *>(native_joint_)->getTwistLimit();
}

void ConfigurableJoint::SetSwingLimit(const PxJointLimitCone &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setSwingLimit(limit);
}

PxJointLimitCone ConfigurableJoint::SwingLimit() const {
    return static_cast<PxD6Joint *>(native_joint_)->getSwingLimit();
}

void ConfigurableJoint::PyramidSwingLimit(const PxJointLimitPyramid &limit) {
    static_cast<PxD6Joint *>(native_joint_)->setPyramidSwingLimit(limit);
}

PxJointLimitPyramid ConfigurableJoint::PyramidSwingLimit() const {
    return static_cast<PxD6Joint *>(native_joint_)->getPyramidSwingLimit();
}

void ConfigurableJoint::SetDrive(PxD6Drive::Enum index, const PxD6JointDrive &drive) {
    static_cast<PxD6Joint *>(native_joint_)->setDrive(index, drive);
}

PxD6JointDrive ConfigurableJoint::Drive(PxD6Drive::Enum index) const {
    return static_cast<PxD6Joint *>(native_joint_)->getDrive(index);
}

void ConfigurableJoint::SetDrivePosition(const Transform3F &pose, bool autowake) {
    const auto &p = pose.translation();
    const auto &q = pose.orientation();

    static_cast<PxD6Joint *>(native_joint_)
            ->setDrivePosition(PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)), autowake);
}

Transform3F ConfigurableJoint::DrivePosition() const {
    const auto kPose = static_cast<PxD6Joint *>(native_joint_)->getDrivePosition();
    Transform3F trans;
    trans.setTranslation(Vector3F(kPose.p.x, kPose.p.y, kPose.p.z));
    trans.setOrientation(QuaternionF(kPose.q.x, kPose.q.y, kPose.q.z, kPose.q.w));
    return trans;
}

void ConfigurableJoint::SetDriveVelocity(const Vector3F &linear, const Vector3F &angular, bool autowake) {
    static_cast<PxD6Joint *>(native_joint_)
            ->setDriveVelocity(PxVec3(linear.x, linear.y, linear.z), PxVec3(angular.x, angular.y, angular.z), autowake);
}

void ConfigurableJoint::DriveVelocity(Vector3F &linear, Vector3F &angular) const {
    PxVec3 l, a;
    static_cast<PxD6Joint *>(native_joint_)->getDriveVelocity(l, a);
    linear = Vector3F(l.x, l.y, l.z);
    angular = Vector3F(a.x, a.y, a.z);
}

void ConfigurableJoint::SetProjectionLinearTolerance(float tolerance) {
    static_cast<PxD6Joint *>(native_joint_)->setProjectionLinearTolerance(tolerance);
}

float ConfigurableJoint::ProjectionLinearTolerance() const {
    return static_cast<PxD6Joint *>(native_joint_)->getProjectionLinearTolerance();
}

void ConfigurableJoint::SetProjectionAngularTolerance(float tolerance) {
    static_cast<PxD6Joint *>(native_joint_)->setProjectionAngularTolerance(tolerance);
}

float ConfigurableJoint::ProjectionAngularTolerance() const {
    return static_cast<PxD6Joint *>(native_joint_)->getProjectionAngularTolerance();
}

}  // namespace vox::physics
