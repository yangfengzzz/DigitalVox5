//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "collider_shape.h"

#include "../physics_manager.h"

namespace vox::physics {
ColliderShape::ColliderShape() : native_material_(PhysicsManager::native_physics_()->createMaterial(0, 0, 0)) {}

Collider *ColliderShape::collider() { return collider_; }

void ColliderShape::set_local_pose(const Transform3F &pose) {
    pose_ = pose;

    const auto &p = pose.translation();
    const auto &q = pose.orientation();
    native_shape_->setLocalPose(PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
#ifdef DEBUG
    if (entity_) {
        entity_->transform->SetPosition(get_local_translation());
    }
#endif
}

Transform3F ColliderShape::local_pose() const { return pose_; }

void ColliderShape::set_position(const Vector3F &pos) {
    pose_.setTranslation(pos);
    set_local_pose(pose_);
}

Vector3F ColliderShape::position() const { return pose_.translation(); }

void ColliderShape::set_world_scale(const Vector3F &scale) {
    pose_.setTranslation(pose_.translation() * scale);
    set_local_pose(pose_);
}

void ColliderShape::set_material(PxMaterial *materials) {
    native_material_ = materials;

    std::vector<PxMaterial *> material_vec = {materials};
    native_shape_->setMaterials(material_vec.data(), 1);
}

PxMaterial *ColliderShape::material() { return native_material_; }

// MARK: - QueryFilterData
PxFilterData ColliderShape::query_filter_data() { return native_shape_->getQueryFilterData(); }

void ColliderShape::set_query_filter_data(const PxFilterData &data) { native_shape_->setQueryFilterData(data); }

uint32_t ColliderShape::unique_id() { return native_shape_->getQueryFilterData().word0; }

// MARK: - ShapeFlag
void ColliderShape::set_flag(PxShapeFlag::Enum flag, bool value) { native_shape_->setFlag(flag, value); }

void ColliderShape::set_flags(const PxShapeFlags &in_flags) { native_shape_->setFlags(in_flags); }

PxShapeFlags ColliderShape::get_flags() const { return native_shape_->getFlags(); }

bool ColliderShape::trigger() { return native_shape_->getFlags().isSet(PxShapeFlag::Enum::eTRIGGER_SHAPE); }

void ColliderShape::set_trigger(bool is_trigger) {
    native_shape_->setFlag(PxShapeFlag::Enum::eSIMULATION_SHAPE, !is_trigger);
    native_shape_->setFlag(PxShapeFlag::Enum::eTRIGGER_SHAPE, is_trigger);
}

bool ColliderShape::scene_query() { return native_shape_->getFlags().isSet(PxShapeFlag::Enum::eSCENE_QUERY_SHAPE); }

void ColliderShape::set_scene_query(bool is_query) {
    native_shape_->setFlag(PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, is_query);
}

#ifdef DEBUG
void ColliderShape::set_entity(Entity *value) {
    entity_ = value->CreateChild();
    entity_->transform->SetPosition(get_local_translation());
}

void ColliderShape::remove_entity(Entity *value) {
    value->RemoveChild(entity_);
    entity_ = nullptr;
}

Point3F ColliderShape::get_local_translation() {
    auto trans = native_shape_->getLocalPose();
    return {trans.p.x, trans.p.y, trans.p.z};
}
#endif

}  // namespace vox::physics
