//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "box_character_controller.h"

#include "../physics_manager.h"
#include "entity.h"
#include "scene.h"

namespace vox::physics {
std::string BoxCharacterController::name() { return "BoxCharacterController"; }

BoxCharacterController::BoxCharacterController(Entity *entity) : CharacterController(entity) {}

void BoxCharacterController::set_desc(const PxBoxControllerDesc &desc) {
    native_controller_ = PhysicsManager::GetSingleton().native_character_controller_manager_->createController(desc);
}

float BoxCharacterController::half_height() const {
    return static_cast<PxBoxController *>(native_controller_)->getHalfHeight();
}

float BoxCharacterController::half_side_extent() const {
    return static_cast<PxBoxController *>(native_controller_)->getHalfSideExtent();
}

float BoxCharacterController::half_forward_extent() const {
    return static_cast<PxBoxController *>(native_controller_)->getHalfForwardExtent();
}

bool BoxCharacterController::set_half_height(float half_height) {
    return static_cast<PxBoxController *>(native_controller_)->setHalfHeight(half_height);
}

bool BoxCharacterController::set_half_side_extent(float half_side_extent) {
    return static_cast<PxBoxController *>(native_controller_)->setHalfSideExtent(half_side_extent);
}

bool BoxCharacterController::set_half_forward_extent(float half_forward_extent) {
    return static_cast<PxBoxController *>(native_controller_)->setHalfForwardExtent(half_forward_extent);
}

// MARK: - Reflection
void BoxCharacterController::OnSerialize(nlohmann::json &data) {}

void BoxCharacterController::OnDeserialize(const nlohmann::json &data) {}

void BoxCharacterController::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox::physics
