//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/static_collider.h"

#include "vox.render/physics/physics_manager.h"

namespace vox::physics {
std::string StaticCollider::name() { return "StaticCollider"; }

StaticCollider::StaticCollider(Entity *entity) : Collider(entity) {
    const auto &p = entity->transform->WorldPosition();
    auto q = entity->transform->WorldRotationQuaternion();
    q.normalize();

    native_actor_ = PhysicsManager::native_physics_()->createRigidStatic(
            PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
}

// MARK: - Reflection
void StaticCollider::OnSerialize(nlohmann::json &data) {}

void StaticCollider::OnDeserialize(const nlohmann::json &data) {}

void StaticCollider::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox::physics
