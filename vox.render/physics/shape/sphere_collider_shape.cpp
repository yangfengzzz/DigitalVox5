//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/shape/sphere_collider_shape.h"

#include "vox.render/physics/physics_manager.h"

#ifdef DEBUG
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/wireframe/wireframe_primitive_mesh.h"
#include "vox.render/scene.h"
#endif

namespace vox::physics {
SphereColliderShape::SphereColliderShape() {
    native_geometry_ = std::make_shared<PxSphereGeometry>(radius_ * std::max(std::max(scale_.x, scale_.y), scale_.z));
    native_shape_ = PhysicsManager::native_physics_()->createShape(*native_geometry_, *native_material_, true);
    native_shape_->setQueryFilterData(PxFilterData(PhysicsManager::id_generator_++, 0, 0, 0));
    SetLocalPose(pose_);
}

float SphereColliderShape::Radius() const { return radius_; }

void SphereColliderShape::SetRadius(float value) {
    radius_ = value;
    static_cast<PxSphereGeometry *>(native_geometry_.get())->radius =
            value * std::max(std::max(scale_.x, scale_.y), scale_.z);
    native_shape_->setGeometry(*native_geometry_);

#ifdef DEBUG
    SyncSphereGeometry();
#endif
}

void SphereColliderShape::SetWorldScale(const Vector3F &scale) {
    ColliderShape::SetWorldScale(scale);

    scale_ = scale;
    static_cast<PxSphereGeometry *>(native_geometry_.get())->radius =
            radius_ * std::max(std::max(scale_.x, scale_.y), scale_.z);
    native_shape_->setGeometry(*native_geometry_);

#ifdef DEBUG
    SyncSphereGeometry();
#endif
}

#ifdef DEBUG
void SphereColliderShape::SetEntity(Entity *value) {
    ColliderShape::SetEntity(value);

    auto renderer = entity_->AddComponent<MeshRenderer>();
    renderer->SetMaterial(std::make_shared<UnlitMaterial>(value->Scene()->Device()));
    renderer->SetMesh(WireframePrimitiveMesh::CreateSphereWireFrame(1));
    SyncSphereGeometry();
}

void SphereColliderShape::SyncSphereGeometry() {
    if (entity_) {
        auto radius = static_cast<PxSphereGeometry *>(native_geometry_.get())->radius;
        entity_->transform->SetScale(radius, radius, radius);
    }
}
#endif

}  // namespace vox::physics
