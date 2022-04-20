//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light_manager.h"
#include "shader/internal_variant_name.h"
#include "scene.h"
#include "camera.h"
//#include "lighting/wgsl/wgsl_cluster_compute.h"
//#include "lighting/wgsl/wgsl_cluster_debug.h"
//#include "shaderlib/wgsl_unlit.h"
#include "logging.h"

namespace vox {
LightManager *LightManager::get_singleton_ptr() {
    return ms_singleton_;
}

LightManager &LightManager::get_singleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

LightManager::LightManager(Scene *scene) :
scene_(scene),
shader_data_(scene->device()),
point_light_property_("u_pointLight"),
spot_light_property_("u_spotLight"),
direct_light_property_("u_directLight"),
forward_plus_prop_("u_cluster_uniform"),
clusters_prop_("u_clusters"),
cluster_lights_prop_("u_clusterLights") {
    //    Shader::create("cluster_debug", std::make_unique<WGSLUnlitVertex>(),
    //                   std::make_unique<WGSLClusterDebug>(tile_count_, max_lights_per_cluster_));
    //
    //    auto &device = scene_->device();
    //    clusters_buffer_ = std::make_unique<Buffer>(device, sizeof(Clusters),
    //                                                wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst);
    //    shader_data_.setBufferFunctor(clusters_prop_, [this]() -> Buffer {
    //        return *clusters_buffer_;
    //    });
    //
    //    cluster_lights_buffer_ = std::make_unique<Buffer>(device, sizeof(ClusterLightGroup),
    //                                                      wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst);
    //    scene_->shaderData.setBufferFunctor(cluster_lights_prop_, [this]() -> Buffer {
    //        return *cluster_lights_buffer_;
    //    });
    //
    //    _clusterBoundsCompute =
    //    std::make_unique<ComputePass>(device, std::make_unique<WGSLClusterBoundsSource>(tile_count_, max_lights_per_cluster_,
    //                                                                                    workgroup_size_));
    //    _clusterBoundsCompute->attachShaderData(&shader_data_);
    //    _clusterBoundsCompute->attachShaderData(&scene_->shaderData);
    //    _clusterBoundsCompute->setDispatchCount(dispatch_size_[0], dispatch_size_[1], dispatch_size_[2]);
    //
    //    _clusterLightsCompute =
    //    std::make_unique<ComputePass>(device, std::make_unique<WGSLClusterLightsSource>(tile_count_, max_lights_per_cluster_,
    //                                                                                    workgroup_size_));
    //    _clusterLightsCompute->attachShaderData(&shader_data_);
    //    _clusterLightsCompute->attachShaderData(&scene_->shaderData);
    //    _clusterLightsCompute->setDispatchCount(dispatch_size_[0], dispatch_size_[1], dispatch_size_[2]);
}

void LightManager::set_camera(Camera *camera) {
    camera_ = camera;
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
bool LightManager::enable_forward_plus() const {
    return enable_forward_plus_;
}

void LightManager::draw(CommandBuffer &command_buffer) {
    update_shader_data(scene_->shader_data_);
    
    size_t point_light_count = point_lights_.size();
    size_t spot_light_count = spot_lights_.size();
    if (point_light_count + spot_light_count > forward_plus_enable_min_count_) {
        enable_forward_plus_ = true;
        bool update_bounds = false;
        
        forward_plus_uniforms_.matrix = camera_->projection_matrix();
        forward_plus_uniforms_.inverse_matrix = camera_->inverse_projection_matrix();
        if (forward_plus_uniforms_.output_size[0] != camera_->width() ||
            forward_plus_uniforms_.output_size[1] != camera_->height()) {
            update_bounds = true;
            forward_plus_uniforms_.output_size = {camera_->framebuffer_width(), camera_->framebuffer_height()};
        }
        forward_plus_uniforms_.z_near = camera_->near_clip_plane();
        forward_plus_uniforms_.z_far = camera_->far_clip_plane();
        forward_plus_uniforms_.view_matrix = camera_->view_matrix();
        scene_->shader_data_.set_data(forward_plus_prop_, forward_plus_uniforms_);
        
        // Reset the light offset counter to 0 before populating the light clusters.
        //        uint32_t empty = 0;
        //        cluster_lights_buffer_->uploadData(scene_->device(), &empty, sizeof(uint32_t));
        //
        //        auto encoder = commandEncoder.BeginComputePass();
        //        if (updateBounds) {
        //            _clusterBoundsCompute->compute(encoder);
        //        }
        //        _clusterLightsCompute->compute(encoder);
        //        encoder.End();
    }
}

}
