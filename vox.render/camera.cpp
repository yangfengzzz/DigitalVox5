//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "camera.h"
#include "entity.h"
#include "scene.h"
#include "matrix_utils.h"
//#include "shader/shader.h"

namespace vox {
std::string Camera::name() {
    return "Camera";
}

Camera::Camera(Entity *entity) :
Component(entity),
shader_data_(entity->scene()->device()),
camera_property_("cameraData") {
    auto transform = entity->transform_;
    transform_ = transform;
    is_view_matrix_dirty_ = transform->register_world_change_flag();
    is_inv_view_proj_dirty_ = transform->register_world_change_flag();
    frustum_view_change_flag_ = transform->register_world_change_flag();
}

const BoundingFrustum &Camera::frustum() const {
    return frustum_;
}

float Camera::near_clip_plane() const {
    return near_clip_plane_;
}

void Camera::set_near_clip_plane(float value) {
    near_clip_plane_ = value;
    proj_mat_change();
}

float Camera::far_clip_plane() const {
    return far_clip_plane_;
}

void Camera::set_far_clip_plane(float value) {
    far_clip_plane_ = value;
    proj_mat_change();
}

float Camera::field_of_view() const {
    return field_of_view_;
}

void Camera::set_field_of_view(float value) {
    field_of_view_ = value;
    proj_mat_change();
}

float Camera::aspect_ratio() const {
    if (custom_aspect_ratio_ == std::nullopt) {
        return (static_cast<float>(width_) * viewport_.z) / (static_cast<float>(height_) * viewport_.w);
    } else {
        return custom_aspect_ratio_.value();
    }
}

void Camera::set_aspect_ratio(float value) {
    custom_aspect_ratio_ = value;
    proj_mat_change();
}

Vector4F Camera::viewport() const {
    return viewport_;
}

void Camera::set_viewport(const Vector4F &value) {
    viewport_ = value;
    proj_mat_change();
}

bool Camera::is_orthographic() const {
    return is_orthographic_;
}

void Camera::set_is_orthographic(bool value) {
    is_orthographic_ = value;
    proj_mat_change();
}

float Camera::orthographic_size() const {
    return orthographic_size_;
}

void Camera::set_orthographic_size(float value) {
    orthographic_size_ = value;
    proj_mat_change();
}

Matrix4x4F Camera::view_matrix() {
    // Remove scale
    if (is_view_matrix_dirty_->flag_) {
        is_view_matrix_dirty_->flag_ = false;
        view_matrix_ = transform_->world_matrix().inverse();
    }
    return view_matrix_;
}

void Camera::set_projection_matrix(const Matrix4x4F &value) {
    projection_matrix_ = value;
    is_proj_mat_setting_ = true;
    proj_mat_change();
}

Matrix4x4F Camera::projection_matrix() {
    if ((!is_projection_dirty_ || is_proj_mat_setting_) &&
        last_aspect_size_.x == static_cast<float>(width_) &&
        last_aspect_size_.y == static_cast<float>(height_)) {
        return projection_matrix_;
    }
    is_projection_dirty_ = false;
    last_aspect_size_.x = static_cast<float>(width_);
    last_aspect_size_.y = static_cast<float>(height_);
    if (!is_orthographic_) {
        projection_matrix_ = makePerspective(degreesToRadians(field_of_view_),
											 aspect_ratio(),
											 near_clip_plane_,
											 far_clip_plane_);
    } else {
        const auto kWidth = orthographic_size_ * aspect_ratio();
        const auto kHeight = orthographic_size_;
        projection_matrix_ = makeOrtho(-kWidth, kWidth, -kHeight, kHeight, near_clip_plane_, far_clip_plane_);
    }
    return projection_matrix_;
}

bool Camera::enable_hdr() {
    return false;
}

void Camera::set_enable_hdr(bool value) {
    assert(false && "not implementation");
}

void Camera::reset_projection_matrix() {
    is_proj_mat_setting_ = false;
    proj_mat_change();
}

void Camera::reset_aspect_ratio() {
    custom_aspect_ratio_ = std::nullopt;
    proj_mat_change();
}

Vector4F Camera::world_to_viewport_point(const Point3F &point) {
    auto temp_mat_4 = projection_matrix() * view_matrix();
    auto temp_vec_4 = Vector4F(point.x, point.y, point.z, 1.0);
    temp_vec_4 = temp_mat_4 * temp_vec_4;
    
    const auto kW = temp_vec_4.w;
    const auto kNx = temp_vec_4.x / kW;
    const auto kNy = temp_vec_4.y / kW;
    const auto kNz = temp_vec_4.z / kW;
    
    // Transform of coordinate axis.
    return {(kNx + 1.f) * 0.5f, (1.f - kNy) * 0.5f, kNz, kW};
}

Point3F Camera::viewport_to_world_point(const Vector3F &point) {
    return inner_viewport_to_world_point(point, inv_view_proj_mat());
}

Ray3F Camera::viewport_point_to_ray(const Vector2F &point) {
    Ray3F out;
    // Use the intersection of the near clipping plane as the origin point.
    Vector3F clip_point = Vector3F(point.x, point.y, 0);
    out.origin = viewport_to_world_point(clip_point);
    // Use the intersection of the far clipping plane as the origin point.
    clip_point.z = 1.0;
    Point3F far_point = inner_viewport_to_world_point(clip_point, inv_view_proj_mat_);
    out.direction = far_point - out.origin;
    out.direction = out.direction.normalized();
    
    return out;
}

Vector2F Camera::screen_to_viewport_point(const Vector2F &point) const {
    const Vector4F kViewport = this->viewport();
    return {(point.x / static_cast<float>(width_) - kViewport.x) / kViewport.z,
        (point.y / static_cast<float>(height_) - kViewport.y) / kViewport.w};
}

Vector3F Camera::screen_to_viewport_point(const Vector3F &point) const {
    const Vector4F kViewport = this->viewport();
    return {(point.x / static_cast<float>(width_) - kViewport.x) / kViewport.z,
        (point.y / static_cast<float>(height_) - kViewport.y) / kViewport.w, 0};
}

Vector2F Camera::viewport_to_screen_point(const Vector2F &point) const {
    const Vector4F kViewport = this->viewport();
    return {(kViewport.x + point.x * kViewport.z) * static_cast<float>(width_),
        (kViewport.y + point.y * kViewport.w) * static_cast<float>(height_)};
}

Vector3F Camera::viewport_to_screen_point(const Vector3F &point) const {
    const Vector4F kViewport = this->viewport();
    return {(kViewport.x + point.x * kViewport.z) * static_cast<float>(width_),
        (kViewport.y + point.y * kViewport.w) * static_cast<float>(height_), 0};
}

Vector4F Camera::viewport_to_screen_point(const Vector4F &point) const {
    const Vector4F kViewport = this->viewport();
    return {(kViewport.x + point.x * kViewport.z) * static_cast<float>(width_),
        (kViewport.y + point.y * kViewport.w) * static_cast<float>(height_), 0, 0};
}

Vector4F Camera::world_to_screen_point(const Point3F &point) {
    auto out = world_to_viewport_point(point);
    return viewport_to_screen_point(out);
}

Point3F Camera::screen_to_world_point(const Vector3F &point) {
    auto out = screen_to_viewport_point(point);
    return viewport_to_world_point(out);
}

Ray3F Camera::screen_point_to_ray(const Vector2F &point) {
    Vector2F viewport_point = screen_to_viewport_point(point);
    return viewport_point_to_ray(viewport_point);
}

void Camera::on_active() {
    entity()->scene()->attach_render_camera(this);
}

void Camera::on_in_active() {
    entity()->scene()->detach_render_camera(this);
}

void Camera::proj_mat_change() {
    is_frustum_project_dirty_ = true;
    is_projection_dirty_ = true;
    is_inv_proj_mat_dirty_ = true;
    is_inv_view_proj_dirty_->flag_ = true;
}

Point3F Camera::inner_viewport_to_world_point(const Vector3F &point, const Matrix4x4F &inv_view_proj_mat) {
    // Depth is a normalized value, 0 is nearPlane, 1 is far_clip_plane.
    const auto kDepth = point.z * 2 - 1;
    // Transform to clipping space matrix
    Point3F clip_point = Point3F(point.x * 2 - 1, 1 - point.y * 2, kDepth);
    clip_point = inv_view_proj_mat * clip_point;
    return clip_point;
}

void Camera::update() {
    camera_data_.view_mat = view_matrix();
    camera_data_.proj_mat = projection_matrix();
    camera_data_.vp_mat = projection_matrix() * view_matrix();
    camera_data_.view_inv_mat = transform_->world_matrix();
    camera_data_.proj_inv_mat = inverse_projection_matrix();
    camera_data_.camera_pos = transform_->world_position();
    shader_data_.set_data(Camera::camera_property_, camera_data_);
    
    if (enable_frustum_culling_ && (frustum_view_change_flag_->flag_ || is_frustum_project_dirty_)) {
        frustum_.calculateFromMatrix(camera_data_.vp_mat);
        frustum_view_change_flag_->flag_ = false;
        is_frustum_project_dirty_ = false;
    }
}

Matrix4x4F Camera::inv_view_proj_mat() {
    if (is_inv_view_proj_dirty_->flag_) {
        is_inv_view_proj_dirty_->flag_ = false;
        inv_view_proj_mat_ = transform_->world_matrix() * inverse_projection_matrix();
    }
    return inv_view_proj_mat_;
}

Matrix4x4F Camera::inverse_projection_matrix() {
    if (is_inv_proj_mat_dirty_) {
        is_inv_proj_mat_dirty_ = false;
        inverse_projection_matrix_ = projection_matrix().inverse();
    }
    return inverse_projection_matrix_;
}

void Camera::resize(uint32_t win_width, uint32_t win_height,
                    uint32_t fb_width, uint32_t fb_height) {
    width_ = win_width;
    height_ = win_height;
    fb_width_ = fb_width;
    fb_height_ = fb_height;
}

uint32_t Camera::width() const {
    return width_;
}

uint32_t Camera::height() const {
    return height_;
}

uint32_t Camera::framebuffer_width() const {
    return fb_width_;
}

uint32_t Camera::framebuffer_height() const {
    return fb_height_;
}

//MARK: - Reflection
void Camera::on_serialize(nlohmann::json &data) {
    
}

void Camera::on_deserialize(const nlohmann::json &data) {
    
}

void Camera::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
