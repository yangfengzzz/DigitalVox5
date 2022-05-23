//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <vector>

#include "vox.render/inspector_item.h"
#include "vox.render/scene_forward.h"

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
    [[nodiscard]] bool Enabled() const;

    void SetEnabled(bool value);

    /**
     * The entity which the component belongs to.
     */
    [[nodiscard]] Entity *GetEntity() const;

    /**
     * The scene which the component's entity belongs to.
     */
    Scene *GetScene();

public:
    virtual void OnAwake() {}

    virtual void OnEnable() {}

    virtual void OnDisable() {}

    virtual void OnActive() {}

    virtual void OnInActive() {}

protected:
    friend class Entity;

    void SetActive(bool value);

    vox::Entity *entity_;

private:
    bool enabled_ = true;
    bool awoken_ = false;
};

}  // namespace vox
