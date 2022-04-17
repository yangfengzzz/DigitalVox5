//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "collider.h"
#include "shape/collider_shape.h"
#include "physics_manager.h"
#include "entity.h"
#include "scene.h"

namespace vox::physics {
Collider::Collider(Entity *entity) :
Component(entity) {
    update_flag_ = entity->transform_->register_world_change_flag();
}

Collider::~Collider() {
    clear_shapes();
}

PxRigidActor *Collider::handle() {
    return native_actor_;
}

void Collider::add_shape(const ColliderShapePtr &shape) {
    const auto &old_collider = shape->collider_;
    if (old_collider != this) {
        if (old_collider != nullptr) {
            old_collider->remove_shape(shape);
        }
        shapes_.push_back(shape);
        PhysicsManager::get_singleton().add_collider_shape(shape);
        native_actor_->attachShape(*shape->native_shape_);
        shape->collider_ = this;
    }
    
#ifdef DEBUG
    if (debug_entity_) {
		shape->set_entity(debug_entity_);
    }
#endif
}

void Collider::remove_shape(const ColliderShapePtr &shape) {
    auto iter = std::find(shapes_.begin(), shapes_.end(), shape);
    
    if (iter != shapes_.end()) {
        shapes_.erase(iter);
        native_actor_->detachShape(*shape->native_shape_);
        PhysicsManager::get_singleton().remove_collider_shape(shape);
        shape->collider_ = nullptr;
    }
    
#ifdef DEBUG
    if (debug_entity_) {
		shape->remove_entity(debug_entity_);
    }
#endif
}

void Collider::clear_shapes() {
    for (auto &shape : shapes_) {
        native_actor_->detachShape(*shape->native_shape_);
        PhysicsManager::get_singleton().remove_collider_shape(shape);
    }
    shapes_.clear();
}

void Collider::on_update() {
    if (update_flag_->flag_) {
        const auto &transform = entity()->transform_;
        const auto &p = transform->world_position();
        auto q = transform->world_rotation_quaternion();
        q.normalize();
        native_actor_->setGlobalPose(PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
        update_flag_->flag_ = false;
        
        const auto kWorldScale = transform->lossy_world_scale();
        for (auto &shape : shapes_) {
            shape->set_world_scale(kWorldScale);
        }
        
#ifdef DEBUG
        if (debug_entity_) {
            auto transform = native_actor_->getGlobalPose();
            debug_entity_->transform_->set_position(Point3F(transform.p.x, transform.p.y, transform.p.z));
            debug_entity_->transform_->set_rotation_quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
        }
#endif
    }
}

void Collider::on_enable() {
    PhysicsManager::get_singleton().add_collider(this);
}

void Collider::on_disable() {
    PhysicsManager::get_singleton().remove_collider(this);
}

}
