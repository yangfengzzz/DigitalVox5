//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "box_collider_shape.h"

#include "../physics_manager.h"

#ifdef DEBUG
#include "material/unlit_material.h"
#include "mesh/mesh_renderer.h"
#include "mesh/wireframe_primitive_mesh.h"
#include "scene.h"
#endif

namespace vox::physics {
BoxColliderShape::BoxColliderShape() : ColliderShape() {
    auto half_extent = half_ * scale_;
    native_geometry_ = std::make_shared<PxBoxGeometry>(half_extent.x, half_extent.y, half_extent.z);
    native_shape_ = PhysicsManager::native_physics_()->createShape(*native_geometry_, *native_material_, true);
    native_shape_->setQueryFilterData(PxFilterData(PhysicsManager::id_generator_++, 0, 0, 0));
    set_local_pose(pose_);
}

Vector3F BoxColliderShape::size() { return half_ * 2.f; }

void BoxColliderShape::set_size(const Vector3F &value) {
    half_ = value * 0.5f;
    auto half_extent = half_ * scale_;
    static_cast<PxBoxGeometry *>(native_geometry_.get())->halfExtents =
            PxVec3(half_extent.x, half_extent.y, half_extent.z);
    native_shape_->setGeometry(*native_geometry_);

#ifdef DEBUG
    sync_box_geometry();
#endif
}

void BoxColliderShape::set_world_scale(const Vector3F &scale) {
    ColliderShape::set_world_scale(scale);

    scale_ = scale;
    auto half_extent = half_ * scale_;
    static_cast<PxBoxGeometry *>(native_geometry_.get())->halfExtents =
            PxVec3(half_extent.x, half_extent.y, half_extent.z);
    native_shape_->setGeometry(*native_geometry_);

#ifdef DEBUG
    sync_box_geometry();
#endif
}

#ifdef DEBUG
void BoxColliderShape::set_entity(Entity *value) {
    ColliderShape::set_entity(value);

    auto renderer = entity_->AddComponent<MeshRenderer>();
    renderer->SetMaterial(std::make_shared<UnlitMaterial>(value->Scene()->Device()));
    renderer->SetMesh(WireframePrimitiveMesh::CreateCuboidWireFrame(1, 1, 1));
    sync_box_geometry();
}

void BoxColliderShape::sync_box_geometry() {
    if (entity_) {
        auto half_extents = static_cast<PxBoxGeometry *>(native_geometry_.get())->halfExtents;
        entity_->transform->SetScale(half_extents.x * 2, half_extents.y * 2, half_extents.z * 2);
    }
}
#endif

}  // namespace vox::physics
