//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "box_character_controller.h"
#include "entity.h"
#include "scene.h"
#include "physics_manager.h"

namespace vox {
namespace physics {
std::string BoxCharacterController::name() {
    return "BoxCharacterController";
}

BoxCharacterController::BoxCharacterController(Entity *entity) :
CharacterController(entity) {
}

void BoxCharacterController::setDesc(const PxBoxControllerDesc &desc) {
    _nativeController = PhysicsManager::get_singleton()._nativeCharacterControllerManager->createController(desc);
}

float BoxCharacterController::halfHeight() const {
    return static_cast<PxBoxController *>(_nativeController)->getHalfHeight();
}

float BoxCharacterController::halfSideExtent() const {
    return static_cast<PxBoxController *>(_nativeController)->getHalfSideExtent();
}

float BoxCharacterController::halfForwardExtent() const {
    return static_cast<PxBoxController *>(_nativeController)->getHalfForwardExtent();
}

bool BoxCharacterController::setHalfHeight(float halfHeight) {
    return static_cast<PxBoxController *>(_nativeController)->setHalfHeight(halfHeight);
}

bool BoxCharacterController::setHalfSideExtent(float halfSideExtent) {
    return static_cast<PxBoxController *>(_nativeController)->setHalfSideExtent(halfSideExtent);
}

bool BoxCharacterController::setHalfForwardExtent(float halfForwardExtent) {
    return static_cast<PxBoxController *>(_nativeController)->setHalfForwardExtent(halfForwardExtent);
}

//MARK: - Reflection
void BoxCharacterController::on_serialize(nlohmann::json &data) {
    
}

void BoxCharacterController::on_deserialize(const nlohmann::json &data) {
    
}

void BoxCharacterController::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
}
