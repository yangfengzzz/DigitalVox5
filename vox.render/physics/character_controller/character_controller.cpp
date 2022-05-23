//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/character_controller/character_controller.h"

#include "vox.render/physics/physics_manager.h"
#include "vox.render/entity.h"
#include "vox.render/scene.h"

namespace vox::physics {
CharacterController::CharacterController(Entity *entity) : Component(entity) {}

PxControllerCollisionFlags CharacterController::Move(const Vector3F &disp, float min_dist, float elapsed_time) {
    return native_controller_->move(PxVec3(disp.x, disp.y, disp.z), min_dist, elapsed_time, PxControllerFilters());
}

bool CharacterController::SetPosition(const Point3F &position) {
    return native_controller_->setPosition(PxExtendedVec3(position.x, position.y, position.z));
}

Point3F CharacterController::Position() const {
    auto pose = native_controller_->getPosition();
    return {pose.x, pose.y, pose.z};
}

bool CharacterController::SetFootPosition(const Vector3F &position) {
    return native_controller_->setFootPosition(PxExtendedVec3(position.x, position.y, position.z));
}

Vector3F CharacterController::FootPosition() const {
    auto pose = native_controller_->getFootPosition();
    return {pose.x, pose.y, pose.z};
}

void CharacterController::SetStepOffset(const float offset) { native_controller_->setStepOffset(offset); }

float CharacterController::StepOffset() const { return native_controller_->getStepOffset(); }

void CharacterController::SetNonWalkableMode(PxControllerNonWalkableMode::Enum flag) {
    native_controller_->setNonWalkableMode(flag);
}

PxControllerNonWalkableMode::Enum CharacterController::NonWalkableMode() const {
    return native_controller_->getNonWalkableMode();
}

float CharacterController::ContactOffset() const { return native_controller_->getContactOffset(); }

void CharacterController::SetContactOffset(float offset) { native_controller_->setContactOffset(offset); }

Vector3F CharacterController::UpDirection() const {
    auto dir = native_controller_->getUpDirection();
    return {dir.x, dir.y, dir.z};
}

void CharacterController::SetUpDirection(const Vector3F &up) {
    native_controller_->setUpDirection(PxVec3(up.x, up.y, up.z));
}

float CharacterController::SlopeLimit() const { return native_controller_->getSlopeLimit(); }

void CharacterController::SetSlopeLimit(float slope_limit) { native_controller_->setSlopeLimit(slope_limit); }

void CharacterController::InvalidateCache() { native_controller_->invalidateCache(); }

void CharacterController::State(PxControllerState &state) const { native_controller_->getState(state); }

void CharacterController::Stats(PxControllerStats &stats) const { native_controller_->getStats(stats); }

void CharacterController::Resize(float height) { native_controller_->resize(height); }

void CharacterController::OnLateUpdate() { GetEntity()->transform->SetWorldPosition(Position()); }

void CharacterController::OnEnable() { PhysicsManager::GetSingleton().AddCharacterController(this); }

void CharacterController::OnDisable() { PhysicsManager::GetSingleton().RemoveCharacterController(this); }

}  // namespace vox::physics
