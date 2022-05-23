//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/component.h"

#include "vox.render/entity.h"

namespace vox {
Component::Component(vox::Entity *entity) : entity_(entity) {}

Component::~Component() {
    if (entity_->IsActiveInHierarchy()) {
        if (enabled_) {
            OnDisable();
        }
        OnInActive();
    }
}

bool Component::Enabled() const { return enabled_; }

void Component::SetEnabled(bool value) {
    if (value == enabled_) {
        return;
    }
    enabled_ = value;
    if (value) {
        if (entity_->IsActiveInHierarchy()) {
            OnEnable();
        }
    } else {
        if (entity_->IsActiveInHierarchy()) {
            OnDisable();
        }
    }
}

Entity *Component::GetEntity() const { return entity_; }

Scene *Component::GetScene() { return entity_->Scene(); }

void Component::SetActive(bool value) {
    if (value) {
        if (!awoken_) {
            awoken_ = true;
            OnAwake();
        }
        // You can do is_active = false in onAwake function.
        if (entity_->is_active_in_hierarchy) {
            OnActive();
            if (enabled_) {
                OnEnable();
            }
        }
    } else {
        if (enabled_) {
            OnDisable();
        }
        OnInActive();
    }
}

}  // namespace vox
