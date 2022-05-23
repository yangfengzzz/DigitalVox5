//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/controls/orbit_control.h"

#include "vox.render/entity.h"

namespace vox::control {
std::string OrbitControl::name() { return "OrbitControl"; }

OrbitControl::OrbitControl(Entity *entity) : Script(entity), camera_entity_(entity) {}

void OrbitControl::OnScriptDisable() {
    enable_event_ = false;
    enable_move_ = false;
}

void OrbitControl::OnScriptEnable() { enable_event_ = true; }

void OrbitControl::OnDestroy() { OnScriptDisable(); }

void OrbitControl::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    width_ = win_width;
    height_ = win_height;
}

void OrbitControl::InputEvent(const vox::InputEvent &input_event) {
    if (enable_event_) {
        if (input_event.GetSource() == EventSource::KEYBOARD) {
            const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
            OnKeyDown(key_event.GetCode());
        } else if (input_event.GetSource() == EventSource::MOUSE) {
            const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
            if (mouse_button.GetAction() == MouseAction::DOWN) {
                OnMouseDown(mouse_button.GetButton(), mouse_button.GetPosX(), mouse_button.GetPosY());
                enable_move_ = true;
            } else if (mouse_button.GetAction() == MouseAction::UP) {
                OnMouseUp();
                enable_move_ = false;
            }

            if (enable_move_ && mouse_button.GetAction() == MouseAction::MOVE) {
                OnMouseMove(mouse_button.GetPosX(), mouse_button.GetPosY());
            }
        } else if (input_event.GetSource() == EventSource::SCROLL) {
            const auto &scroll_event = static_cast<const ScrollInputEvent &>(input_event);
            OnMouseWheel(scroll_event.OffsetX(), scroll_event.OffsetY());
        } else if (input_event.GetSource() == EventSource::TOUCHSCREEN) {
            // TODO
        }
    }
}

void OrbitControl::OnUpdate(float delta_time) {
    if (!Enabled()) return;

    const auto &position = camera_entity_->transform->Position();
    offset_ = position - target_;
    spherical_.SetFromVec3(offset_);

    if (auto_rotate_ && state_ == STATE::NONE) {
        RotateLeft(AutoRotationAngle(delta_time));
    }

    spherical_.theta_ += spherical_delta_.theta_;
    spherical_.phi_ += spherical_delta_.phi_;

    spherical_.theta_ = std::max(min_azimuth_angle_, std::min(max_azimuth_angle_, spherical_.theta_));
    spherical_.phi_ = std::max(min_polar_angle_, std::min(max_polar_angle_, spherical_.phi_));
    spherical_.MakeSafe();

    if (scale_ != 1) {
        zoom_frag_ = spherical_.radius_ * (scale_ - 1);
    }

    spherical_.radius_ += zoom_frag_;
    spherical_.radius_ = std::max(min_distance_, std::min(max_distance_, spherical_.radius_));

    target_ = target_ + pan_offset_;
    spherical_.SetToVec3(offset_);
    position_ = target_;
    position_ = position_ + offset_;

    camera_entity_->transform->SetPosition(position_);
    camera_entity_->transform->LookAt(target_, up_);

    if (enable_damping_) {
        spherical_dump_.theta_ *= 1 - damping_factor_;
        spherical_dump_.phi_ *= 1 - damping_factor_;
        zoom_frag_ *= 1 - zoom_factor_;

        if (is_mouse_up_) {
            spherical_delta_.theta_ = spherical_dump_.theta_;
            spherical_delta_.phi_ = spherical_dump_.phi_;
        } else {
            spherical_delta_.Set(0, 0, 0);
        }
    } else {
        spherical_delta_.Set(0, 0, 0);
        zoom_frag_ = 0;
    }

    scale_ = 1;
    pan_offset_ = Vector3F(0, 0, 0);
}

float OrbitControl::AutoRotationAngle(float delta_time) const { return (auto_rotate_speed_ / 1000) * delta_time; }

float OrbitControl::ZoomScale() const { return std::pow(0.95f, zoom_speed_); }

void OrbitControl::RotateLeft(float radian) {
    spherical_delta_.theta_ -= radian;
    if (enable_damping_) {
        spherical_dump_.theta_ = -radian;
    }
}

void OrbitControl::RotateUp(float radian) {
    spherical_delta_.phi_ -= radian;
    if (enable_damping_) {
        spherical_dump_.phi_ = -radian;
    }
}

void OrbitControl::PanLeft(float distance, const Matrix4x4F &world_matrix) {
    v_pan_ = Vector3F(world_matrix[0], world_matrix[1], world_matrix[2]);
    v_pan_ = v_pan_ * distance;
    pan_offset_ = pan_offset_ + v_pan_;
}

void OrbitControl::PanUp(float distance, const Matrix4x4F &world_matrix) {
    v_pan_ = Vector3F(world_matrix[4], world_matrix[5], world_matrix[6]);
    v_pan_ = v_pan_ * distance;
    pan_offset_ = pan_offset_ + v_pan_;
}

