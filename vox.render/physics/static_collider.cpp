//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "static_collider.h"
#include "physics_manager.h"
#include "../entity.h"

namespace vox {
namespace physics {
std::string StaticCollider::name() {
    return "StaticCollider";
}

StaticCollider::StaticCollider(Entity *entity) :
Collider(entity) {
    const auto &p = entity->transform_->world_position();
    auto q = entity->transform_->world_rotation_quaternion();
    q.normalize();
    
    _nativeActor = PhysicsManager::_nativePhysics()->createRigidStatic(PxTransform(PxVec3(p.x, p.y, p.z),
                                                                                   PxQuat(q.x, q.y, q.z, q.w)));
}

//MARK: - Reflection
void StaticCollider::on_serialize(nlohmann::json &data) {
    
}

void StaticCollider::on_deserialize(const nlohmann::json &data) {
    
}

void StaticCollider::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
}
