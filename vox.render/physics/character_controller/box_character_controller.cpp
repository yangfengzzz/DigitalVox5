//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/character_controller/box_character_controller.h"

#include "vox.render/physics/physics_manager.h"
#include "vox.render/entity.h"
#include "vox.render/scene.h"

namespace vox::physics {
std::string BoxCharacterController::name() { return "BoxCharacterController"; }

BoxCharacterController::BoxCharacterController(Entity *entity) : CharacterController(entity) {}

void BoxCharacterController::SetDesc(const PxBoxControllerDesc &desc) {
    native_controller_ = PhysicsManager::GetSingleton().native_character_controller_manager_->createController(desc);
}

float BoxCharacterController::HalfHeight() const {
    return static_cast<PxBoxController *>(native_controller_)->getHalfHeight();
}

float BoxCharacterController::HalfSideExtent() const {
    return static_cast<PxBoxController *>(native_controller_)->getHalfSideExtent();
}

float BoxCharacterController::HalfForwardExtent() const {
    return static_cast<PxBoxController *>(native_controller_)->getHalfForwardExtent();
}

bool BoxCharacterController::SetHalfHeight(float half_height) {
    return static_cast<PxBoxController *>(native_controller_)->setHalfHeight(half_height);
}

bool BoxCharacterController::SetHalfSideExtent(float half_side_extent) {
    return static_cast<PxBoxController *>(native_controller_)->setHalfSideExtent(half_side_extent);
}

bool BoxCharacterController::SetHalfForwardExtent(float half_forward_extent) {
    return static_cast<PxBoxController *>(native_controller_)->setHalfForwardExtent(half_forward_extent);
}

// MARK: - Reflection
void BoxCharacterController::OnSerialize(nlohmann::json &data) {}

void BoxCharacterController::OnDeserialize(const nlohmann::json &data) {}

void BoxCharacterController::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox::physics
