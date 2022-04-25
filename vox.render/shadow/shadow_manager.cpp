//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_manager.h"
#include "camera.h"
#include "entity.h"
#include "matrix_utils.h"
#include "image_manager.h"
#include "lighting/light_manager.h"

namespace vox {
uint32_t ShadowManager::shadow_count_ = 0;
uint32_t ShadowManager::cube_shadow_count_ = 0;
uint32_t ShadowManager::shadow_count() {
    return shadow_count_;
}

uint32_t ShadowManager::cube_shadow_count() {
    return cube_shadow_count_;
}

ShadowManager *ShadowManager::get_singleton_ptr() {
    return ms_singleton_;
}

ShadowManager &ShadowManager::get_singleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

void ShadowManager::update_spot_shadow(SpotLight *light, ShadowManager::ShadowData &shadow_data) {
    shadow_data.radius = light->shadow_radius();
    shadow_data.bias = light->shadow_bias();
    shadow_data.intensity = light->shadow_intensity();
    
    auto view_matrix = light->entity()->transform_->world_matrix().inverse();
    auto proj_matrix = light->shadow_projection_matrix();
    shadow_data.vp[0] = proj_matrix * view_matrix;
    shadow_data.cascade_splits[0] = 1;
    shadow_data.cascade_splits[1] = -1; // mark cascade with negative sign
}

void ShadowManager::update_cascades_shadow(DirectLight *light, ShadowManager::ShadowData &shadow_data) {
    shadow_data.radius = light->shadow_radius();
    shadow_data.bias = light->shadow_bias();
    shadow_data.intensity = light->shadow_intensity();
    
    std::array<float, shadow_map_cascade_count_> cascade_splits{};
    auto world_pos = light->entity()->transform_->world_position();
    
    float near_clip = camera_->near_clip_plane();
    float far_clip = camera_->far_clip_plane();
    float clip_range = far_clip - near_clip;
    
    float min_z = near_clip;
    float max_z = near_clip + clip_range;
    
    float range = max_z - min_z;
    float ratio = max_z / min_z;
    
    // Calculate split depths based on view camera frustum
    // Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
    for (uint32_t i = 0; i < shadow_map_cascade_count_; i++) {
        float p = static_cast<float>(i + 1) / static_cast<float>(shadow_map_cascade_count_);
        float log = min_z * std::pow(ratio, p);
        float uniform = min_z + range * p;
        float d = cascade_split_lambda_ * (log - uniform) + uniform;
        cascade_splits[i] = (d - near_clip) / clip_range;
    }
    
    std::array<Point3F, 8> frustumCorners = {
        Point3F(-1.0f, 1.0f, 0.0f),
        Point3F(1.0f, 1.0f, 0.0f),
        Point3F(1.0f, -1.0f, 0.0f),
        Point3F(-1.0f, -1.0f, 0.0f),
        Point3F(-1.0f, 1.0f, 1.0f),
        Point3F(1.0f, 1.0f, 1.0f),
        Point3F(1.0f, -1.0f, 1.0f),
        Point3F(-1.0f, -1.0f, 1.0f),
    };
    
    // Project frustum corners into world space
    Matrix4x4F inv_cam = (camera_->projection_matrix() * camera_->view_matrix()).inverse();
    for (uint32_t i = 0; i < 8; i++) {
        frustumCorners[i] = inv_cam * frustumCorners[i];
    }
    
    // Calculate orthographic projection matrix for each cascade
    float last_split_dist = 0.0;
    for (uint32_t i = 0; i < shadow_map_cascade_count_; i++) {
        float split_dist = cascade_splits[i];
        std::array<Point3F, 8> frustum_corners = frustumCorners;
        
        for (uint32_t i = 0; i < 4; i++) {
            Vector3F dist = frustum_corners[i + 4] - frustum_corners[i];
            frustum_corners[i + 4] = frustum_corners[i] + (dist * split_dist);
            frustum_corners[i] = frustum_corners[i] + (dist * last_split_dist);
        }
        
        auto light_mat = light->entity()->transform_->world_matrix();
        auto light_view_mat = light_mat.inverse();
        for (uint32_t i = 0; i < 8; i++) {
            frustum_corners[i] = light_view_mat * frustum_corners[i];
        }
        float far_dist = frustum_corners[7].distanceTo(frustum_corners[5]);
        float cross_dist = frustum_corners[7].distanceTo(frustum_corners[1]);
        float max_dist = far_dist > cross_dist ? far_dist : cross_dist;
        
        float min_x = std::numeric_limits<float>::infinity();
        float max_x = -std::numeric_limits<float>::infinity();
        float min_y = std::numeric_limits<float>::infinity();
        float max_y = -std::numeric_limits<float>::infinity();
        float min_z = std::numeric_limits<float>::infinity();
        float max_z = -std::numeric_limits<float>::infinity();
        for (uint32_t i = 0; i < 8; i++) {
            min_x = std::min(min_x, frustum_corners[i].x);
            max_x = std::max(max_x, frustum_corners[i].x);
            min_y = std::min(min_y, frustum_corners[i].y);
            max_y = std::max(max_y, frustum_corners[i].y);
            min_z = std::min(min_z, frustum_corners[i].z);
            max_z = std::max(max_z, frustum_corners[i].z);
        }
        
        // texel tile
        float world_units_per_texel = max_dist / (float)1000;
        float pos_x = (min_x + max_x) * 0.5f;
        pos_x /= world_units_per_texel;
        pos_x = std::floor(pos_x);
        pos_x *= world_units_per_texel;
        
        float pos_y = (min_y + max_y) * 0.5f;
        pos_y /= world_units_per_texel;
        pos_y = std::floor(pos_y);
        pos_y *= world_units_per_texel;
        
        float pos_z = max_z;
        pos_z /= world_units_per_texel;
        pos_z = std::floor(pos_z);
        pos_z *= world_units_per_texel;
        
        Point3F center = Point3F(pos_x, pos_y, pos_z);
        center = light_mat * center;
        light->entity()->transform_->set_world_position(center);
        
        float radius = max_dist / 2.f;
        Vector3F max_extents = Vector3F(radius, radius, radius);
        Vector3F min_extents = -max_extents;
        Matrix4x4F light_ortho_matrix = makeOrtho(min_extents.x, max_extents.x, min_extents.y, max_extents.y, 0.0f, max_z - min_z);
        
        // Store split distance and matrix in cascade
        shadow_data.cascade_splits[i] = (camera_->near_clip_plane() + split_dist * clip_range) * -1.0f;
        shadow_data.vp[i] = light_ortho_matrix * light->entity()->transform_->world_matrix().inverse();
        light->entity()->transform_->set_world_position(world_pos);
        last_split_dist = cascade_splits[i];
    }
}

void ShadowManager::update_point_shadow(PointLight *light, ShadowManager::CubeShadowData &shadow_data) {
    shadow_data.radius = light->shadow_radius();
    shadow_data.bias = light->shadow_bias();
    shadow_data.intensity = light->shadow_intensity();
    
    auto proj_matrix = light->shadow_projection_matrix();
    auto world_pos = light->entity()->transform_->world_position();
    shadow_data.light_pos = Vector4F(world_pos.x, world_pos.y, world_pos.z, 1.0);
    
    for (int i = 0; i < 6; i++) {
        light->entity()->transform_->look_at(world_pos + cube_map_direction_[i].first, cube_map_direction_[i].second);
        auto view_matrix = light->entity()->transform_->world_matrix().inverse();
        shadow_data.vp[i] = proj_matrix * view_matrix;
    }
}

}
