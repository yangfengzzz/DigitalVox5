//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "orbit_control.h"
#include "../entity.h"

namespace vox::control {
std::string OrbitControl::name() {
    return "OrbitControl";
}

OrbitControl::OrbitControl(Entity *entity) :
Script(entity),
camera_entity_(entity) {
}

void OrbitControl::onDisable() {
    enable_event_ = false;
    enable_move_ = false;
}

void OrbitControl::onEnable() {
    enable_event_ = true;
}

void OrbitControl::on_destroy() {
    onDisable();
}

void OrbitControl::resize(uint32_t win_width, uint32_t win_height,
                          uint32_t fb_width, uint32_t fb_height) {
    width_ = win_width;
    height_ = win_height;
}

void OrbitControl::input_event(const InputEvent &input_event) {
    if (enable_event_) {
        if (input_event.GetSource() == EventSource::KEYBOARD) {
            const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
            on_key_down(key_event.GetCode());
        } else if (input_event.GetSource() == EventSource::MOUSE) {
            const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
            if (mouse_button.GetAction() == MouseAction::DOWN) {
                on_mouse_down(mouse_button.GetButton(), mouse_button.GetPosX(), mouse_button.GetPosY());
                enable_move_ = true;
            } else if (mouse_button.GetAction() == MouseAction::UP) {
                on_mouse_up();
                enable_move_ = false;
            }
            
            if (enable_move_ && mouse_button.GetAction() == MouseAction::MOVE) {
                on_mouse_move(mouse_button.GetPosX(), mouse_button.GetPosY());
            }
        } else if (input_event.GetSource() == EventSource::SCROLL) {
            const auto &scroll_event = static_cast<const ScrollInputEvent &>(input_event);
            on_mouse_wheel(scroll_event.OffsetX(), scroll_event.OffsetY());
        } else if (input_event.GetSource() == EventSource::TOUCHSCREEN) {
            // TODO
        }
    }
}

void OrbitControl::on_update(float dtime) {
    if (!enabled()) return;
    
    const auto &position = camera_entity_->transform_->position();
    offset_ = position - target_;
    spherical_.set_from_vec3(offset_);
    
    if (auto_rotate_ && state_ == STATE::NONE) {
        rotate_left(auto_rotation_angle(dtime));
    }
    
    spherical_.theta_ += spherical_delta_.theta_;
    spherical_.phi_ += spherical_delta_.phi_;
    
    spherical_.theta_ = std::max(min_azimuth_angle_, std::min(max_azimuth_angle_, spherical_.theta_));
    spherical_.phi_ = std::max(min_polar_angle_, std::min(max_polar_angle_, spherical_.phi_));
    spherical_.make_safe();
    
    if (scale_ != 1) {
        zoom_frag_ = spherical_.radius_ * (scale_ - 1);
    }
    
    spherical_.radius_ += zoom_frag_;
    spherical_.radius_ = std::max(min_distance_, std::min(max_distance_, spherical_.radius_));
    
    target_ = target_ + pan_offset_;
    spherical_.set_to_vec3(offset_);
    position_ = target_;
    position_ = position_ + offset_;
    
    camera_entity_->transform_->set_position(position_);
    camera_entity_->transform_->look_at(target_, up_);
    
    if (enable_damping_) {
        spherical_dump_.theta_ *= 1 - damping_factor_;
        spherical_dump_.phi_ *= 1 - damping_factor_;
        zoom_frag_ *= 1 - zoom_factor_;
        
        if (is_mouse_up_) {
            spherical_delta_.theta_ = spherical_dump_.theta_;
            spherical_delta_.phi_ = spherical_dump_.phi_;
        } else {
            spherical_delta_.set(0, 0, 0);
        }
    } else {
        spherical_delta_.set(0, 0, 0);
        zoom_frag_ = 0;
    }
    
    scale_ = 1;
    pan_offset_ = Vector3F(0, 0, 0);
}

float OrbitControl::auto_rotation_angle(float dtime) const {
    return (auto_rotate_speed_ / 1000) * dtime;
}

float OrbitControl::zoom_scale() const {
    return std::pow(0.95f, zoom_speed_);
}

void OrbitControl::rotate_left(float radian) {
    spherical_delta_.theta_ -= radian;
    if (enable_damping_) {
        spherical_dump_.theta_ = -radian;
    }
}

void OrbitControl::rotate_up(float radian) {
    spherical_delta_.phi_ -= radian;
    if (enable_damping_) {
        spherical_dump_.phi_ = -radian;
    }
}

void OrbitControl::pan_left(float distance, const Matrix4x4F &world_matrix) {
    v_pan_ = Vector3F(world_matrix[0], world_matrix[1], world_matrix[2]);
    v_pan_ = v_pan_ * distance;
    pan_offset_ = pan_offset_ + v_pan_;
}

void OrbitControl::pan_up(float distance, const Matrix4x4F &world_matrix) {
    v_pan_ = Vector3F(world_matrix[4], world_matrix[5], world_matrix[6]);
    v_pan_ = v_pan_ * distance;
    pan_offset_ = pan_offset_ + v_pan_;
}

void OrbitControl::pan(float delta_x, float delta_y) {
    // perspective only
    Point3F position = camera_entity_->transform_->position();
    v_pan_ = position - target_;
    auto target_distance = v_pan_.length();
    
    target_distance *= (fov_ / 2.f) * (static_cast<float>(M_PI) / 180.f);
    
    pan_left(-2 * delta_x * (target_distance / float(width_)), camera_entity_->transform_->world_matrix());
    pan_up(2 * delta_y * (target_distance / float(height_)), camera_entity_->transform_->world_matrix());
}

void OrbitControl::zoom_in(float zoom_scale) {
    scale_ *= zoom_scale;
}

void OrbitControl::zoom_out(float zoom_scale) {
    scale_ /= zoom_scale;
}

//MARK: - Mouse
void OrbitControl::handle_mouse_down_rotate(double xpos, double ypos) {
    rotate_start_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
}

void OrbitControl::handle_mouse_down_zoom(double xpos, double ypos) {
    zoom_start_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
}

void OrbitControl::handle_mouse_down_pan(double xpos, double ypos) {
    pan_start_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
}

void OrbitControl::handle_mouse_move_rotate(double xpos, double ypos) {
    rotate_end_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
    rotate_delta_ = rotate_end_ - rotate_start_;
    
    rotate_left(2.f * static_cast<float>(M_PI) * (rotate_delta_.x / static_cast<float>(width_)) * rotate_speed_);
    rotate_up(2.f * static_cast<float>(M_PI) * (rotate_delta_.y / static_cast<float>(height_)) * rotate_speed_);
    
    rotate_start_ = rotate_end_;
}

void OrbitControl::handle_mouse_move_zoom(double xpos, double ypos) {
    zoom_end_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
    zoom_delta_ = zoom_end_ - zoom_start_;
    
    if (zoom_delta_.y > 0) {
        zoom_out(zoom_scale());
    } else if (zoom_delta_.y < 0) {
        zoom_in(zoom_scale());
    }
    
    zoom_start_ = zoom_end_;
}

void OrbitControl::handle_mouse_move_pan(double xpos, double ypos) {
    pan_end_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
    pan_delta_ = pan_end_ - pan_start_;
    
    pan(pan_delta_.x, pan_delta_.y);
    
    pan_start_ = pan_end_;
}

void OrbitControl::handle_mouse_wheel(double xoffset, double yoffset) {
    if (yoffset < 0) {
        zoom_in(zoom_scale());
    } else if (yoffset > 0) {
        zoom_out(zoom_scale());
    }
}

void OrbitControl::on_mouse_down(MouseButton button, double xpos, double ypos) {
    if (!enabled()) return;
    
    is_mouse_up_ = false;
    
    switch (button) {
        case MouseButton::LEFT:if (!enable_rotate_) return;
            
            handle_mouse_down_rotate(xpos, ypos);
            state_ = STATE::ROTATE;
            break;
        case MouseButton::MIDDLE:if (!enable_zoom_) return;
            
            handle_mouse_down_zoom(xpos, ypos);
            state_ = STATE::ZOOM;
            break;
        case MouseButton::RIGHT:if (!enable_pan_) return;
            
            handle_mouse_down_pan(xpos, ypos);
            state_ = STATE::PAN;
            break;
        default:break;
    }
}

void OrbitControl::on_mouse_move(double xpos, double ypos) {
    if (!enabled()) return;
    
    switch (state_) {
        case STATE::ROTATE:if (!enable_rotate_) return;
            
            handle_mouse_move_rotate(xpos, ypos);
            break;
            
        case STATE::ZOOM:if (!enable_zoom_) return;
            
            handle_mouse_move_zoom(xpos, ypos);
            break;
            
        case STATE::PAN:if (!enable_pan_) return;
            
            handle_mouse_move_pan(xpos, ypos);
            break;
        default:break;
    }
}

void OrbitControl::on_mouse_up() {
    if (!enabled()) return;
    
    is_mouse_up_ = true;
    state_ = STATE::NONE;
}

void OrbitControl::on_mouse_wheel(double xoffset, double yoffset) {
    if (!enabled() || !enable_zoom_ ||
        (state_ != STATE::NONE && state_ != STATE::ROTATE))
        return;
    
    handle_mouse_wheel(xoffset, yoffset);
}

//MARK: - KeyBoard
void OrbitControl::handle_key_down(KeyCode key) {
    switch (key) {
        case KeyCode::UP:
            pan(0, key_pan_speed_);
            break;
        case KeyCode::DOWN:
            pan(0, -key_pan_speed_);
            break;
        case KeyCode::LEFT:
            pan(key_pan_speed_, 0);
            break;
        case KeyCode::RIGHT:
            pan(-key_pan_speed_, 0);
            break;
        default:break;
    }
}

void OrbitControl::on_key_down(KeyCode key) {
    if (!enabled() || !enable_keys_ || !enable_pan_) return;
    
    handle_key_down(key);
}

//MARK: - Touch
void OrbitControl::handle_touch_start_rotate() {
}

void OrbitControl::handle_touch_start_zoom() {
}

void OrbitControl::handle_touch_start_pan() {
}

void OrbitControl::handle_touch_move_rotate() {
}

void OrbitControl::handle_touch_move_zoom() {
}

void OrbitControl::handle_touch_move_pan() {
}

void OrbitControl::on_touch_start() {
}

void OrbitControl::on_touch_move() {
}

void OrbitControl::on_touch_end() {
}

}
