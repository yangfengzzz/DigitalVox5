//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sphere_collider_shape.h"
#include "../physics_manager.h"

//#ifdef _DEBUG
//#include "mesh/mesh_renderer.h"
//#include "mesh/wireframe_primitive_mesh.h"
//#include "scene.h"
//#include "material/unlit_material.h"
//#endif

namespace vox::physics {
SphereColliderShape::SphereColliderShape() {
    native_geometry_ = std::make_shared<PxSphereGeometry>(radius_ * std::max(std::max(scale_.x, scale_.y), scale_.z));
    native_shape_ = PhysicsManager::native_physics_()->createShape(*native_geometry_, *native_material_, true);
    native_shape_->setQueryFilterData(PxFilterData(PhysicsManager::id_generator_++, 0, 0, 0));
    set_local_pose(pose_);
}

float SphereColliderShape::radius() const {
    return radius_;
}

void SphereColliderShape::set_radius(float value) {
    radius_ = value;
    static_cast<PxSphereGeometry *>(native_geometry_.get())->radius =
    value * std::max(std::max(scale_.x, scale_.y), scale_.z);
    native_shape_->setGeometry(*native_geometry_);
    
    //#ifdef _DEBUG
    //    _syncSphereGeometry();
    //#endif
}

void SphereColliderShape::set_world_scale(const Vector3F &scale) {
    ColliderShape::set_world_scale(scale);
    
    scale_ = scale;
    static_cast<PxSphereGeometry *>(native_geometry_.get())->radius =
    radius_ * std::max(std::max(scale_.x, scale_.y), scale_.z);
    native_shape_->setGeometry(*native_geometry_);
    
    //#ifdef _DEBUG
    //    _syncSphereGeometry();
    //#endif
}

//#ifdef _DEBUG
//void SphereColliderShape::setEntity(Entity* value) {
//    ColliderShape::setEntity(value);
//    
//    auto renderer = _entity->addComponent<MeshRenderer>();
//    renderer->set_material(std::make_shared<UnlitMaterial>(value->scene()->device()));
//    renderer->setMesh(WireframePrimitiveMesh::create_sphere_wire_frame(value->scene()->device(), 1));
//    _syncSphereGeometry();
//}
//
//void SphereColliderShape::_syncSphereGeometry() {
//    if (_entity) {
//        auto radius = static_cast<PxSphereGeometry *>(_nativeGeometry.get())->radius;
//        _entity->transform->setScale(radius, radius, radius);
//    }
//}
//#endif

}
