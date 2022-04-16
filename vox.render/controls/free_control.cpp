//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "free_control.h"
#include "../entity.h"
#include "math_utils.h"

namespace vox::control {
std::string FreeControl::name() {
    return "FreeControl";
}

FreeControl::FreeControl(Entity *entity) :
Script(entity) {
    // init spherical
    update_spherical();
}

void FreeControl::onDisable() {
    enable_event_ = false;
}

void FreeControl::onEnable() {
    enable_event_ = true;
}

void FreeControl::on_destroy() {
    onDisable();
}

void FreeControl::resize(uint32_t win_width, uint32_t win_height,
                         uint32_t fb_width, uint32_t fb_height) {
    width_ = win_width;
    height_ = win_height;
}

void FreeControl::input_event(const InputEvent &input_event) {
    if (enable_event_) {
        if (input_event.get_source() == EventSource::KEYBOARD) {
            const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
            if (key_event.get_action() == KeyAction::DOWN) {
                on_key_down(key_event.get_code());
            } else if (key_event.get_action() == KeyAction::UP) {
                on_key_up(key_event.get_code());
            }
        } else if (input_event.get_source() == EventSource::MOUSE) {
            const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
            if (mouse_button.get_action() == MouseAction::DOWN) {
                on_mouse_down(mouse_button.get_pos_x(), mouse_button.get_pos_y());
            } else if (mouse_button.get_action() == MouseAction::UP) {
                on_mouse_up();
            } else if (mouse_button.get_action() == MouseAction::MOVE) {
                on_mouse_move(mouse_button.get_pos_x(), mouse_button.get_pos_y());
            }
            //        } else if (input_event.get_source() == EventSource::Scroll) {
        } else if (input_event.get_source() == EventSource::TOUCHSCREEN) {
            // TODO
        }
    }
}

void FreeControl::on_key_down(KeyCode key) {
    switch (key) {
        case KeyCode::W:
        case KeyCode::UP:move_forward_ = true;
            break;
            
        case KeyCode::S:
        case KeyCode::DOWN:move_backward_ = true;
            break;
            
        case KeyCode::A:
        case KeyCode::LEFT:move_left_ = true;
            break;
            
        case KeyCode::D:
        case KeyCode::RIGHT:move_right_ = true;
            break;
            
        default:break;
    }
}

void FreeControl::on_key_up(KeyCode key) {
    switch (key) {
        case KeyCode::W:
        case KeyCode::UP:move_forward_ = false;
            break;
            
        case KeyCode::S:
        case KeyCode::DOWN:move_backward_ = false;
            break;
            
        case KeyCode::A:
        case KeyCode::LEFT:move_left_ = false;
            break;
            
        case KeyCode::D:
        case KeyCode::RIGHT:move_right_ = false;
            break;
            
        default:break;
    }
}

void FreeControl::on_mouse_down(double xpos, double ypos) {
    press_ = true;
    rotate_.x = static_cast<float>(xpos);
    rotate_.y = static_cast<float>(ypos);
}

void FreeControl::on_mouse_up() {
    press_ = false;
}

void FreeControl::on_mouse_move(double client_x, double client_y) {
    if (!press_) return;
    if (!enabled()) return;
    
    const auto kMovementX = client_x - rotate_.x;
    const auto kMovementY = client_y - rotate_.y;
    rotate_.x = static_cast<float>(client_x);
    rotate_.y = static_cast<float>(client_y);
    const auto kFactorX = 180.0 / width_;
    const auto kFactorY = 180.0 / height_;
    const auto kActualX = kMovementX * kFactorX;
    const auto kActualY = kMovementY * kFactorY;
    
    rotate(-static_cast<float>(kActualX), static_cast<float>(kActualY));
}

void FreeControl::rotate(float alpha, float beta) {
    theta_ += degreesToRadians(alpha);
    phi_ += degreesToRadians(beta);
    phi_ = clamp<float>(phi_, 1e-6, M_PI - 1e-6);
    spherical_.theta_ = theta_;
    spherical_.phi_ = phi_;
    spherical_.set_to_vec3(v3_cache_);
    Point3F offset = entity()->transform_->position() + v3_cache_;
    v3_cache_ = Vector3F(offset.x, offset.y, offset.y);
    entity()->transform_->look_at(offset, Vector3F(0, 1, 0));
}

void FreeControl::on_update(float delta) {
    if (!enabled()) return;
    
    const auto kActualMoveSpeed = delta * movement_speed_;
    forward_ = entity()->transform_->world_forward();
    right_ = entity()->transform_->world_right();
    
    if (move_forward_) {
        entity()->transform_->translate(forward_ * kActualMoveSpeed, false);
    }
    if (move_backward_) {
        entity()->transform_->translate(forward_ * (-kActualMoveSpeed), false);
    }
    if (move_left_) {
        entity()->transform_->translate(right_ * (-kActualMoveSpeed), false);
    }
    if (move_right_) {
        entity()->transform_->translate(right_ * kActualMoveSpeed, false);
    }
    
    if (floor_mock_) {
        const auto kPosition = entity()->transform_->position();
        if (kPosition.y != floor_y_) {
            entity()->transform_->set_position(kPosition.x, floor_y_, kPosition.z);
        }
    }
}

void FreeControl::update_spherical() {
    v3_cache_ = entity()->transform_->rotation_quaternion() * Vector3F(0, 0, -1);
    spherical_.set_from_vec3(v3_cache_);
    theta_ = spherical_.theta_;
    phi_ = spherical_.phi_;
}

}
