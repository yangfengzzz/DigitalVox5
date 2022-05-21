//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "capsule_collider_shape.h"

#include "../physics_manager.h"

#ifdef DEBUG
#include "material/unlit_material.h"
#include "mesh/wireframe_primitive_mesh.h"
#include "scene.h"
#endif

namespace vox::physics {
CapsuleColliderShape::CapsuleColliderShape() {
    native_geometry_ = std::make_shared<PxCapsuleGeometry>(radius_ * std::max(scale_.x, scale_.z), height_ * scale_.y);
    native_shape_ = PhysicsManager::native_physics_()->createShape(*native_geometry_, *native_material_, true);
    native_shape_->setQueryFilterData(PxFilterData(PhysicsManager::id_generator_++, 0, 0, 0));

    SetUpAxis(ColliderShapeUpAxis::Enum::Y);
}

float CapsuleColliderShape::Radius() const { return radius_; }

void CapsuleColliderShape::SetRadius(float value) {
    radius_ = value;
    switch (up_axis_) {
        case ColliderShapeUpAxis::Enum::X:
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->radius = radius_ * std::max(scale_.y, scale_.z);
            break;
        case ColliderShapeUpAxis::Enum::Y:
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->radius = radius_ * std::max(scale_.x, scale_.z);
            break;
        case ColliderShapeUpAxis::Enum::Z:
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->radius = radius_ * std::max(scale_.x, scale_.y);
            break;
    }
    native_shape_->setGeometry(*native_geometry_);

#ifdef DEBUG
    SyncCapsuleGeometry();
#endif
}

float CapsuleColliderShape::Height() const { return height_ * 2.f; }

void CapsuleColliderShape::SetHeight(float value) {
    height_ = value * 0.5f;
    switch (up_axis_) {
        case ColliderShapeUpAxis::Enum::X:
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->halfHeight = height_ * scale_.x;
            break;
        case ColliderShapeUpAxis::Enum::Y:
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->halfHeight = height_ * scale_.y;
            break;
        case ColliderShapeUpAxis::Enum::Z:
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->halfHeight = height_ * scale_.z;
            break;
    }
    native_shape_->setGeometry(*native_geometry_);

#ifdef DEBUG
    SyncCapsuleGeometry();
#endif
}

ColliderShapeUpAxis::Enum CapsuleColliderShape::UpAxis() { return up_axis_; }

void CapsuleColliderShape::SetUpAxis(ColliderShapeUpAxis::Enum value) {
    up_axis_ = value;
    switch (up_axis_) {
        case ColliderShapeUpAxis::Enum::X:
            pose_.setOrientation(QuaternionF(0, 0, 0, 1));
            break;
        case ColliderShapeUpAxis::Enum::Y:
            pose_.setOrientation(QuaternionF(0, 0, ColliderShape::half_sqrt_, ColliderShape::half_sqrt_));
            break;
        case ColliderShapeUpAxis::Enum::Z:
            pose_.setOrientation(QuaternionF(0, ColliderShape::half_sqrt_, 0, ColliderShape::half_sqrt_));
            break;
    }
    SetLocalPose(pose_);

#ifdef DEBUG
    SyncCapsuleAxis(value);
#endif
}

void CapsuleColliderShape::SetWorldScale(const Vector3F &scale) {
    ColliderShape::SetWorldScale(scale);

    switch (up_axis_) {
        case ColliderShapeUpAxis::Enum::X:
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->radius = radius_ * std::max(scale.y, scale.z);
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->halfHeight = height_ * scale.x;
            break;
        case ColliderShapeUpAxis::Enum::Y:
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->radius = radius_ * std::max(scale.x, scale.z);
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->halfHeight = height_ * scale.y;
            break;
        case ColliderShapeUpAxis::Enum::Z:
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->radius = radius_ * std::max(scale.x, scale.y);
            static_cast<PxCapsuleGeometry *>(native_geometry_.get())->halfHeight = height_ * scale.z;
            break;
    }
    native_shape_->setGeometry(*native_geometry_);

#ifdef DEBUG
    SyncCapsuleGeometry();
#endif
}

#ifdef DEBUG
void CapsuleColliderShape::SetEntity(Entity *value) {
    ColliderShape::SetEntity(value);

    renderer_ = entity_->AddComponent<MeshRenderer>();
    renderer_->SetMaterial(std::make_shared<UnlitMaterial>(value->Scene()->Device()));
    SyncCapsuleGeometry();
}

void CapsuleColliderShape::SyncCapsuleGeometry() {
    if (entity_) {
        auto radius = static_cast<PxCapsuleGeometry *>(native_geometry_.get())->radius;
        auto half_height = static_cast<PxCapsuleGeometry *>(native_geometry_.get())->halfHeight;
        renderer_->SetMesh(WireframePrimitiveMesh::CreateCapsuleWireFrame(radius, half_height * 2.0));
    }
}

void CapsuleColliderShape::SyncCapsuleAxis(ColliderShapeUpAxis::Enum up_axis) {
    if (entity_) {
        switch (up_axis) {
            case ColliderShapeUpAxis::Enum::X:
                entity_->transform->SetRotationQuaternion(0, ColliderShape::half_sqrt_, 0, ColliderShape::half_sqrt_);
                break;
            case ColliderShapeUpAxis::Enum::Y:
                entity_->transform->SetRotationQuaternion(0, 0, 0, 1);
                break;
            case ColliderShapeUpAxis::Enum::Z:
                entity_->transform->SetRotationQuaternion(0, 0, ColliderShape::half_sqrt_, ColliderShape::half_sqrt_);
                break;
        }
    }
}
#endif

}  // namespace vox::physics
