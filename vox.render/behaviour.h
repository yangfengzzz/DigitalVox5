//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "script.h"
#include "event.h"
#include <sol/sol.hpp>

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
    bool register_to_lua_context(sol::state &lua_state, const std::string &script_folder);
    
    /**
     * Register the behaviour to lua
     * Returns true on success
     */
    void unregister_from_lua_context();
    
    /**
     * Call a lua function for this behaviour
     */
    template<typename... Args>
    void behaviour_call(const std::string &function_name, Args &&...args);
    
    /**
     * Return the lua table attached to this behaviour
     */
    sol::table &table();
    
    /**
     * Destructor
     */
    ~Behaviour() override;
    
public:
    /**
     * Called when be enabled first time, only once.
     */
    void onAwake() override;
    
    /**
     * Called when be enabled.
     */
    void onEnable() override;
    
    /**
     * Called when be disabled.
     */
    void onDisable() override;
    
    /**
     * Called at the end of the destroyed frame.
     */
    void on_destroy() override;
    
public:
    /**
     * Called before the frame-level loop start for the first time, only once.
     */
    void on_start() override;
    
    /**
     * The main loop, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    void on_update(float delta_time) override;
    
    /**
     * Called after the onUpdate finished, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    void on_late_update(float delta_time) override;
    
    /**
     * Called before camera rendering, called per camera.
     * @param camera - Current camera.
     */
    void on_begin_render(Camera *camera) override;
    
    /**
     * Called after camera rendering, called per camera.
     * @param camera - Current camera.
     */
    void on_end_render(Camera *camera) override;
    
    /**
     * Called when the collision enter.
     * @param other ColliderShape
     */
    void on_trigger_enter(const physics::ColliderShapePtr &other) override;
    
    /**
     * Called when the collision stay.
     * @remarks onTriggerStay is called every frame while the collision stay.
     * @param other ColliderShape
     */
    void on_trigger_exit(const physics::ColliderShapePtr &other) override;
    
    /**
     * Called when the collision exit.
     * @param other ColliderShape
     */
    void on_trigger_stay(const physics::ColliderShapePtr &other) override;
    
    void input_event(const InputEvent &input_event) override;
    
    void resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void on_inspector(ui::WidgetContainer &root) override;
    
private:
    sol::table object_ = sol::lua_nil;
};

}

#include "behaviour-inl.h"
