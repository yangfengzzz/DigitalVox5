//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/light_manager.h"

#include "vox.base/logging.h"
#include "vox.render/camera.h"
#include "vox.render/scene.h"
#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
LightManager *LightManager::GetSingletonPtr() { return ms_singleton; }

LightManager &LightManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

LightManager::LightManager(Scene *scene, RenderContext &render_context)
    : scene_(scene),
      shader_data_(scene->Device()),
      point_light_property_("pointLight"),
      spot_light_property_("spotLight"),
      direct_light_property_("directLight"),
      forward_plus_prop_("clusterUniform"),
      clusters_prop_("u_clusters"),
      cluster_lights_prop_("clusterLights") {
    auto &device = scene_->Device();
    clusters_buffer_ = std::make_unique<core::Buffer>(device, sizeof(Clusters), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                      VMA_MEMORY_USAGE_GPU_ONLY);
    shader_data_.SetBufferFunctor(clusters_prop_, [this]() -> core::Buffer * { return clusters_buffer_.get(); });

    cluster_lights_buffer_ = std::make_unique<core::Buffer>(
            device, sizeof(ClusterLightGroup), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    scene_->shader_data.SetBufferFunctor(cluster_lights_prop_,
                                         [this]() -> core::Buffer * { return cluster_lights_buffer_.get(); });

    cluster_bounds_compute_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    bounds_pass_ = &cluster_bounds_compute_->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/light/cluster_bounds.comp"));
    bounds_pass_->SetDispatchSize(dispatch_size_);
    bounds_pass_->AttachShaderData(&shader_data_);
    bounds_pass_->AttachShaderData(&scene_->shader_data);

    cluster_lights_compute_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    lights_pass_ = &cluster_lights_compute_->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/light/cluster_light.comp"));
    lights_pass_->SetDispatchSize(dispatch_size_);
    lights_pass_->AttachShaderData(&shader_data_);
    lights_pass_->AttachShaderData(&scene_->shader_data);
}

void LightManager::SetCamera(Camera *camera) {
    camera_ = camera;
    bounds_pass_->AttachShaderData(&camera_->shader_data_);
    lights_pass_->AttachShaderData(&camera_->shader_data_);
}

// MARK: - Point Light
void LightManager::AttachPointLight(PointLight *light) {
    auto iter = std::find(point_lights_.begin(), point_lights_.end(), light);
    if (iter == point_lights_.end()) {
        point_lights_.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::DetachPointLight(PointLight *light) {
    auto iter = std::find(point_lights_.begin(), point_lights_.end(), light);
    if (iter != point_lights_.end()) {
        point_lights_.erase(iter);
    }
}

const std::vector<PointLight *> &LightManager::PointLights() const { return point_lights_; }

// MARK: - Spot Light
void LightManager::AttachSpotLight(SpotLight *light) {
    auto iter = std::find(spot_lights_.begin(), spot_lights_.end(), light);
    if (iter == spot_lights_.end()) {
        spot_lights_.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::DetachSpotLight(SpotLight *light) {
    auto iter = std::find(spot_lights_.begin(), spot_lights_.end(), light);
    if (iter != spot_lights_.end()) {
        spot_lights_.erase(iter);
    }
}

const std::vector<SpotLight *> &LightManager::SpotLights() const { return spot_lights_; }

// MARK: - Direct Light
void LightManager::AttachDirectLight(DirectLight *light) {
    auto iter = std::find(direct_lights_.begin(), direct_lights_.end(), light);
    if (iter == direct_lights_.end()) {
        direct_lights_.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::DetachDirectLight(DirectLight *light) {
    auto iter = std::find(direct_lights_.begin(), direct_lights_.end(), light);
    if (iter != direct_lights_.end()) {
        direct_lights_.erase(iter);
    }
}

const std::vector<DirectLight *> &LightManager::DirectLights() const { return direct_lights_; }

void LightManager::UpdateShaderData(ShaderData &shader_data) {
    size_t point_light_count = point_lights_.size();
    point_light_datas_.resize(point_light_count);
    size_t spot_light_count = spot_lights_.size();
    spot_light_datas_.resize(spot_light_count);
    size_t direct_light_count = direct_lights_.size();
    direct_light_datas_.resize(direct_light_count);

    for (size_t i = 0; i < point_light_count; i++) {
        point_lights_[i]->UpdateShaderData(point_light_datas_[i]);
    }

    for (size_t i = 0; i < spot_light_count; i++) {
        spot_lights_[i]->UpdateShaderData(spot_light_datas_[i]);
    }

    for (size_t i = 0; i < direct_light_count; i++) {
        direct_lights_[i]->UpdateShaderData(direct_light_datas_[i]);
    }

    if (direct_light_count) {
        shader_data.AddDefine(DIRECT_LIGHT_COUNT + std::to_string(direct_light_count));
        shader_data.SetData(LightManager::direct_light_property_, direct_light_datas_);
    } else {
        shader_data.RemoveDefine(DIRECT_LIGHT_COUNT);
    }

    if (point_light_count) {
        shader_data.AddDefine(POINT_LIGHT_COUNT + std::to_string(point_light_count));
        shader_data.SetData(LightManager::point_light_property_, point_light_datas_);
    } else {
        shader_data.RemoveDefine(POINT_LIGHT_COUNT);
    }

    if (spot_light_count) {
        shader_data.AddDefine(SPOT_LIGHT_COUNT + std::to_string(spot_light_count));
        shader_data.SetData(LightManager::spot_light_property_, spot_light_datas_);
    } else {
        shader_data.RemoveDefine(SPOT_LIGHT_COUNT);
    }
}

// MARK: - Forward Plus
void LightManager::Draw(CommandBuffer &command_buffer, RenderTarget &render_target) {
    UpdateShaderData(scene_->shader_data);

    size_t point_light_count = point_lights_.size();
    size_t spot_light_count = spot_lights_.size();
    if (point_light_count + spot_light_count > forward_plus_enable_min_count_) {
        scene_->shader_data.AddDefine("NEED_FORWARD_PLUS");

        bool update_bounds = false;

        if (forward_plus_uniforms_.x != camera_->FramebufferWidth() ||
            forward_plus_uniforms_.y != camera_->FramebufferHeight()) {
            update_bounds = true;
            forward_plus_uniforms_.x = camera_->FramebufferWidth();
            forward_plus_uniforms_.y = camera_->FramebufferHeight();
        }
        forward_plus_uniforms_.z = camera_->NearClipPlane();
        forward_plus_uniforms_.w = camera_->FarClipPlane();
        scene_->shader_data.SetData(forward_plus_prop_, forward_plus_uniforms_);

        // Reset the light offset counter to 0 before populating the light clusters.
        uint32_t empty = 0;
        cluster_lights_buffer_->Update(&empty, sizeof(uint32_t));
        if (update_bounds) {
            cluster_bounds_compute_->Draw(command_buffer, render_target);
        }
        cluster_lights_compute_->Draw(command_buffer, render_target);
    }
}

}  // namespace vox
