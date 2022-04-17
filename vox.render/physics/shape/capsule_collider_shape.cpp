//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "capsule_collider_shape.h"
#include "../physics_manager.h"

//#ifdef _DEBUG
//#include "mesh/wireframe_primitive_mesh.h"
//#include "scene.h"
//#include "material/unlit_material.h"
//#endif

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
    
    //#ifdef _DEBUG
    //    _syncCapsuleGeometry();
    //#endif
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
    
    //#ifdef _DEBUG
    //    _syncCapsuleGeometry();
    //#endif
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
    
    //#ifdef _DEBUG
    //    _syncCapsuleAxis(value);
    //#endif
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
    
    //#ifdef _DEBUG
    //    _syncCapsuleGeometry();
    //#endif
}

//#ifdef _DEBUG
//void CapsuleColliderShape::setEntity(Entity* value) {
//    ColliderShape::setEntity(value);
//
//    _renderer = _entity->addComponent<MeshRenderer>();
//    _renderer->set_material(std::make_shared<UnlitMaterial>(value->scene()->device()));
//    _syncCapsuleGeometry();
//}
//
//void CapsuleColliderShape::_syncCapsuleGeometry() {
//    if (_entity) {
//        auto radius = static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->radius;
//        auto halfHeight = static_cast<PxCapsuleGeometry *>(_nativeGeometry.get())->halfHeight;
//        _renderer->set_mesh(WireframePrimitiveMesh::create_capsule_wire_frame(_entity->scene()->device(), radius, halfHeight * 2.0));
//    }
//}
//
//void CapsuleColliderShape::_syncCapsuleAxis(ColliderShapeUpAxis::Enum up_axis) {
//    if (_entity) {
//        switch (up_axis) {
//            case ColliderShapeUpAxis::Enum::X:
//                _entity->transform->setRotationQuaternion(0, ColliderShape::halfSqrt,
//                                                          0, ColliderShape::halfSqrt);
//                break;
//            case ColliderShapeUpAxis::Enum::Y:
//                _entity->transform->setRotationQuaternion(0, 0, 0, 1);
//                break;
//            case ColliderShapeUpAxis::Enum::Z:
//                _entity->transform->setRotationQuaternion(0, 0, ColliderShape::halfSqrt,
//                                                          ColliderShape::halfSqrt);
//                break;
//        }
//    }
//}
//#endif

}
