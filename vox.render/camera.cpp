/* Copyright (c) 2019, Sascha Willems
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "camera.h"
#include "matrix_utils.h"

namespace vox {
void Camera::update_view_matrix() {
    auto rotation_matrix = Matrix4x4F();
    Matrix4x4F transformation_matrix;
    
    rotation_matrix *= makeRotationMatrix(Vector3F(1.0f, 0.0f, 0.0f), degreesToRadians(rotation.x));
    rotation_matrix *= makeRotationMatrix(Vector3F(0.0f, 1.0f, 0.0f), degreesToRadians(rotation.y));
    rotation_matrix *= makeRotationMatrix(Vector3F(0.0f, 0.0f, 1.0f), degreesToRadians(rotation.z));
    
    transformation_matrix = makeTranslationMatrix(position);
    
    if (type == CameraType::FirstPerson) {
        matrices.view = rotation_matrix * transformation_matrix;
    } else {
        matrices.view = transformation_matrix * rotation_matrix;
    }
    
    updated = true;
}

bool Camera::moving() const {
    return keys.left || keys.right || keys.up || keys.down;
}

float Camera::get_near_clip() const {
    return znear;
}

float Camera::get_far_clip() const {
    return zfar;
}

void Camera::set_perspective(float fov, float aspect, float znear, float zfar) {
    this->fov = fov;
    this->znear = znear;
    this->zfar = zfar;
    matrices.perspective = makepPerspective(degreesToRadians(fov), aspect, znear, zfar);
}

void Camera::update_aspect_ratio(float aspect) {
    matrices.perspective = makepPerspective(degreesToRadians(fov), aspect, znear, zfar);
}

void Camera::set_position(const Point3F &position) {
    this->position = position;
    update_view_matrix();
}

void Camera::set_rotation(const Vector3F &rotation) {
    this->rotation = rotation;
    update_view_matrix();
}

void Camera::rotate(const Vector3F &delta) {
    this->rotation += delta;
    update_view_matrix();
}

void Camera::set_translation(const Vector3F &translation) {
    position = Point3F(translation.x, translation.y, translation.z);
    update_view_matrix();
}

void Camera::translate(const Vector3F &delta) {
    this->position += delta;
    update_view_matrix();
}

void Camera::update(float deltaTime) {
    updated = false;
    if (type == CameraType::FirstPerson) {
        if (moving()) {
            Vector3F front;
            front.x = -cos(degreesToRadians(rotation.x)) * sin(degreesToRadians(rotation.y));
            front.y = sin(degreesToRadians(rotation.x));
            front.z = cos(degreesToRadians(rotation.x)) * cos(degreesToRadians(rotation.y));
            front.normalize();
            
            float move_speed = deltaTime * translation_speed;
            
            if (keys.up) {
                position += front * move_speed;
            }
            if (keys.down) {
                position -= front * move_speed;
            }
            if (keys.left) {
                position -= front.cross(Vector3F(0.0f, 1.0f, 0.0f)).normalized() * move_speed;
            }
            if (keys.right) {
                position += front.cross(Vector3F(0.0f, 1.0f, 0.0f)).normalized() * move_speed;
            }
            
            update_view_matrix();
        }
    }
}

bool Camera::update_gamepad(const Vector2F &axis_left, const Vector2F &axis_right, float delta_time) {
    bool changed = false;
    
    if (type == CameraType::FirstPerson) {
        // Use the common console thumbstick layout
        // Left = view, right = move
        
        const float dead_zone = 0.0015f;
        const float range = 1.0f - dead_zone;
        
        Vector3F front;
        front.x = -cos(degreesToRadians(rotation.x)) * sin(degreesToRadians(rotation.y));
        front.y = sin(degreesToRadians(rotation.x));
        front.z = cos(degreesToRadians(rotation.x)) * cos(degreesToRadians(rotation.y));
        front.normalize();
        
        float move_speed = delta_time * translation_speed * 2.0f;
        float new_rotation_speed = delta_time * rotation_speed * 50.0f;
        
        // Move
        if (fabsf(axis_left.y) > dead_zone) {
            float pos = (fabsf(axis_left.y) - dead_zone) / range;
            position -= front * pos * ((axis_left.y < 0.0f) ? -1.0f : 1.0f) * move_speed;
            changed = true;
        }
        if (fabsf(axis_left.x) > dead_zone) {
            float pos = (fabsf(axis_left.x) - dead_zone) / range;
            position += front.cross(Vector3F(0.0f, 1.0f, 0.0f)).normalized() * pos *
            ((axis_left.x < 0.0f) ? -1.0f : 1.0f) * move_speed;
            changed = true;
        }
        
        // Rotate
        if (fabsf(axis_right.x) > dead_zone) {
            float pos = (fabsf(axis_right.x) - dead_zone) / range;
            rotation.y += pos * ((axis_right.x < 0.0f) ? -1.0f : 1.0f) * new_rotation_speed;
            changed = true;
        }
        if (fabsf(axis_right.y) > dead_zone) {
            float pos = (fabsf(axis_right.y) - dead_zone) / range;
            rotation.x -= pos * ((axis_right.y < 0.0f) ? -1.0f : 1.0f) * new_rotation_speed;
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
