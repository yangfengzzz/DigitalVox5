//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "component.h"
#include "platform/input_events.h"

namespace vox {
namespace physics {
class ColliderShape;

using ColliderShapePtr = std::shared_ptr<ColliderShape>;
}

/**
 * Script class, used for logic writing.
 */
class Script : public Component {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    explicit Script(Entity *entity);
    
    ~Script() override;
    
    /**
     * Called when be enabled first time, only once.
     */
    virtual void onAwake() {
    }
    
    /**
     * Called when be enabled.
     */
    virtual void onEnable() {
    }
    
    /**
     * Called when be disabled.
     */
    virtual void onDisable() {
    }
    
    /**
     * Called at the end of the destroyed frame.
     */
    virtual void on_destroy() {
    }
    
public:
    void set_is_started(bool value);
    
    [[nodiscard]] bool is_started() const;
    
    /**
     * Called before the frame-level loop start for the first time, only once.
     */
    virtual void on_start() {
    }
    
    /**
     * The main loop, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    virtual void on_update(float delta_time) {
    }
    
    /**
     * Called after the on_update finished, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    virtual void on_late_update(float delta_time) {
    }
    
    /**
     * Called before camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void on_begin_render(Camera *camera) {
    }
    
    /**
     * Called after camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void on_end_render(Camera *camera) {
    }
    
    /**
     * Called when the collision enter.
     * @param other ColliderShape
     */
    virtual void on_trigger_enter(const physics::ColliderShapePtr &other) {
    }
    
    /**
     * Called when the collision stay.
     * @remarks on_trigger_stay is called every frame while the collision stay.
     * @param other ColliderShape
     */
    virtual void on_trigger_exit(const physics::ColliderShapePtr &other) {
    }
    
    /**
     * Called when the collision exit.
     * @param other ColliderShape
     */
    virtual void on_trigger_stay(const physics::ColliderShapePtr &other) {
    }
    
    virtual void input_event(const InputEvent &input_event) {
    }
    
    virtual void resize(uint32_t win_width, uint32_t win_height,
                        uint32_t fb_width, uint32_t fb_height) {
    }
    
public:
    /**
     * Called when the serialization is asked
     */
    void on_serialize(nlohmann::json &data) override;
    
    /**
     * Called when the deserialization is asked
     */
    void on_deserialize(const nlohmann::json &data) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void on_inspector(ui::WidgetContainer &p_root) override;
    
protected:
    void on_awake() override;
    
    void on_enable() override;
    
    void on_disable() override;
    
    bool started_ = false;
};

}
