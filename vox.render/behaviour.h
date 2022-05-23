//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <sol/sol.hpp>

#include "vox.render/event.h"
#include "vox.render/script.h"

namespace vox {
/**
 * Behaviour class, used for lua logic writing.
 */
class Behaviour : public Script {
public:
    static Event<Behaviour *> created_event_;
    static Event<Behaviour *> destroyed_event_;

    std::string script_name_;

    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit Behaviour(Entity *entity);

    /**
     * Register the behaviour to lua
     * Returns true on success
     */
    bool RegisterToLuaContext(sol::state &lua_state, const std::string &script_folder);

    /**
     * Register the behaviour to lua
     * Returns true on success
     */
    void UnregisterFromLuaContext();

    /**
     * Call a lua function for this behaviour
     */
    template <typename... Args>
    void BehaviourCall(const std::string &function_name, Args &&...args);

    /**
     * Return the lua table attached to this behaviour
     */
    sol::table &Table();

    /**
     * Destructor
     */
    ~Behaviour() override;

public:
    /**
     * Called when be enabled first time, only once.
     */
    void OnScriptAwake() override;

    /**
     * Called when be enabled.
     */
    void OnScriptEnable() override;

    /**
     * Called when be disabled.
     */
    void OnScriptDisable() override;

    /**
     * Called at the end of the destroyed frame.
     */
    void OnDestroy() override;

public:
    /**
     * Called before the frame-level loop start for the first time, only once.
     */
    void OnStart() override;

    /**
     * The main loop, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    void OnUpdate(float delta_time) override;

    /**
     * Called after the onUpdate finished, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    void OnLateUpdate(float delta_time) override;

    /**
     * Called before camera rendering, called per camera.
     * @param camera - Current camera.
     */
    void OnBeginRender(Camera *camera) override;

    /**
     * Called after camera rendering, called per camera.
     * @param camera - Current camera.
     */
    void OnEndRender(Camera *camera) override;

    /**
     * Called when the collision enter.
     * @param other ColliderShape
     */
    void OnTriggerEnter(const physics::ColliderShapePtr &other) override;

    /**
     * Called when the collision stay.
     * @remarks onTriggerStay is called every frame while the collision stay.
     * @param other ColliderShape
     */
    void OnTriggerExit(const physics::ColliderShapePtr &other) override;

    /**
     * Called when the collision exit.
     * @param other ColliderShape
     */
    void OnTriggerStay(const physics::ColliderShapePtr &other) override;

    void InputEvent(const vox::InputEvent &input_event) override;

    void Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void OnInspector(ui::WidgetContainer &root) override;

private:
    sol::table object_ = sol::lua_nil;
};

}  // namespace vox

#include "vox.render/behaviour-inl.h"
