//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_CONTROLS_FREE_CONTROL_H_
#define DIGITALVOX_VOX_RENDER_CONTROLS_FREE_CONTROL_H_

#include "../script.h"
#include "spherical.h"
#include "vector3.h"
#include <array>

namespace vox::control {
/**
 * The camera's roaming controller, can move up and down, left and right, and rotate the viewing angle.
 */
class FreeControl : public Script {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    explicit FreeControl(Entity *entity);
    
    void onDisable() override;
    
    void onEnable() override;
    
    void on_destroy() override;
    
    void on_update(float delta) override;
    
    void input_event(const InputEvent &input_event) override;
    
    void resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;
    
public:
    /**
     * Keyboard press event.
     */
    void on_key_down(KeyCode key);
    
    /**
     * Keyboard up event.
     */
    void on_key_up(KeyCode key);
    
public:
    /**
     * Mouse press event.
     */
    void on_mouse_down(double xpos, double ypos);
    
    /**
     * Mouse up event.
     */
    void on_mouse_up();
    
    /**
     * Mouse movement event.
     */
    void on_mouse_move(double client_x, double client_y);
    
    /**
     * The angle of rotation around the y axis and the x axis respectively.
     * @param alpha - Radian to rotate around the y axis
     * @param beta - Radian to rotate around the x axis
     */
    void rotate(float alpha = 0, float beta = 0);
    
    /**
     * must update_spherical after quaternion has been changed
     * @example
     * Entity#lookAt([0,1,0],[0,1,0]);
     * AFreeControls#update_spherical();
     */
    void update_spherical();
    
private:
    Vector3F forward_;
    Vector3F right_;
    
    /**
     * Movement distance per second, the unit is the unit before MVP conversion.
     */
    float movement_speed_ = 1.0;
    
    /**
     * Rotate speed.
     */
    float rotate_speed_ = 1.0;
    
    /**
     * Simulate a ground.
     */
    bool floor_mock_ = false;
    
    /**
     * Simulated ground height.
     */
    float floor_y_ = 0;
    
    /**
     * Only rotate when press=true
     */
    bool press_ = false;
    
    /**
     * Radian of spherical.theta.
     */
    float theta_ = 0;
    
    /**
     * Radian of spherical.phi.
     */
    float phi_ = 0;
    
    bool move_forward_ = false;
    bool move_backward_ = false;
    bool move_left_ = false;
    bool move_right_ = false;
    
    Vector3F v3_cache_;
    Spherical spherical_;
    Vector2F rotate_{};
    
    bool enable_event_ = true;
    uint32_t width_ = 1000;
    uint32_t height_ = 1000;
};

}

#endif /* DIGITALVOX_VOX_RENDER_CONTROLS_FREE_CONTROL_H_ */
