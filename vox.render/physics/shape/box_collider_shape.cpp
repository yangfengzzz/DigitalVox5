//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "box_collider_shape.h"
#include "../physics_manager.h"

//#ifdef _DEBUG
//#include "mesh/mesh_renderer.h"
//#include "mesh/wireframe_primitive_mesh.h"
//#include "scene.h"
//#include "material/unlit_material.h"
//#endif

namespace vox::physics {
BoxColliderShape::BoxColliderShape() : ColliderShape() {
    auto half_extent = half_ * scale_;
    native_geometry_ = std::make_shared<PxBoxGeometry>(half_extent.x, half_extent.y, half_extent.z);
    native_shape_ = PhysicsManager::native_physics_()->createShape(*native_geometry_, *native_material_, true);
    native_shape_->setQueryFilterData(PxFilterData(PhysicsManager::id_generator_++, 0, 0, 0));
    set_local_pose(pose_);
}

Vector3F BoxColliderShape::size() {
    return half_ * 2.f;
}

void BoxColliderShape::set_size(const Vector3F &value) {
    half_ = value * 0.5f;
    auto half_extent = half_ * scale_;
    static_cast<PxBoxGeometry *>(native_geometry_.get())->halfExtents =
    PxVec3(half_extent.x, half_extent.y, half_extent.z);
    native_shape_->setGeometry(*native_geometry_);
    
    //#ifdef _DEBUG
    //    _syncBoxGeometry();
    //#endif
}

void BoxColliderShape::set_world_scale(const Vector3F &scale) {
    ColliderShape::set_world_scale(scale);
    
    scale_ = scale;
    auto half_extent = half_ * scale_;
    static_cast<PxBoxGeometry *>(native_geometry_.get())->halfExtents =
    PxVec3(half_extent.x, half_extent.y, half_extent.z);
    native_shape_->setGeometry(*native_geometry_);
    
    //#ifdef _DEBUG
    //    _syncBoxGeometry();
    //#endif
}

//#ifdef _DEBUG
//void BoxColliderShape::setEntity(Entity* value) {
//    ColliderShape::setEntity(value);
//    
//    auto renderer = _entity->addComponent<MeshRenderer>();
//    renderer->set_material(std::make_shared<UnlitMaterial>(value->scene()->device()));
//    renderer->set_mesh(WireframePrimitiveMesh::create_cuboid_wire_frame(value->scene()->device(), 1, 1, 1));
//    _syncBoxGeometry();
//}
//
//void BoxColliderShape::_syncBoxGeometry() {
//    if (_entity) {
//        auto halfExtents = static_cast<PxBoxGeometry *>(_nativeGeometry.get())->halfExtents;
//        _entity->transform->setScale(halfExtents.x * 2, halfExtents.y * 2, halfExtents.z * 2);
//    }
//}
//#endif

}
