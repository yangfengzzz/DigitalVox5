//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "capsule_character_controller.h"

#include "../physics_manager.h"
#include "entity.h"
#include "scene.h"

namespace vox::physics {
std::string CapsuleCharacterController::name() { return "CapsuleCharacterController"; }

CapsuleCharacterController::CapsuleCharacterController(Entity *entity) : CharacterController(entity) {}

void CapsuleCharacterController::set_desc(const PxCapsuleControllerDesc &desc) {
    native_controller_ = PhysicsManager::GetSingleton().native_character_controller_manager_->createController(desc);
}

float CapsuleCharacterController::radius() const {
    return static_cast<PxCapsuleController *>(native_controller_)->getRadius();
}

bool CapsuleCharacterController::set_radius(float radius) {
    return static_cast<PxCapsuleController *>(native_controller_)->setRadius(radius);
}

float CapsuleCharacterController::height() const {
    return static_cast<PxCapsuleController *>(native_controller_)->getHeight();
}

bool CapsuleCharacterController::set_height(float height) {
    return static_cast<PxCapsuleController *>(native_controller_)->setHeight(height);
}

PxCapsuleClimbingMode::Enum CapsuleCharacterController::climbing_mode() const {
    return static_cast<PxCapsuleController *>(native_controller_)->getClimbingMode();
}

bool CapsuleCharacterController::set_climbing_mode(PxCapsuleClimbingMode::Enum mode) {
    return static_cast<PxCapsuleController *>(native_controller_)->setClimbingMode(mode);
}

// MARK: - Reflection
void CapsuleCharacterController::OnSerialize(nlohmann::json &data) {}

void CapsuleCharacterController::OnDeserialize(const nlohmann::json &data) {}

void CapsuleCharacterController::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox::physics
