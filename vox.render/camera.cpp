//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/camera.h"

#include "vox.math/matrix_utils.h"
#include "vox.render/entity.h"
#include "vox.render/scene.h"

namespace vox {
std::string Camera::name() { return "Camera"; }

Camera::Camera(Entity *entity)
    : Component(entity), shader_data_(entity->Scene()->Device()), camera_property_("cameraData") {
    auto transform = entity->transform;
    transform_ = transform;
    is_view_matrix_dirty_ = transform->RegisterWorldChangeFlag();
    is_inv_view_proj_dirty_ = transform->RegisterWorldChangeFlag();
    frustum_view_change_flag_ = transform->RegisterWorldChangeFlag();
}

const BoundingFrustum &Camera::Frustum() const { return frustum_; }

float Camera::NearClipPlane() const { return near_clip_plane_; }

void Camera::SetNearClipPlane(float value) {
    near_clip_plane_ = value;
    ProjMatChange();
}

float Camera::FarClipPlane() const { return far_clip_plane_; }

void Camera::SetFarClipPlane(float value) {
    far_clip_plane_ = value;
    ProjMatChange();
}

float Camera::FieldOfView() const { return field_of_view_; }

void Camera::SetFieldOfView(float value) {
    field_of_view_ = value;
    ProjMatChange();
}

float Camera::AspectRatio() const {
    if (custom_aspect_ratio_ == std::nullopt) {
        return (static_cast<float>(width_) * viewport_.z) / (static_cast<float>(height_) * viewport_.w);
    } else {
        return custom_aspect_ratio_.value();
    }
}

void Camera::SetAspectRatio(float value) {
    custom_aspect_ratio_ = value;
    ProjMatChange();
}

Vector4F Camera::Viewport() const { return viewport_; }

void Camera::SetViewport(const Vector4F &value) {
    viewport_ = value;
    ProjMatChange();
}

bool Camera::IsOrthographic() const { return is_orthographic_; }

void Camera::SetIsOrthographic(bool value) {
    is_orthographic_ = value;
    ProjMatChange();
}

float Camera::OrthographicSize() const { return orthographic_size_; }

void Camera::SetOrthographicSize(float value) {
    orthographic_size_ = value;
    ProjMatChange();
}

Matrix4x4F Camera::ViewMatrix() {
    // Remove scale
    if (is_view_matrix_dirty_->flag_) {
        is_view_matrix_dirty_->flag_ = false;
        view_matrix_ = transform_->WorldMatrix().inverse();
    }
    return view_matrix_;
}

void Camera::SetProjectionMatrix(const Matrix4x4F &value) {
    projection_matrix_ = value;
    is_proj_mat_setting_ = true;
    ProjMatChange();
}

Matrix4x4F Camera::ProjectionMatrix() {
    if ((!is_projection_dirty_ || is_proj_mat_setting_) && last_aspect_size_.x == static_cast<float>(width_) &&
        last_aspect_size_.y == static_cast<float>(height_)) {
        return projection_matrix_;
    }
    is_projection_dirty_ = false;
    last_aspect_size_.x = static_cast<float>(width_);
    last_aspect_size_.y = static_cast<float>(height_);
    if (!is_orthographic_) {
        projection_matrix_ =
                makePerspective(degreesToRadians(field_of_view_), AspectRatio(), near_clip_plane_, far_clip_plane_);
    } else {
        const auto kWidth = orthographic_size_ * AspectRatio();
        const auto kHeight = orthographic_size_;
        projection_matrix_ = makeOrtho(-kWidth, kWidth, -kHeight, kHeight, near_clip_plane_, far_clip_plane_);
    }
    return projection_matrix_;
}

bool Camera::EnableHdr() { return false; }

void Camera::SetEnableHdr(bool value) { assert(false && "not implementation"); }

void Camera::ResetProjectionMatrix() {
    is_proj_mat_setting_ = false;
    ProjMatChange();
}

void Camera::ResetAspectRatio() {
    custom_aspect_ratio_ = std::nullopt;
    ProjMatChange();
}

Vector4F Camera::WorldToViewportPoint(const Point3F &point) {
    auto temp_mat_4 = ProjectionMatrix() * ViewMatrix();
    auto temp_vec_4 = Vector4F(point.x, point.y, point.z, 1.0);
    temp_vec_4 = temp_mat_4 * temp_vec_4;

    const auto kW = temp_vec_4.w;
    const auto kNx = temp_vec_4.x / kW;
    const auto kNy = temp_vec_4.y / kW;
    const auto kNz = temp_vec_4.z / kW;

    // Transform of coordinate axis.
    return {(kNx + 1.f) * 0.5f, (1.f - kNy) * 0.5f, kNz, kW};
}

Point3F Camera::ViewportToWorldPoint(const Vector3F &point) {
    return InnerViewportToWorldPoint(point, InvViewProjMat());
}

Ray3F Camera::ViewportPointToRay(const Vector2F &point) {
    Ray3F out;
    // Use the intersection of the near clipping plane as the origin point.
    Vector3F clip_point = Vector3F(point.x, point.y, 0);
    out.origin = ViewportToWorldPoint(clip_point);
    // Use the intersection of the far clipping plane as the origin point.
    clip_point.z = 1.0;
    Point3F far_point = InnerViewportToWorldPoint(clip_point, inv_view_proj_mat_);
    out.direction = far_point - out.origin;
    out.direction = out.direction.normalized();

    return out;
}

Vector2F Camera::ScreenToViewportPoint(const Vector2F &point) const {
    const Vector4F kViewport = this->Viewport();
    return {(point.x / static_cast<float>(width_) - kViewport.x) / kViewport.z,
            1.f - (point.y / static_cast<float>(height_) - kViewport.y) / kViewport.w};
}

Vector3F Camera::ScreenToViewportPoint(const Vector3F &point) const {
    const Vector4F kViewport = this->Viewport();
    return {(point.x / static_cast<float>(width_) - kViewport.x) / kViewport.z,
            1.f - (point.y / static_cast<float>(height_) - kViewport.y) / kViewport.w, 0};
}

Vector2F Camera::ViewportToScreenPoint(const Vector2F &point) const {
    const Vector4F kViewport = this->Viewport();
    return {(kViewport.x + point.x * kViewport.z) * static_cast<float>(width_),
            (kViewport.y + point.y * kViewport.w) * static_cast<float>(height_)};
}

Vector3F Camera::ViewportToScreenPoint(const Vector3F &point) const {
    const Vector4F kViewport = this->Viewport();
    return {(kViewport.x + point.x * kViewport.z) * static_cast<float>(width_),
            (kViewport.y + point.y * kViewport.w) * static_cast<float>(height_), 0};
}

Vector4F Camera::ViewportToScreenPoint(const Vector4F &point) const {
    const Vector4F kViewport = this->Viewport();
    return {(kViewport.x + point.x * kViewport.z) * static_cast<float>(width_),
            (kViewport.y + point.y * kViewport.w) * static_cast<float>(height_), 0, 0};
}

Vector4F Camera::WorldToScreenPoint(const Point3F &point) {
    auto out = WorldToViewportPoint(point);
    return ViewportToScreenPoint(out);
}

Point3F Camera::ScreenToWorldPoint(const Vector3F &point) {
    auto out = ScreenToViewportPoint(point);
    return ViewportToWorldPoint(out);
}

Ray3F Camera::ScreenPointToRay(const Vector2F &point) {
    Vector2F viewport_point = ScreenToViewportPoint(point);
    return ViewportPointToRay(viewport_point);
}

void Camera::OnActive() { GetEntity()->Scene()->AttachRenderCamera(this); }

void Camera::OnInActive() { GetEntity()->Scene()->DetachRenderCamera(this); }

void Camera::ProjMatChange() {
    is_frustum_project_dirty_ = true;
    is_projection_dirty_ = true;
    is_inv_proj_mat_dirty_ = true;
    is_inv_view_proj_dirty_->flag_ = true;
}

Point3F Camera::InnerViewportToWorldPoint(const Vector3F &point, const Matrix4x4F &inv_view_proj_mat) {
    // Depth is a normalized value, 0 is nearPlane, 1 is far_clip_plane.
    const auto kDepth = point.z * 2 - 1;
    // Transform to clipping space matrix
    Point3F clip_point = Point3F(point.x * 2 - 1, 1 - point.y * 2, kDepth);
    clip_point = inv_view_proj_mat * clip_point;
    return clip_point;
}

void Camera::Update() {
    camera_data_.view_mat = ViewMatrix();
    camera_data_.proj_mat = ProjectionMatrix();
    camera_data_.vp_mat = ProjectionMatrix() * ViewMatrix();
    camera_data_.view_inv_mat = transform_->WorldMatrix();
    camera_data_.proj_inv_mat = InverseProjectionMatrix();
    camera_data_.camera_pos = transform_->WorldPosition();
    shader_data_.SetData(Camera::camera_property_, camera_data_);

    if (enable_frustum_culling_ && (frustum_view_change_flag_->flag_ || is_frustum_project_dirty_)) {
        frustum_.calculateFromMatrix(camera_data_.vp_mat);
        frustum_view_change_flag_->flag_ = false;
        is_frustum_project_dirty_ = false;
    }
}

Matrix4x4F Camera::InvViewProjMat() {
    if (is_inv_view_proj_dirty_->flag_) {
        is_inv_view_proj_dirty_->flag_ = false;
        inv_view_proj_mat_ = transform_->WorldMatrix() * InverseProjectionMatrix();
    }
    return inv_view_proj_mat_;
}

Matrix4x4F Camera::InverseProjectionMatrix() {
    if (is_inv_proj_mat_dirty_) {
        is_inv_proj_mat_dirty_ = false;
        inverse_projection_matrix_ = ProjectionMatrix().inverse();
    }
    return inverse_projection_matrix_;
}

void Camera::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    width_ = win_width;
    height_ = win_height;
    fb_width_ = fb_width;
    fb_height_ = fb_height;
}

uint32_t Camera::Width() const { return width_; }

uint32_t Camera::Height() const { return height_; }

uint32_t Camera::FramebufferWidth() const { return fb_width_; }

uint32_t Camera::FramebufferHeight() const { return fb_height_; }

// MARK: - Reflection
void Camera::OnSerialize(nlohmann::json &data) {}

void Camera::OnDeserialize(const nlohmann::json &data) {}

void Camera::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
