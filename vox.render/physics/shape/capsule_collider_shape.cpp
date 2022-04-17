//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "capsule_collider_shape.h"
#include "../physics_manager.h"

#ifdef DEBUG
#include "mesh/wireframe_primitive_mesh.h"
#include "scene.h"
#include "material/unlit_material.h"
#endif

namespace vox::physics {
CapsuleColliderShape::CapsuleColliderShape() {
    native_geometry_ = std::make_shared<PxCapsuleGeometry>(radius_ * std::max(scale_.x, scale_.z), height_ * scale_.y);
    native_shape_ = PhysicsManager::native_physics_()->createShape(*native_geometry_, *native_material_, true);
    native_shape_->setQueryFilterData(PxFilterData(PhysicsManager::id_generator_++, 0, 0, 0));
    
    set_up_axis(ColliderShapeUpAxis::Enum::Y);
}

float CapsuleColliderShape::radius() const {
    return radius_;
}

void CapsuleColliderShape::set_radius(float value) {
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
    sync_capsule_geometry();
#endif
}

float CapsuleColliderShape::height() const {
    return height_ * 2.f;
}

void CapsuleColliderShape::set_height(float value) {
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
    sync_capsule_geometry();
#endif
}

ColliderShapeUpAxis::Enum CapsuleColliderShape::up_axis() {
    return up_axis_;
}

void CapsuleColliderShape::set_up_axis(ColliderShapeUpAxis::Enum value) {
    up_axis_ = value;
    switch (up_axis_) {
        case ColliderShapeUpAxis::Enum::X:pose_.setOrientation(QuaternionF(0, 0, 0, 1));
            break;
        case ColliderShapeUpAxis::Enum::Y:
            pose_.setOrientation(QuaternionF(0, 0, ColliderShape::half_sqrt_, ColliderShape::half_sqrt_));
            break;
        case ColliderShapeUpAxis::Enum::Z:
            pose_.setOrientation(QuaternionF(0, ColliderShape::half_sqrt_, 0, ColliderShape::half_sqrt_));
            break;
    }
    set_local_pose(pose_);
    
#ifdef DEBUG
    sync_capsule_axis(value);
#endif
}

void CapsuleColliderShape::set_world_scale(const Vector3F &scale) {
    ColliderShape::set_world_scale(scale);
    
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
    sync_capsule_geometry();
#endif
}

#ifdef DEBUG
void CapsuleColliderShape::set_entity(Entity *value) {
    ColliderShape::set_entity(value);
    
    renderer_ = entity_->add_component<MeshRenderer>();
    renderer_->set_material(std::make_shared<UnlitMaterial>(value->scene()->device()));
    sync_capsule_geometry();
}

void CapsuleColliderShape::sync_capsule_geometry() {
    if (entity_) {
        auto radius = static_cast<PxCapsuleGeometry *>(native_geometry_.get())->radius;
        auto half_height = static_cast<PxCapsuleGeometry *>(native_geometry_.get())->halfHeight;
        renderer_->set_mesh(WireframePrimitiveMesh::create_capsule_wire_frame(entity_->scene()->device(),
                                                                              radius,
                                                                              half_height * 2.0));
    }
}

void CapsuleColliderShape::sync_capsule_axis(ColliderShapeUpAxis::Enum up_axis) {
    if (entity_) {
        switch (up_axis) {
            case ColliderShapeUpAxis::Enum::X:
                entity_->transform_->set_rotation_quaternion(0, ColliderShape::half_sqrt_,
                                                             0, ColliderShape::half_sqrt_);
                break;
            case ColliderShapeUpAxis::Enum::Y:entity_->transform_->set_rotation_quaternion(0, 0, 0, 1);
                break;
            case ColliderShapeUpAxis::Enum::Z:
                entity_->transform_->set_rotation_quaternion(0, 0, ColliderShape::half_sqrt_,
                                                             ColliderShape::half_sqrt_);
                break;
        }
    }
}
#endif

}
