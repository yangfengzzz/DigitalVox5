//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light_manager.h"
#include "shader/internal_variant_name.h"
#include "shader/shader_manager.h"
#include "scene.h"
#include "camera.h"
#include "logging.h"

namespace vox {
LightManager *LightManager::get_singleton_ptr() {
    return ms_singleton_;
}

LightManager &LightManager::get_singleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

LightManager::LightManager(Scene *scene, RenderContext &render_context) :
scene_(scene),
shader_data_(scene->device()),
point_light_property_("pointLight"),
spot_light_property_("spotLight"),
direct_light_property_("directLight"),
forward_plus_prop_("clusterUniform"),
clusters_prop_("u_clusters"),
cluster_lights_prop_("clusterLights") {
    auto &device = scene_->device();
    clusters_buffer_ = std::make_unique<core::Buffer>(device, sizeof(Clusters),
                                                      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    shader_data_.set_buffer_functor(clusters_prop_, [this]() -> core::Buffer* {
        return clusters_buffer_.get();
    });
    
    cluster_lights_buffer_ = std::make_unique<core::Buffer>(device, sizeof(ClusterLightGroup),
                                                            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    scene_->shader_data_.set_buffer_functor(cluster_lights_prop_, [this]() -> core::Buffer* {
        return cluster_lights_buffer_.get();
    });
    
    cluster_bounds_compute_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    bounds_pass_ = &cluster_bounds_compute_->add_pass<PostProcessingComputePass>(ShaderManager::get_singleton().load_shader("base/light/cluster_bounds.comp"));
    bounds_pass_->set_dispatch_size(dispatch_size_);
    bounds_pass_->attach_shader_data(&shader_data_);
    bounds_pass_->attach_shader_data(&scene_->shader_data_);
    
    cluster_lights_compute_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    lights_pass_ = &cluster_lights_compute_->add_pass<PostProcessingComputePass>(ShaderManager::get_singleton().load_shader("base/light/cluster_light.comp"));
    lights_pass_->set_dispatch_size(dispatch_size_);
    lights_pass_->attach_shader_data(&shader_data_);
    lights_pass_->attach_shader_data(&scene_->shader_data_);
}

void LightManager::set_camera(Camera *camera) {
    camera_ = camera;
    bounds_pass_->attach_shader_data(&camera_->shader_data_);
    lights_pass_->attach_shader_data(&camera_->shader_data_);
}

//MARK: - Point Light
void LightManager::attach_point_light(PointLight *light) {
    auto iter = std::find(point_lights_.begin(), point_lights_.end(), light);
    if (iter == point_lights_.end()) {
        point_lights_.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::detach_point_light(PointLight *light) {
    auto iter = std::find(point_lights_.begin(), point_lights_.end(), light);
    if (iter != point_lights_.end()) {
        point_lights_.erase(iter);
    }
}

const std::vector<PointLight *> &LightManager::point_lights() const {
    return point_lights_;
}

//MARK: - Spot Light
void LightManager::attach_spot_light(SpotLight *light) {
    auto iter = std::find(spot_lights_.begin(), spot_lights_.end(), light);
    if (iter == spot_lights_.end()) {
        spot_lights_.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::detach_spot_light(SpotLight *light) {
    auto iter = std::find(spot_lights_.begin(), spot_lights_.end(), light);
    if (iter != spot_lights_.end()) {
        spot_lights_.erase(iter);
    }
}

const std::vector<SpotLight *> &LightManager::spot_lights() const {
    return spot_lights_;
}

//MARK: - Direct Light
void LightManager::attach_direct_light(DirectLight *light) {
    auto iter = std::find(direct_lights_.begin(), direct_lights_.end(), light);
    if (iter == direct_lights_.end()) {
        direct_lights_.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::detach_direct_light(DirectLight *light) {
    auto iter = std::find(direct_lights_.begin(), direct_lights_.end(), light);
    if (iter != direct_lights_.end()) {
        direct_lights_.erase(iter);
    }
}

const std::vector<DirectLight *> &LightManager::direct_lights() const {
    return direct_lights_;
}

void LightManager::update_shader_data(ShaderData &shader_data) {
    size_t point_light_count = point_lights_.size();
    point_light_datas_.resize(point_light_count);
    size_t spot_light_count = spot_lights_.size();
    spot_light_datas_.resize(spot_light_count);
    size_t direct_light_count = direct_lights_.size();
    direct_light_datas_.resize(direct_light_count);
    
    for (size_t i = 0; i < point_light_count; i++) {
        point_lights_[i]->update_shader_data(point_light_datas_[i]);
    }
    
    for (size_t i = 0; i < spot_light_count; i++) {
        spot_lights_[i]->update_shader_data(spot_light_datas_[i]);
    }
    
    for (size_t i = 0; i < direct_light_count; i++) {
        direct_lights_[i]->update_shader_data(direct_light_datas_[i]);
    }
    
    if (direct_light_count) {
        shader_data.add_define(DIRECT_LIGHT_COUNT + std::to_string(direct_light_count));
        shader_data.set_data(LightManager::direct_light_property_, direct_light_datas_);
    } else {
        shader_data.remove_define(DIRECT_LIGHT_COUNT);
    }
    
    if (point_light_count) {
        shader_data.add_define(POINT_LIGHT_COUNT + std::to_string(point_light_count));
        shader_data.set_data(LightManager::point_light_property_, point_light_datas_);
    } else {
        shader_data.remove_define(POINT_LIGHT_COUNT);
    }
    
    if (spot_light_count) {
        shader_data.add_define(SPOT_LIGHT_COUNT + std::to_string(spot_light_count));
        shader_data.set_data(LightManager::spot_light_property_, spot_light_datas_);
    } else {
        shader_data.remove_define(SPOT_LIGHT_COUNT);
    }
}

//MARK: - Forward Plus
void LightManager::draw(CommandBuffer &command_buffer, RenderTarget &render_target) {
    update_shader_data(scene_->shader_data_);
    
    size_t point_light_count = point_lights_.size();
    size_t spot_light_count = spot_lights_.size();
    if (point_light_count + spot_light_count > forward_plus_enable_min_count_) {
        scene_->shader_data_.add_define("NEED_FORWARD_PLUS");

        bool update_bounds = false;
        
        if (forward_plus_uniforms_.x != camera_->framebuffer_width() ||
            forward_plus_uniforms_.y != camera_->framebuffer_height()) {
            update_bounds = true;
            forward_plus_uniforms_.x = camera_->framebuffer_width();
            forward_plus_uniforms_.y = camera_->framebuffer_height();
        }
        forward_plus_uniforms_.z = camera_->near_clip_plane();
        forward_plus_uniforms_.w = camera_->far_clip_plane();
        scene_->shader_data_.set_data(forward_plus_prop_, forward_plus_uniforms_);
        
        // Reset the light offset counter to 0 before populating the light clusters.
        uint32_t empty = 0;
        cluster_lights_buffer_->update(&empty, sizeof(uint32_t));
        if (update_bounds) {
            cluster_bounds_compute_->draw(command_buffer, render_target);
        }
        cluster_lights_compute_->draw(command_buffer, render_target);
    }
}

}
