//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/character_controller/capsule_character_controller.h"

#include "vox.render/entity.h"
#include "vox.render/physics/physics_manager.h"
#include "vox.render/scene.h"

namespace vox::physics {
std::string CapsuleCharacterController::name() { return "CapsuleCharacterController"; }

CapsuleCharacterController::CapsuleCharacterController(Entity *entity) : CharacterController(entity) {}

void CapsuleCharacterController::SetDesc(const PxCapsuleControllerDesc &desc) {
    native_controller_ = PhysicsManager::GetSingleton().native_character_controller_manager_->createController(desc);
}

float CapsuleCharacterController::Radius() const {
    return static_cast<PxCapsuleController *>(native_controller_)->getRadius();
}

bool CapsuleCharacterController::SetRadius(float radius) {
    return static_cast<PxCapsuleController *>(native_controller_)->setRadius(radius);
}

float CapsuleCharacterController::Height() const {
    return static_cast<PxCapsuleController *>(native_controller_)->getHeight();
}

bool CapsuleCharacterController::SetHeight(float height) {
    return static_cast<PxCapsuleController *>(native_controller_)->setHeight(height);
}

PxCapsuleClimbingMode::Enum CapsuleCharacterController::ClimbingMode() const {
    return static_cast<PxCapsuleController *>(native_controller_)->getClimbingMode();
}

bool CapsuleCharacterController::SetClimbingMode(PxCapsuleClimbingMode::Enum mode) {
    return static_cast<PxCapsuleController *>(native_controller_)->setClimbingMode(mode);
}

// MARK: - Reflection
void CapsuleCharacterController::OnSerialize(nlohmann::json &data) {}

void CapsuleCharacterController::OnDeserialize(const nlohmann::json &data) {}

void CapsuleCharacterController::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox::physics
