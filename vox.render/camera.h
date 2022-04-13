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

#pragma once

#include "matrix4x4.h"
#include "error.h"

namespace vox {
enum CameraType {
    LookAt,
    FirstPerson
};

class Camera {
public:
    void update(float deltaTime);
    
    // Update camera passing separate axis data (gamepad)
    // Returns true if view or position has been changed
    bool update_gamepad(const Vector2F &axis_left, const Vector2F &axis_right, float delta_time);
    
    CameraType type = CameraType::LookAt;
    
    Vector3F rotation = Vector3F();
    Point3F position = Point3F();
    
    float rotation_speed = 1.0f;
    float translation_speed = 1.0f;
    
    bool updated = false;
    
    struct {
        Matrix4x4F perspective;
        Matrix4x4F view;
    } matrices;
    
    struct {
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
    } keys;
    
    [[nodiscard]] bool moving() const;
    
    [[nodiscard]] float get_near_clip() const;
    
    [[nodiscard]] float get_far_clip() const;
    
    void set_perspective(float fov, float aspect, float znear, float zfar);
    
    void update_aspect_ratio(float aspect);
    
    void set_position(const Point3F &position);
    
    void set_rotation(const Vector3F &rotation);
    
    void rotate(const Vector3F &delta);
    
    void set_translation(const Vector3F &translation);
    
    void translate(const Vector3F &delta);
    
private:
    float fov;
    float znear, zfar;
    
    void update_view_matrix();
};

}        // namespace vox
