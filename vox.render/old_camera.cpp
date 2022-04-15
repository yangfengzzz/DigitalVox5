//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "old_camera.h"
#include "matrix_utils.h"

namespace vox {
void Camera::update_view_matrix() {
    auto rotation_matrix = Matrix4x4F();
    Matrix4x4F transformation_matrix;
    
    rotation_matrix *= makeRotationMatrix(Vector3F(1.0f, 0.0f, 0.0f), degreesToRadians(rotation_.x));
    rotation_matrix *= makeRotationMatrix(Vector3F(0.0f, 1.0f, 0.0f), degreesToRadians(rotation_.y));
    rotation_matrix *= makeRotationMatrix(Vector3F(0.0f, 0.0f, 1.0f), degreesToRadians(rotation_.z));
    
    transformation_matrix = makeTranslationMatrix(position_);
    
    if (type_ == CameraType::FIRST_PERSON) {
        matrices_.view_ = rotation_matrix * transformation_matrix;
    } else {
        matrices_.view_ = transformation_matrix * rotation_matrix;
    }
    
    updated_ = true;
}

bool Camera::moving() const {
    return keys_.left_ || keys_.right_ || keys_.up_ || keys_.down_;
}

float Camera::get_near_clip() const {
    return znear_;
}

float Camera::get_far_clip() const {
    return zfar_;
}

void Camera::set_perspective(float fov, float aspect, float znear, float zfar) {
    fov_ = fov;
    znear_ = znear;
    zfar_ = zfar;
    matrices_.perspective_ = makepPerspective(degreesToRadians(fov), aspect, znear, zfar);
}

void Camera::update_aspect_ratio(float aspect) {
    matrices_.perspective_ = makepPerspective(degreesToRadians(fov_), aspect, znear_, zfar_);
}

void Camera::set_position(const Point3F &position) {
    position_ = position;
    update_view_matrix();
}

void Camera::set_rotation(const Vector3F &rotation) {
    rotation_ = rotation;
    update_view_matrix();
}

void Camera::rotate(const Vector3F &delta) {
    rotation_ += delta;
    update_view_matrix();
}

void Camera::set_translation(const Vector3F &translation) {
    position_ = Point3F(translation.x, translation.y, translation.z);
    update_view_matrix();
}

void Camera::translate(const Vector3F &delta) {
    position_ += delta;
    update_view_matrix();
}

void Camera::update(float delta_time) {
    updated_ = false;
    if (type_ == CameraType::FIRST_PERSON) {
        if (moving()) {
            Vector3F front;
            front.x = -cos(degreesToRadians(rotation_.x)) * sin(degreesToRadians(rotation_.y));
            front.y = sin(degreesToRadians(rotation_.x));
            front.z = cos(degreesToRadians(rotation_.x)) * cos(degreesToRadians(rotation_.y));
            front.normalize();
            
            float move_speed = delta_time * translation_speed_;
            
            if (keys_.up_) {
                position_ += front * move_speed;
            }
            if (keys_.down_) {
                position_ -= front * move_speed;
            }
            if (keys_.left_) {
                position_ -= front.cross(Vector3F(0.0f, 1.0f, 0.0f)).normalized() * move_speed;
            }
            if (keys_.right_) {
                position_ += front.cross(Vector3F(0.0f, 1.0f, 0.0f)).normalized() * move_speed;
            }
            
            update_view_matrix();
        }
    }
}

bool Camera::update_gamepad(const Vector2F &axis_left, const Vector2F &axis_right, float delta_time) {
    bool changed = false;
    
    if (type_ == CameraType::FIRST_PERSON) {
        // Use the common console thumbstick layout
        // Left = view, right = move
        
        const float kDeadZone = 0.0015f;
        const float kRange = 1.0f - kDeadZone;
        
        Vector3F front;
        front.x = -cos(degreesToRadians(rotation_.x)) * sin(degreesToRadians(rotation_.y));
        front.y = sin(degreesToRadians(rotation_.x));
        front.z = cos(degreesToRadians(rotation_.x)) * cos(degreesToRadians(rotation_.y));
        front.normalize();
        
        float move_speed = delta_time * translation_speed_ * 2.0f;
        float new_rotation_speed = delta_time * rotation_speed_ * 50.0f;
        
        // Move
        if (fabsf(axis_left.y) > kDeadZone) {
            float pos = (fabsf(axis_left.y) - kDeadZone) / kRange;
            position_ -= front * pos * ((axis_left.y < 0.0f) ? -1.0f : 1.0f) * move_speed;
            changed = true;
        }
        if (fabsf(axis_left.x) > kDeadZone) {
            float pos = (fabsf(axis_left.x) - kDeadZone) / kRange;
            position_ += front.cross(Vector3F(0.0f, 1.0f, 0.0f)).normalized() * pos *
            ((axis_left.x < 0.0f) ? -1.0f : 1.0f) * move_speed;
            changed = true;
        }
        
        // Rotate
        if (fabsf(axis_right.x) > kDeadZone) {
            float pos = (fabsf(axis_right.x) - kDeadZone) / kRange;
            rotation_.y += pos * ((axis_right.x < 0.0f) ? -1.0f : 1.0f) * new_rotation_speed;
            changed = true;
        }
        if (fabsf(axis_right.y) > kDeadZone) {
            float pos = (fabsf(axis_right.y) - kDeadZone) / kRange;
            rotation_.x -= pos * ((axis_right.y < 0.0f) ? -1.0f : 1.0f) * new_rotation_speed;
            changed = true;
        }
    } else {
        // todo: move code from example base class for look-at
    }
    
    if (changed) {
        update_view_matrix();
    }
    
    return changed;
}

}        // namespace vox