void OrbitControl::Pan(float delta_x, float delta_y) {
    // perspective only
    Point3F position = camera_entity_->transform->Position();
    v_pan_ = position - target_;
    auto target_distance = v_pan_.length();

    target_distance *= (fov_ / 2.f) * (static_cast<float>(M_PI) / 180.f);

    PanLeft(-2 * delta_x * (target_distance / float(width_)), camera_entity_->transform->WorldMatrix());
    PanUp(2 * delta_y * (target_distance / float(height_)), camera_entity_->transform->WorldMatrix());
}

void OrbitControl::ZoomIn(float zoom_scale) { scale_ *= zoom_scale; }

void OrbitControl::ZoomOut(float zoom_scale) { scale_ /= zoom_scale; }

// MARK: - Mouse
void OrbitControl::HandleMouseDownRotate(double xpos, double ypos) {
    rotate_start_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
}

void OrbitControl::HandleMouseDownZoom(double xpos, double ypos) {
    zoom_start_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
}

void OrbitControl::HandleMouseDownPan(double xpos, double ypos) {
    pan_start_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
}

void OrbitControl::HandleMouseMoveRotate(double xpos, double ypos) {
    rotate_end_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
    rotate_delta_ = rotate_end_ - rotate_start_;

    RotateLeft(2.f * static_cast<float>(M_PI) * (rotate_delta_.x / static_cast<float>(width_)) * rotate_speed_);
    RotateUp(2.f * static_cast<float>(M_PI) * (rotate_delta_.y / static_cast<float>(height_)) * rotate_speed_);

    rotate_start_ = rotate_end_;
}

void OrbitControl::HandleMouseMoveZoom(double xpos, double ypos) {
    zoom_end_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
    zoom_delta_ = zoom_end_ - zoom_start_;

    if (zoom_delta_.y > 0) {
        ZoomOut(ZoomScale());
    } else if (zoom_delta_.y < 0) {
        ZoomIn(ZoomScale());
    }

    zoom_start_ = zoom_end_;
}

void OrbitControl::HandleMouseMovePan(double xpos, double ypos) {
    pan_end_ = Vector2F(static_cast<float>(xpos), static_cast<float>(ypos));
    pan_delta_ = pan_end_ - pan_start_;

    Pan(pan_delta_.x, pan_delta_.y);

    pan_start_ = pan_end_;
}

void OrbitControl::HandleMouseWheel(double xoffset, double yoffset) {
    if (yoffset < 0) {
        ZoomIn(ZoomScale());
    } else if (yoffset > 0) {
        ZoomOut(ZoomScale());
    }
}

void OrbitControl::OnMouseDown(MouseButton button, double xpos, double ypos) {
    if (!Enabled()) return;

    is_mouse_up_ = false;

    switch (button) {
        case MouseButton::LEFT:
            if (!enable_rotate_) return;

            HandleMouseDownRotate(xpos, ypos);
            state_ = STATE::ROTATE;
            break;
        case MouseButton::MIDDLE:
            if (!enable_zoom_) return;

            HandleMouseDownZoom(xpos, ypos);
            state_ = STATE::ZOOM;
            break;
        case MouseButton::RIGHT:
            if (!enable_pan_) return;

            HandleMouseDownPan(xpos, ypos);
            state_ = STATE::PAN;
            break;
        default:
            break;
    }
}

void OrbitControl::OnMouseMove(double xpos, double ypos) {
    if (!Enabled()) return;

    switch (state_) {
        case STATE::ROTATE:
            if (!enable_rotate_) return;

            HandleMouseMoveRotate(xpos, ypos);
            break;

        case STATE::ZOOM:
            if (!enable_zoom_) return;

            HandleMouseMoveZoom(xpos, ypos);
            break;

        case STATE::PAN:
            if (!enable_pan_) return;

            HandleMouseMovePan(xpos, ypos);
            break;
        default:
            break;
    }
}

void OrbitControl::OnMouseUp() {
    if (!Enabled()) return;

    is_mouse_up_ = true;
    state_ = STATE::NONE;
}

void OrbitControl::OnMouseWheel(double xoffset, double yoffset) {
    if (!Enabled() || !enable_zoom_ || (state_ != STATE::NONE && state_ != STATE::ROTATE)) return;

    HandleMouseWheel(xoffset, yoffset);
}

// MARK: - KeyBoard
void OrbitControl::HandleKeyDown(KeyCode key) {
    switch (key) {
        case KeyCode::UP:
            Pan(0, key_pan_speed_);
            break;
        case KeyCode::DOWN:
            Pan(0, -key_pan_speed_);
            break;
        case KeyCode::LEFT:
            Pan(key_pan_speed_, 0);
            break;
        case KeyCode::RIGHT:
            Pan(-key_pan_speed_, 0);
            break;
        default:
            break;
    }
}

void OrbitControl::OnKeyDown(KeyCode key) {
    if (!Enabled() || !enable_keys_ || !enable_pan_) return;

    HandleKeyDown(key);
}

// MARK: - Touch
void OrbitControl::HandleTouchStartRotate() {}

void OrbitControl::HandleTouchStartZoom() {}

void OrbitControl::HandleTouchStartPan() {}

void OrbitControl::HandleTouchMoveRotate() {}

void OrbitControl::HandleTouchMoveZoom() {}

void OrbitControl::HandleTouchMovePan() {}

void OrbitControl::OnTouchStart() {}

void OrbitControl::OnTouchMove() {}

void OrbitControl::OnTouchEnd() {}

}  // namespace vox::control
