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

ShadowManager::ShadowManager(Device &device, RenderContext &render_context,
                             Scene *scene, Camera *camera) :
device_(device),
render_context_(render_context),
scene_(scene),
camera_(camera),
shadow_map_prop_("u_shadowMap"),
shadow_sampler_prop_("u_shadowSampler"),
shadow_data_prop_("u_shadowData"),

cube_shadow_map_prop_("u_cubeShadowMap"),
cube_shadow_sampler_prop_("u_cubeShadowSampler"),
cube_shadow_data_prop_("u_cubeShadowData") {
    auto subpass = std::make_unique<ShadowSubpass>(render_context_, scene, camera);
    shadow_subpass_ = subpass.get();
    render_pipeline_ = std::make_unique<RenderPipeline>();
    render_pipeline_->add_subpass(std::move(subpass));
    
    float offset = static_cast<float>(shadow_map_resolution_ / 2);
    viewport_[0] = {0.f, 0.f,offset, offset, 0.f, 1.f};
    viewport_[1] = {offset, 0.f, offset, offset, 0.f, 1.f};
    viewport_[2] = {0.f, offset, offset, offset, 0.f, 1.f};
    viewport_[3] = {offset, offset, offset, offset, 0.f, 1.f};
}

float ShadowManager::cascade_split_lambda() {
    return cascade_split_lambda_;
}

void ShadowManager::set_cascade_split_lambda(float value) {
    cascade_split_lambda_ = value;
}

void ShadowManager::draw(CommandBuffer &command_buffer) {
    shadow_count_ = 0;
    draw_spot_shadow_map(command_buffer);
    draw_direct_shadow_map(command_buffer);
    if (shadow_count_) {
    }
    
    cube_shadow_count_ = 0;
    draw_point_shadow_map(command_buffer);
    if (cube_shadow_count_) {
    }
}

void ShadowManager::draw_spot_shadow_map(CommandBuffer &command_buffer) {
    const auto &lights = LightManager::get_singleton().spot_lights();
    for (const auto &light : lights) {
        if (light->enable_shadow() && shadow_count_ < max_shadow_) {
            update_spot_shadow(light, shadow_datas_[shadow_count_]);
            
            RenderTarget *render_target{nullptr};
            if (shadow_count_ < shadow_maps_.size()) {
                render_target = shadow_maps_[shadow_count_][render_context_.get_active_frame_index()].get();
            } else {
                std::vector<std::unique_ptr<RenderTarget>> shadow_render_targets(render_context_.get_render_frames().size());
                for (uint32_t i = 0; i < shadow_render_targets.size(); i++) {
                    shadow_render_targets[i] = create_shadow_render_target(shadow_map_resolution_);
                }
                render_target = shadow_render_targets[render_context_.get_active_frame_index()].get();
                shadow_maps_.emplace_back(std::move(shadow_render_targets));
            }
            
            shadow_subpass_->set_view_projection_matrix(shadow_datas_[shadow_count_].vp[0]);
            render_pipeline_->draw(command_buffer, *render_target);
            shadow_count_++;
        }
    }
}

void ShadowManager::draw_direct_shadow_map(CommandBuffer &command_buffer) {
    const auto &lights = LightManager::get_singleton().direct_lights();
    for (const auto &light : lights) {
        if (light->enable_shadow() && shadow_count_ < max_shadow_) {
            update_cascades_shadow(light, shadow_datas_[shadow_count_]);
            
            RenderTarget *render_target{nullptr};
            if (shadow_count_ < shadow_maps_.size()) {
                render_target = shadow_maps_[shadow_count_][render_context_.get_active_frame_index()].get();
            } else {
                std::vector<std::unique_ptr<RenderTarget>> shadow_render_targets(render_context_.get_render_frames().size());
                for (uint32_t i = 0; i < shadow_render_targets.size(); i++) {
                    shadow_render_targets[i] = create_shadow_render_target(shadow_map_resolution_);
                }
                render_target = shadow_render_targets[render_context_.get_active_frame_index()].get();
                shadow_maps_.emplace_back(std::move(shadow_render_targets));
            }
            
            for (int i = 0; i < shadow_map_cascade_count_; i++) {
                shadow_subpass_->set_view_projection_matrix(shadow_datas_[shadow_count_].vp[i]);
                shadow_subpass_->set_viewport(viewport_[i]);
                render_pipeline_->draw(command_buffer, *render_target);
            }
            shadow_count_++;
        }
    }
    shadow_subpass_->set_viewport(std::nullopt);
}

void ShadowManager::draw_point_shadow_map(CommandBuffer &command_buffer) {
    const auto &lights = LightManager::get_singleton().point_lights();
    for (const auto &light : lights) {
        if (light->enable_shadow() && cube_shadow_count_ < max_cube_shadow_) {
            update_point_shadow(light, cube_shadow_datas_[cube_shadow_count_]);
            
            RenderTarget *render_target{nullptr};
            if (cube_shadow_count_ < cube_shadow_maps_.size()) {
                render_target = cube_shadow_maps_[shadow_count_][render_context_.get_active_frame_index()].get();
            } else {
                std::vector<std::unique_ptr<RenderTarget>> shadow_render_targets(render_context_.get_render_frames().size());
                for (uint32_t i = 0; i < shadow_render_targets.size(); i++) {
                    shadow_render_targets[i] = create_shadow_render_target(shadow_map_resolution_);
                }
                render_target = shadow_render_targets[render_context_.get_active_frame_index()].get();
                cube_shadow_maps_.emplace_back(std::move(shadow_render_targets));
            }
            
            for (int i = 0; i < 6; i++) {
                shadow_subpass_->set_view_projection_matrix(cube_shadow_datas_[cube_shadow_count_].vp[i]);
                render_pipeline_->draw(command_buffer, *render_target);
            }
            cube_shadow_count_++;
        }
    }
}

std::unique_ptr<RenderTarget> ShadowManager::create_shadow_render_target(uint32_t size) {
    VkExtent3D extent{size, size, 1};
    
    core::Image depth_image{device_,
        extent,
        get_suitable_depth_format(device_.get_gpu().get_handle()),
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY
    };
    
    std::vector<core::Image> images;
    images.push_back(std::move(depth_image));
    return std::make_unique<RenderTarget>(std::move(images));
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
