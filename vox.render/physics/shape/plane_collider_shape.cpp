//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/shape/plane_collider_shape.h"

#include "vox.render/physics/physics_manager.h"

namespace vox::physics {
PlaneColliderShape::PlaneColliderShape() {
    native_geometry_ = std::make_shared<PxPlaneGeometry>();
    native_shape_ = PhysicsManager::native_physics_()->createShape(*native_geometry_, *native_material_, true);
    native_shape_->setQueryFilterData(PxFilterData(PhysicsManager::id_generator_++, 0, 0, 0));

    pose_.setOrientation(QuaternionF(0, 0, ColliderShape::half_sqrt_, ColliderShape::half_sqrt_));
    SetLocalPose(pose_);
}

Vector3F PlaneColliderShape::Rotation() { return pose_.orientation().toEuler(); }

void PlaneColliderShape::SetRotation(const Vector3F &value) {
    auto rotation = QuaternionF::makeRotationEuler(value.x, value.y, value.z);
    rotation.rotateZ(M_PI * 0.5);
    rotation.normalize();
    pose_.setOrientation(rotation);
    SetLocalPose(pose_);
}

}  // namespace vox::physics
