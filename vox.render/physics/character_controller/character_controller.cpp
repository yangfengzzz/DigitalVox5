//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "character_controller.h"
#include "../physics_manager.h"
#include "entity.h"
#include "scene.h"

namespace vox::physics {
CharacterController::CharacterController(Entity *entity) :
Component(entity) {
}

PxControllerCollisionFlags CharacterController::move(const Vector3F &disp, float min_dist, float elapsed_time) {
    return native_controller_->move(PxVec3(disp.x, disp.y, disp.z), min_dist, elapsed_time, PxControllerFilters());
}

bool CharacterController::set_position(const Point3F &position) {
    return native_controller_->setPosition(PxExtendedVec3(position.x, position.y, position.z));
}

Point3F CharacterController::position() const {
    auto pose = native_controller_->getPosition();
    return {pose.x, pose.y, pose.z};
}

bool CharacterController::set_foot_position(const Vector3F &position) {
    return native_controller_->setFootPosition(PxExtendedVec3(position.x, position.y, position.z));
}

Vector3F CharacterController::foot_position() const {
    auto pose = native_controller_->getFootPosition();
    return {pose.x, pose.y, pose.z};
}

void CharacterController::set_step_offset(const float offset) {
    native_controller_->setStepOffset(offset);
}

float CharacterController::step_offset() const {
    return native_controller_->getStepOffset();
}

void CharacterController::set_non_walkable_mode(PxControllerNonWalkableMode::Enum flag) {
    native_controller_->setNonWalkableMode(flag);
}

PxControllerNonWalkableMode::Enum CharacterController::non_walkable_mode() const {
    return native_controller_->getNonWalkableMode();
}

float CharacterController::contact_offset() const {
    return native_controller_->getContactOffset();
}

void CharacterController::set_contact_offset(float offset) {
    native_controller_->setContactOffset(offset);
}

Vector3F CharacterController::up_direction() const {
    auto dir = native_controller_->getUpDirection();
    return {dir.x, dir.y, dir.z};
}

void CharacterController::set_up_direction(const Vector3F &up) {
    native_controller_->setUpDirection(PxVec3(up.x, up.y, up.z));
}

float CharacterController::slope_limit() const {
    return native_controller_->getSlopeLimit();
}

void CharacterController::set_slope_limit(float slope_limit) {
    native_controller_->setSlopeLimit(slope_limit);
}

void CharacterController::invalidate_cache() {
    native_controller_->invalidateCache();
}

void CharacterController::state(PxControllerState &state) const {
    native_controller_->getState(state);
}

void CharacterController::stats(PxControllerStats &stats) const {
    native_controller_->getStats(stats);
}

void CharacterController::resize(float height) {
    native_controller_->resize(height);
}

void CharacterController::on_late_update() {
    entity()->transform_->set_world_position(position());
}

void CharacterController::on_enable() {
    PhysicsManager::GetSingleton().add_character_controller(this);
}

void CharacterController::on_disable() {
    PhysicsManager::GetSingleton().remove_character_controller(this);
}

}
