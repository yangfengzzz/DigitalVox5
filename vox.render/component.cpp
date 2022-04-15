//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "component.h"
#include "entity.h"

namespace vox {
Component::Component(Entity *entity) : entity_(entity) {
}

Component::~Component() {
    if (entity_->is_active_in_hierarchy()) {
        if (enabled_) {
            on_disable();
        }
        on_in_active();
    }
}

bool Component::enabled() const {
    return enabled_;
}

void Component::set_enabled(bool value) {
    if (value == enabled_) {
        return;
    }
    enabled_ = value;
    if (value) {
        if (entity_->is_active_in_hierarchy()) {
            on_enable();
        }
    } else {
        if (entity_->is_active_in_hierarchy()) {
            on_disable();
        }
    }
}

Entity *Component::entity() const {
    return entity_;
}

Scene *Component::scene() {
    return entity_->scene();
}

void Component::set_active(bool value) {
    if (value) {
        if (!awoken_) {
            awoken_ = true;
            on_awake();
        }
        // You can do is_active = false in onAwake function.
        if (entity_->is_active_in_hierarchy_) {
            on_active();
            if (enabled_) {
                on_enable();
            }
        }
    } else {
        if (enabled_) {
            on_disable();
        }
        on_in_active();
    }
}

}        // namespace vox
