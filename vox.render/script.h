//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/component.h"
#include "vox.render/platform/input_events.h"

namespace vox {
namespace physics {
class ColliderShape;

using ColliderShapePtr = std::shared_ptr<ColliderShape>;
}  // namespace physics

/**
 * Script class, used for logic writing.
 */
class Script : public Component {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit Script(vox::Entity *entity);

    ~Script() override;

    /**
     * Called when be enabled first time, only once.
     */
    virtual void OnScriptAwake() {}

    /**
     * Called when be enabled.
     */
    virtual void OnScriptEnable() {}

    /**
     * Called when be disabled.
     */
    virtual void OnScriptDisable() {}

    /**
     * Called at the end of the destroyed frame.
     */
    virtual void OnDestroy() {}

public:
    void SetIsStarted(bool value);

    [[nodiscard]] bool IsStarted() const;

    /**
     * Called before the frame-level loop start for the first time, only once.
     */
    virtual void OnStart() {}

    /**
     * The main loop, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    virtual void OnUpdate(float delta_time) {}

    /**
     * Called after the OnUpdate finished, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    virtual void OnLateUpdate(float delta_time) {}

    /**
     * Called before camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void OnBeginRender(Camera *camera) {}

    /**
     * Called after camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void OnEndRender(Camera *camera) {}

    /**
     * Called before physics calculations, the number of times is related to the physical update frequency.
     */
    virtual void OnPhysicsUpdate() {}

    /**
     * Called when the collision enter.
     * @param other ColliderShape
     */
    virtual void OnTriggerEnter(const physics::ColliderShapePtr &other) {}

    /**
     * Called when the collision stay.
     * @remarks OnTriggerStay is called every frame while the collision stay.
     * @param other ColliderShape
     */
    virtual void OnTriggerExit(const physics::ColliderShapePtr &other) {}

    /**
     * Called when the collision exit.
     * @param other ColliderShape
     */
    virtual void OnTriggerStay(const physics::ColliderShapePtr &other) {}

    virtual void InputEvent(const InputEvent &input_event) {}

    virtual void Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {}

public:
    /**
     * Called when the serialization is asked
     */
    void OnSerialize(nlohmann::json &data) override;

    /**
     * Called when the deserialization is asked
     */
    void OnDeserialize(const nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void OnInspector(ui::WidgetContainer &p_root) override;

protected:
    void OnAwake() override;

    void OnEnable() override;

    void OnDisable() override;

    bool started_ = false;
};

}  // namespace vox
