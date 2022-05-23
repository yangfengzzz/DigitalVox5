//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/controls/free_control.h"

#include "vox.math/math_utils.h"
#include "vox.render/entity.h"

namespace vox::control {
std::string FreeControl::name() { return "FreeControl"; }

FreeControl::FreeControl(Entity *entity) : Script(entity) {
    // init spherical
    UpdateSpherical();
}

void FreeControl::OnScriptDisable() { enable_event_ = false; }

void FreeControl::OnScriptEnable() { enable_event_ = true; }

void FreeControl::OnDestroy() { OnScriptDisable(); }

void FreeControl::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    width_ = win_width;
    height_ = win_height;
}

void FreeControl::InputEvent(const vox::InputEvent &input_event) {
    if (enable_event_) {
        if (input_event.GetSource() == EventSource::KEYBOARD) {
            const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
            if (key_event.GetAction() == KeyAction::DOWN) {
                OnKeyDown(key_event.GetCode());
            } else if (key_event.GetAction() == KeyAction::UP) {
                OnKeyUp(key_event.GetCode());
            }
        } else if (input_event.GetSource() == EventSource::MOUSE) {
            const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
            if (mouse_button.GetAction() == MouseAction::DOWN) {
                OnMouseDown(mouse_button.GetPosX(), mouse_button.GetPosY());
            } else if (mouse_button.GetAction() == MouseAction::UP) {
                OnMouseUp();
            } else if (mouse_button.GetAction() == MouseAction::MOVE) {
                OnMouseMove(mouse_button.GetPosX(), mouse_button.GetPosY());
            }
        } else if (input_event.GetSource() == EventSource::SCROLL) {
        } else if (input_event.GetSource() == EventSource::TOUCHSCREEN) {
            // TODO
        }
    }
}

void FreeControl::OnKeyDown(KeyCode key) {
    switch (key) {
        case KeyCode::W:
        case KeyCode::UP:
            move_forward_ = true;
            break;

        case KeyCode::S:
        case KeyCode::DOWN:
            move_backward_ = true;
            break;

        case KeyCode::A:
        case KeyCode::LEFT:
            move_left_ = true;
            break;

        case KeyCode::D:
        case KeyCode::RIGHT:
            move_right_ = true;
            break;

        default:
            break;
    }
}

void FreeControl::OnKeyUp(KeyCode key) {
    switch (key) {
        case KeyCode::W:
        case KeyCode::UP:
            move_forward_ = false;
            break;

        case KeyCode::S:
        case KeyCode::DOWN:
            move_backward_ = false;
            break;

        case KeyCode::A:
        case KeyCode::LEFT:
            move_left_ = false;
            break;

        case KeyCode::D:
        case KeyCode::RIGHT:
            move_right_ = false;
            break;

        default:
            break;
    }
}

void FreeControl::OnMouseDown(double xpos, double ypos) {
    press_ = true;
    rotate_.x = static_cast<float>(xpos);
    rotate_.y = static_cast<float>(ypos);
}

void FreeControl::OnMouseUp() { press_ = false; }

void FreeControl::OnMouseMove(double client_x, double client_y) {
    if (!press_) return;
    if (!Enabled()) return;

    const auto kMovementX = client_x - rotate_.x;
    const auto kMovementY = client_y - rotate_.y;
    rotate_.x = static_cast<float>(client_x);
    rotate_.y = static_cast<float>(client_y);
    const auto kFactorX = 180.0 / width_;
    const auto kFactorY = 180.0 / height_;
    const auto kActualX = kMovementX * kFactorX;
    const auto kActualY = kMovementY * kFactorY;

    Rotate(-static_cast<float>(kActualX), static_cast<float>(kActualY));
}

void FreeControl::Rotate(float alpha, float beta) {
    theta_ += degreesToRadians(alpha);
    phi_ += degreesToRadians(beta);
    phi_ = clamp<float>(phi_, 1e-6, M_PI - 1e-6);
    spherical_.theta_ = theta_;
    spherical_.phi_ = phi_;
    spherical_.SetToVec3(v3_cache_);
    Point3F offset = GetEntity()->transform->Position() + v3_cache_;
    v3_cache_ = Vector3F(offset.x, offset.y, offset.y);
    GetEntity()->transform->LookAt(offset, Vector3F(0, 1, 0));
}

void FreeControl::OnUpdate(float delta) {
    if (!Enabled()) return;

    const auto kActualMoveSpeed = delta * movement_speed_;
    forward_ = GetEntity()->transform->WorldForward();
    right_ = GetEntity()->transform->WorldRight();

    if (move_forward_) {
        GetEntity()->transform->Translate(forward_ * kActualMoveSpeed, false);
    }
    if (move_backward_) {
        GetEntity()->transform->Translate(forward_ * (-kActualMoveSpeed), false);
    }
    if (move_left_) {
        GetEntity()->transform->Translate(right_ * (-kActualMoveSpeed), false);
    }
    if (move_right_) {
        GetEntity()->transform->Translate(right_ * kActualMoveSpeed, false);
    }

    if (floor_mock_) {
        const auto kPosition = GetEntity()->transform->Position();
        if (kPosition.y != floor_y_) {
            GetEntity()->transform->SetPosition(kPosition.x, floor_y_, kPosition.z);
        }
    }
}

void FreeControl::UpdateSpherical() {
    v3_cache_ = GetEntity()->transform->RotationQuaternion() * Vector3F(0, 0, -1);
    spherical_.SetFromVec3(v3_cache_);
    theta_ = spherical_.theta_;
    phi_ = spherical_.phi_;
}

}  // namespace vox::control
