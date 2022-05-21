//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "collider_shape.h"

#include "../physics_manager.h"

namespace vox::physics {
ColliderShape::ColliderShape() : native_material_(PhysicsManager::native_physics_()->createMaterial(0, 0, 0)) {}

Collider *ColliderShape::GetCollider() { return collider_; }

void ColliderShape::SetLocalPose(const Transform3F &pose) {
    pose_ = pose;

    const auto &p = pose.translation();
    const auto &q = pose.orientation();
    native_shape_->setLocalPose(PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
#ifdef DEBUG
    if (entity_) {
        entity_->transform->SetPosition(GetLocalTranslation());
    }
#endif
}

Transform3F ColliderShape::LocalPose() const { return pose_; }

void ColliderShape::SetPosition(const Vector3F &pos) {
    pose_.setTranslation(pos);
    SetLocalPose(pose_);
}

Vector3F ColliderShape::Position() const { return pose_.translation(); }

void ColliderShape::SetWorldScale(const Vector3F &scale) {
    pose_.setTranslation(pose_.translation() * scale);
    SetLocalPose(pose_);
}

void ColliderShape::SetMaterial(PxMaterial *materials) {
    native_material_ = materials;

    std::vector<PxMaterial *> material_vec = {materials};
    native_shape_->setMaterials(material_vec.data(), 1);
}

PxMaterial *ColliderShape::Material() { return native_material_; }

// MARK: - QueryFilterData
PxFilterData ColliderShape::QueryFilterData() { return native_shape_->getQueryFilterData(); }

void ColliderShape::SetQueryFilterData(const PxFilterData &data) { native_shape_->setQueryFilterData(data); }

uint32_t ColliderShape::UniqueId() { return native_shape_->getQueryFilterData().word0; }

// MARK: - ShapeFlag
void ColliderShape::SetFlag(PxShapeFlag::Enum flag, bool value) { native_shape_->setFlag(flag, value); }

void ColliderShape::SetFlags(const PxShapeFlags &in_flags) { native_shape_->setFlags(in_flags); }

PxShapeFlags ColliderShape::GetFlags() const { return native_shape_->getFlags(); }

bool ColliderShape::Trigger() { return native_shape_->getFlags().isSet(PxShapeFlag::Enum::eTRIGGER_SHAPE); }

void ColliderShape::SetTrigger(bool is_trigger) {
    native_shape_->setFlag(PxShapeFlag::Enum::eSIMULATION_SHAPE, !is_trigger);
    native_shape_->setFlag(PxShapeFlag::Enum::eTRIGGER_SHAPE, is_trigger);
}

bool ColliderShape::SceneQuery() { return native_shape_->getFlags().isSet(PxShapeFlag::Enum::eSCENE_QUERY_SHAPE); }

void ColliderShape::SetSceneQuery(bool is_query) {
    native_shape_->setFlag(PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, is_query);
}

#ifdef DEBUG
void ColliderShape::SetEntity(Entity *value) {
    entity_ = value->CreateChild();
    entity_->transform->SetPosition(GetLocalTranslation());
}

void ColliderShape::RemoveEntity(Entity *value) {
    value->RemoveChild(entity_);
    entity_ = nullptr;
}

Point3F ColliderShape::GetLocalTranslation() {
    auto trans = native_shape_->getLocalPose();
    return {trans.p.x, trans.p.y, trans.p.z};
}
#endif

}  // namespace vox::physics
