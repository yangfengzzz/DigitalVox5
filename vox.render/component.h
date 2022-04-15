//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "scene_forward.h"
#include "inspector_item.h"
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

namespace vox {
/**
 * The base class of the components.
 */
class Component : public InspectorItem {
public:
    explicit Component(Entity *entity);
    
    Component(Component &&other) = default;
    
    ~Component() override;
    
    /**
     * Indicates whether the component is enabled.
     */
    [[nodiscard]] bool enabled() const;
    
    void set_enabled(bool value);
    
    /**
     * The entity which the component belongs to.
     */
    [[nodiscard]] Entity *entity() const;
    
    /**
     * The scene which the component's entity belongs to.
     */
    Scene *scene();
    
public:
    virtual void on_awake() {
    }
    
    virtual void on_enable() {
    }
    
    virtual void on_disable() {
    }
    
    virtual void on_active() {
    }
    
    virtual void on_in_active() {
    }
    
protected:
    friend class Entity;
    
    void set_active(bool value);
    
    Entity *entity_;
    
private:
    bool enabled_ = true;
    bool awoken_ = false;
};

}        // namespace vox
