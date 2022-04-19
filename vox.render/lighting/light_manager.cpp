//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light_manager.h"
//#include "shader/shader.h"
#include "scene.h"
#include "camera.h"
//#include "lighting/wgsl/wgsl_cluster_compute.h"
//#include "lighting/wgsl/wgsl_cluster_debug.h"
//#include "shaderlib/wgsl_unlit.h"
#include "logging.h"

namespace vox {
LightManager *LightManager::get_singleton_ptr(void) {
    return ms_singleton_;
}

LightManager &LightManager::get_singleton(void) {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

LightManager::LightManager(Scene *scene) :
scene_(scene),
shader_data_(scene->device()),
_pointLightProperty(Shader::createProperty("u_pointLight", ShaderDataGroup::Scene)),
_spotLightProperty(Shader::createProperty("u_spotLight", ShaderDataGroup::Scene)),
_directLightProperty(Shader::createProperty("u_directLight", ShaderDataGroup::Scene)),
_forwardPlusProp(Shader::createProperty("u_cluster_uniform", ShaderDataGroup::Scene)),
_clustersProp(Shader::createProperty("u_clusters", ShaderDataGroup::Compute)),
_clusterLightsProp(Shader::createProperty("u_clusterLights", ShaderDataGroup::Scene)) {
    Shader::create("cluster_debug", std::make_unique<WGSLUnlitVertex>(),
                   std::make_unique<WGSLClusterDebug>(tile_count_, max_lights_per_cluster_));
    
    auto &device = scene_->device();
    clusters_buffer_ = std::make_unique<Buffer>(device, sizeof(Clusters),
                                                wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst);
    shader_data_.setBufferFunctor(clusters_prop_, [this]() -> Buffer {
        return *clusters_buffer_;
    });
    
    cluster_lights_buffer_ = std::make_unique<Buffer>(device, sizeof(ClusterLightGroup),
                                                      wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst);
    scene_->shaderData.setBufferFunctor(cluster_lights_prop_, [this]() -> Buffer {
        return *cluster_lights_buffer_;
    });
    
    _clusterBoundsCompute =
    std::make_unique<ComputePass>(device, std::make_unique<WGSLClusterBoundsSource>(tile_count_, max_lights_per_cluster_,
                                                                                    workgroup_size_));
    _clusterBoundsCompute->attachShaderData(&shader_data_);
    _clusterBoundsCompute->attachShaderData(&scene_->shaderData);
    _clusterBoundsCompute->setDispatchCount(dispatch_size_[0], dispatch_size_[1], dispatch_size_[2]);
    
    _clusterLightsCompute =
    std::make_unique<ComputePass>(device, std::make_unique<WGSLClusterLightsSource>(tile_count_, max_lights_per_cluster_,
                                                                                    workgroup_size_));
    _clusterLightsCompute->attachShaderData(&shader_data_);
    _clusterLightsCompute->attachShaderData(&scene_->shaderData);
    _clusterLightsCompute->setDispatchCount(dispatch_size_[0], dispatch_size_[1], dispatch_size_[2]);
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
        LOG(ERROR) << "Light already attached." << std::endl;;
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
        LOG(ERROR) << "Light already attached." << std::endl;;
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
        LOG(ERROR) << "Light already attached." << std::endl;;
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
    size_t pointLightCount = point_lights_.size();
    point_light_datas_.resize(pointLightCount);
    size_t spotLightCount = spot_lights_.size();
    spot_light_datas_.resize(spotLightCount);
    size_t directLightCount = direct_lights_.size();
    direct_light_datas_.resize(directLightCount);
    
    for (size_t i = 0; i < pointLightCount; i++) {
        point_lights_[i]->update_shader_data(point_light_datas_[i]);
    }
    
    for (size_t i = 0; i < spotLightCount; i++) {
        spot_lights_[i]->update_shader_data(spot_light_datas_[i]);
    }
    
    for (size_t i = 0; i < directLightCount; i++) {
        direct_lights_[i]->update_shader_data(direct_light_datas_[i]);
    }
    
    if (directLightCount) {
        shader_data.enableMacro(DIRECT_LIGHT_COUNT, directLightCount);
        shader_data.setData(LightManager::direct_light_property_, direct_light_datas_);
    } else {
        shader_data.disableMacro(DIRECT_LIGHT_COUNT);
    }
    
    if (pointLightCount) {
        shader_data.enableMacro(POINT_LIGHT_COUNT, pointLightCount);
        shader_data.setData(LightManager::point_light_property_, point_light_datas_);
    } else {
        shader_data.disableMacro(POINT_LIGHT_COUNT);
    }
    
    if (spotLightCount) {
        shader_data.enableMacro(SPOT_LIGHT_COUNT, spotLightCount);
        shader_data.setData(LightManager::spot_light_property_, spot_light_datas_);
    } else {
        shader_data.disableMacro(SPOT_LIGHT_COUNT);
    }
}

//MARK: - Forward Plus
bool LightManager::enable_forward_plus() const {
    return enable_forward_plus_;
}

void LightManager::draw(wgpu::CommandEncoder &commandEncoder) {
    update_shader_data(scene_->shaderData);
    
    size_t pointLightCount = point_lights_.size();
    size_t spotLightCount = spot_lights_.size();
    if (pointLightCount + spotLightCount > forward_plus_enable_min_count_) {
        enable_forward_plus_ = true;
        bool updateBounds = false;
        
        forward_plus_uniforms_.matrix = camera_->projectionMatrix();
        forward_plus_uniforms_.inverse_matrix = camera_->inverseProjectionMatrix();
        if (forward_plus_uniforms_.output_size.x != camera_->width() ||
            forward_plus_uniforms_.output_size.y != camera_->height()) {
            updateBounds = true;
            forward_plus_uniforms_.output_size = Vector2F(camera_->framebufferWidth(), camera_->framebufferHeight());
        }
        forward_plus_uniforms_.z_near = camera_->nearClipPlane();
        forward_plus_uniforms_.z_far = camera_->farClipPlane();
        forward_plus_uniforms_.view_matrix = camera_->viewMatrix();
        scene_->shaderData.setData(forward_plus_prop_, forward_plus_uniforms_);
        
        // Reset the light offset counter to 0 before populating the light clusters.
        uint32_t empty = 0;
        cluster_lights_buffer_->uploadData(scene_->device(), &empty, sizeof(uint32_t));
        
        auto encoder = commandEncoder.BeginComputePass();
        if (updateBounds) {
            _clusterBoundsCompute->compute(encoder);
        }
        _clusterLightsCompute->compute(encoder);
        encoder.End();
    }
}

}
