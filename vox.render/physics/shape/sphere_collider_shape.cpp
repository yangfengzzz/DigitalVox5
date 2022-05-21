//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sphere_collider_shape.h"

#include "../physics_manager.h"

#ifdef DEBUG
#include "material/unlit_material.h"
#include "mesh/mesh_renderer.h"
#include "mesh/wireframe_primitive_mesh.h"
#include "scene.h"
#endif

namespace vox::physics {
SphereColliderShape::SphereColliderShape() {
    native_geometry_ = std::make_shared<PxSphereGeometry>(radius_ * std::max(std::max(scale_.x, scale_.y), scale_.z));
    native_shape_ = PhysicsManager::native_physics_()->createShape(*native_geometry_, *native_material_, true);
    native_shape_->setQueryFilterData(PxFilterData(PhysicsManager::id_generator_++, 0, 0, 0));
    set_local_pose(pose_);
}

float SphereColliderShape::radius() const { return radius_; }

void SphereColliderShape::set_radius(float value) {
    radius_ = value;
    static_cast<PxSphereGeometry *>(native_geometry_.get())->radius =
            value * std::max(std::max(scale_.x, scale_.y), scale_.z);
    native_shape_->setGeometry(*native_geometry_);

#ifdef DEBUG
    sync_sphere_geometry();
#endif
}

void SphereColliderShape::set_world_scale(const Vector3F &scale) {
    ColliderShape::set_world_scale(scale);

    scale_ = scale;
    static_cast<PxSphereGeometry *>(native_geometry_.get())->radius =
            radius_ * std::max(std::max(scale_.x, scale_.y), scale_.z);
    native_shape_->setGeometry(*native_geometry_);

#ifdef DEBUG
    sync_sphere_geometry();
#endif
}

#ifdef DEBUG
void SphereColliderShape::set_entity(Entity *value) {
    ColliderShape::set_entity(value);

    auto renderer = entity_->AddComponent<MeshRenderer>();
    renderer->SetMaterial(std::make_shared<UnlitMaterial>(value->Scene()->Device()));
    renderer->SetMesh(WireframePrimitiveMesh::CreateSphereWireFrame(1));
    sync_sphere_geometry();
}

void SphereColliderShape::sync_sphere_geometry() {
    if (entity_) {
        auto radius = static_cast<PxSphereGeometry *>(native_geometry_.get())->radius;
        entity_->transform->SetScale(radius, radius, radius);
    }
}
#endif

}  // namespace vox::physics
