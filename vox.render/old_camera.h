//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "matrix4x4.h"
#include "error.h"

namespace vox {
enum CameraType {
    LOOK_AT,
    FIRST_PERSON
};

class Camera {
public:
    void update(float delta_time);
    
    // Update camera passing separate axis data (gamepad)
    // Returns true if view or position has been changed
    bool update_gamepad(const Vector2F &axis_left, const Vector2F &axis_right, float delta_time);
    
    CameraType type_ = CameraType::LOOK_AT;
    
    Vector3F rotation_ = Vector3F();
    Point3F position_ = Point3F();
    
    float rotation_speed_ = 1.0f;
    float translation_speed_ = 1.0f;
    
    bool updated_ = false;
    
    struct {
        Matrix4x4F perspective_;
        Matrix4x4F view_;
    } matrices_;
    
    struct {
        bool left_ = false;
        bool right_ = false;
        bool up_ = false;
        bool down_ = false;
    } keys_;
    
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
    float fov_;
    float znear_, zfar_;
    
    void update_view_matrix();
};

}        // namespace vox
