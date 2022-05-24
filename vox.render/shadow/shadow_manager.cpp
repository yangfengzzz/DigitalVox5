//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/shadow_manager.h"

#include "vox.math/matrix_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/lighting/light_manager.h"
#include "vox.render/texture_manager.h"

namespace vox {
uint32_t ShadowManager::cube_shadow_count_ = 0;
uint32_t ShadowManager::ShadowCount() { return static_cast<uint32_t>(used_shadow_.size()); }

uint32_t ShadowManager::CubeShadowCount() { return cube_shadow_count_; }

ShadowManager *ShadowManager::GetSingletonPtr() { return ms_singleton; }

ShadowManager &ShadowManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

ShadowManager::ShadowManager(Device &device, RenderContext &render_context, Scene *scene, Camera *camera)
    : device_(device),
      render_context_(render_context),
      scene_(scene),
      camera_(camera),
      shadow_map_prop_("shadowMap"),
      shadow_data_prop_("shadowData"),

      cube_shadow_map_prop_("cubeShadowMap"),
      cube_shadow_data_prop_("cubeShadowData"),
      sampler_create_info_{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO} {
    // Create a default sampler
    sampler_create_info_.magFilter = VK_FILTER_LINEAR;
    sampler_create_info_.minFilter = VK_FILTER_LINEAR;
    sampler_create_info_.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info_.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info_.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info_.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info_.mipLodBias = 0.0f;
    sampler_create_info_.compareEnable = VK_TRUE;
    sampler_create_info_.compareOp = VK_COMPARE_OP_LESS;
    sampler_create_info_.minLod = 0.0f;
    // Max level-of-detail should match mip level count
    sampler_create_info_.maxLod = 0.0f;
    sampler_create_info_.unnormalizedCoordinates = VK_FALSE;
    sampler_ = std::make_unique<core::Sampler>(device, sampler_create_info_);

    auto subpass = std::make_unique<ShadowSubpass>(render_context_, scene, camera);
    shadow_subpass_ = subpass.get();
    render_pipeline_ = std::make_unique<RenderPipeline>();
    render_pipeline_->AddSubpass(std::move(subpass));

    std::vector<VkClearValue> clear_value(1);
    clear_value[0].depthStencil = {1.0f, 255U};
    render_pipeline_->SetClearValue(clear_value);

    std::vector<LoadStoreInfo> load_store_info(1);
    load_store_info[0].load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
    render_pipeline_->SetLoadStore(load_store_info);

    auto offset = static_cast<float>(shadow_map_resolution_ / 2);
    viewport_[0] = {0.f, 0.f, offset, offset, 0.f, 1.f};
    viewport_[1] = {offset, 0.f, offset, offset, 0.f, 1.f};
    viewport_[2] = {0.f, offset, offset, offset, 0.f, 1.f};
    viewport_[3] = {offset, offset, offset, offset, 0.f, 1.f};
}

float ShadowManager::CascadeSplitLambda() const { return cascade_split_lambda_; }

void ShadowManager::SetCascadeSplitLambda(float value) { cascade_split_lambda_ = value; }

void ShadowManager::Draw(CommandBuffer &command_buffer) {
    used_shadow_.clear();
    DrawSpotShadowMap(command_buffer);
    DrawDirectShadowMap(command_buffer);
    if (!used_shadow_.empty()) {
        auto image =
                TextureManager::GetSingleton().PackedShadowMap(command_buffer, used_shadow_, shadow_map_resolution_);
        scene_->shader_data.SetSampledTexture(shadow_map_prop_, image->GetVkImageView(VK_IMAGE_VIEW_TYPE_2D_ARRAY),
                                              sampler_.get());
        scene_->shader_data.SetData(shadow_data_prop_, shadow_datas_);
    }

    cube_shadow_count_ = 0;
    DrawPointShadowMap(command_buffer);
    if (cube_shadow_count_) {
    }
}

void ShadowManager::DrawSpotShadowMap(CommandBuffer &command_buffer) {
    const auto &lights = LightManager::GetSingleton().SpotLights();
    for (const auto &light : lights) {
        size_t shadow_count = used_shadow_.size();
        if (light->EnableShadow() && shadow_count < max_shadow_) {
            UpdateSpotShadow(light, shadow_datas_[shadow_count]);

            RenderTarget *render_target{nullptr};
            if (shadow_count < shadow_maps_.size()) {
                render_target = shadow_maps_[shadow_count][render_context_.GetActiveFrameIndex()].get();
            } else {
                std::vector<std::unique_ptr<RenderTarget>> shadow_render_targets(
                        render_context_.GetRenderFrames().size());
                for (auto &shadow_render_target : shadow_render_targets) {
                    shadow_render_target = CreateShadowRenderTarget(shadow_map_resolution_);
                }
                render_target = shadow_render_targets[render_context_.GetActiveFrameIndex()].get();
                shadow_maps_.emplace_back(std::move(shadow_render_targets));
            }

            shadow_subpass_->SetViewProjectionMatrix(shadow_datas_[shadow_count].vp[0]);

            RecordShadowPassImageMemoryBarrier(command_buffer, *render_target);
            render_pipeline_->Draw(command_buffer, *render_target);
            command_buffer.EndRenderPass();
            used_shadow_.emplace_back(render_target);
        }
    }
}

void ShadowManager::DrawDirectShadowMap(CommandBuffer &command_buffer) {
    auto load_store = render_pipeline_->GetLoadStore();

    const auto &lights = LightManager::GetSingleton().DirectLights();
    for (const auto &light : lights) {
        size_t shadow_count = used_shadow_.size();
        if (light->EnableShadow() && shadow_count < max_shadow_) {
            UpdateCascadesShadow(light, shadow_datas_[shadow_count]);

            RenderTarget *render_target{nullptr};
            if (shadow_count < shadow_maps_.size()) {
                render_target = shadow_maps_[shadow_count][render_context_.GetActiveFrameIndex()].get();
            } else {
                std::vector<std::unique_ptr<RenderTarget>> shadow_render_targets(
                        render_context_.GetRenderFrames().size());
                for (auto &shadow_render_target : shadow_render_targets) {
                    shadow_render_target = CreateShadowRenderTarget(shadow_map_resolution_);
                }
                render_target = shadow_render_targets[render_context_.GetActiveFrameIndex()].get();
                shadow_maps_.emplace_back(std::move(shadow_render_targets));
            }

            for (int i = 0; i < shadow_map_cascade_count_; i++) {
                shadow_subpass_->SetViewProjectionMatrix(shadow_datas_[shadow_count].vp[i]);
                shadow_subpass_->SetViewport(viewport_[i]);

                if (i == 0) {
                    load_store[0].load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
                } else {
                    load_store[0].load_op = VK_ATTACHMENT_LOAD_OP_LOAD;
                }
                render_pipeline_->SetLoadStore(load_store);

                RecordShadowPassImageMemoryBarrier(command_buffer, *render_target);
                render_pipeline_->Draw(command_buffer, *render_target);
                command_buffer.EndRenderPass();
            }
            used_shadow_.emplace_back(render_target);
        }
    }
    load_store[0].load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
    render_pipeline_->SetLoadStore(load_store);
    shadow_subpass_->SetViewport(std::nullopt);
}

void ShadowManager::DrawPointShadowMap(CommandBuffer &command_buffer) {
    const auto &lights = LightManager::GetSingleton().PointLights();
    for (const auto &light : lights) {
        if (light->EnableShadow() && cube_shadow_count_ < max_cube_shadow_) {
            UpdatePointShadow(light, cube_shadow_datas_[cube_shadow_count_]);

            RenderTarget *render_target{nullptr};
            if (cube_shadow_count_ < cube_shadow_maps_.size()) {
                render_target = cube_shadow_maps_[cube_shadow_count_][render_context_.GetActiveFrameIndex()].get();
            } else {
                std::vector<std::unique_ptr<RenderTarget>> shadow_render_targets(
                        render_context_.GetRenderFrames().size());
                for (auto &shadow_render_target : shadow_render_targets) {
                    shadow_render_target = CreateShadowRenderTarget(shadow_map_resolution_);
                }
                render_target = shadow_render_targets[render_context_.GetActiveFrameIndex()].get();
                cube_shadow_maps_.emplace_back(std::move(shadow_render_targets));
            }

            for (const auto &i : cube_shadow_datas_[cube_shadow_count_].vp) {
                shadow_subpass_->SetViewProjectionMatrix(i);
                render_pipeline_->Draw(command_buffer, *render_target);
            }
            cube_shadow_count_++;
        }
    }
}

std::unique_ptr<RenderTarget> ShadowManager::CreateShadowRenderTarget(uint32_t size) {
    VkExtent3D extent{size, size, 1};

    core::Image depth_image{device_, extent, GetSuitableDepthFormat(device_.GetGpu().GetHandle()),
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            VMA_MEMORY_USAGE_GPU_ONLY};

    std::vector<core::Image> images;
    images.push_back(std::move(depth_image));
    return std::make_unique<RenderTarget>(std::move(images));
}

void ShadowManager::RecordShadowPassImageMemoryBarrier(CommandBuffer &command_buffer, RenderTarget &render_target) {
    auto &shadowmap = render_target.GetViews().at(0);

    ImageMemoryBarrier memory_barrier{};
    memory_barrier.old_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    memory_barrier.new_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    memory_barrier.src_access_mask = 0;
    memory_barrier.dst_access_mask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    memory_barrier.dst_stage_mask =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

    command_buffer.ImageMemoryBarrier(shadowmap, memory_barrier);
}

void ShadowManager::UpdateSpotShadow(SpotLight *light, ShadowManager::ShadowData &shadow_data) {
    shadow_data.radius = light->ShadowRadius();
    shadow_data.bias = light->ShadowBias();
    shadow_data.intensity = light->ShadowIntensity();

    auto view_matrix = light->GetEntity()->transform->WorldMatrix().inverse();
    auto proj_matrix = light->ShadowProjectionMatrix();
    shadow_data.vp[0] = proj_matrix * view_matrix;
    shadow_data.cascade_splits[0] = 1;
    shadow_data.cascade_splits[1] = -1;  // mark cascade with negative sign
}

void ShadowManager::UpdateCascadesShadow(DirectLight *light, ShadowManager::ShadowData &shadow_data) {
    shadow_data.radius = light->ShadowRadius();
    shadow_data.bias = light->ShadowBias();
    shadow_data.intensity = light->ShadowIntensity();

    std::array<float, shadow_map_cascade_count_> cascade_splits{};
    auto world_pos = light->GetEntity()->transform->WorldPosition();

    float near_clip = camera_->NearClipPlane();
    float far_clip = camera_->FarClipPlane();
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
            Point3F(-1.0f, 1.0f, 0.0f),  Point3F(1.0f, 1.0f, 0.0f),   Point3F(1.0f, -1.0f, 0.0f),
            Point3F(-1.0f, -1.0f, 0.0f), Point3F(-1.0f, 1.0f, 1.0f),  Point3F(1.0f, 1.0f, 1.0f),
            Point3F(1.0f, -1.0f, 1.0f),  Point3F(-1.0f, -1.0f, 1.0f),
    };

    // Project frustum corners into world space
    Matrix4x4F inv_cam = (camera_->ProjectionMatrix() * camera_->ViewMatrix()).inverse();
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

        auto light_mat = light->GetEntity()->transform->WorldMatrix();
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
        light->GetEntity()->transform->SetWorldPosition(center);

        float radius = max_dist / 2.f;
        Vector3F max_extents = Vector3F(radius, radius, radius);
        Vector3F min_extents = -max_extents;
        Matrix4x4F light_ortho_matrix =
                makeOrtho(min_extents.x, max_extents.x, min_extents.y, max_extents.y, 0.0f, max_z - min_z);

        // Store split distance and matrix in cascade
        shadow_data.cascade_splits[i] = (camera_->NearClipPlane() + split_dist * clip_range) * -1.0f;
        shadow_data.vp[i] = light_ortho_matrix * light->GetEntity()->transform->WorldMatrix().inverse();
        light->GetEntity()->transform->SetWorldPosition(world_pos);
        last_split_dist = cascade_splits[i];
    }
}

void ShadowManager::UpdatePointShadow(PointLight *light, ShadowManager::CubeShadowData &shadow_data) {
    shadow_data.radius = light->ShadowRadius();
    shadow_data.bias = light->ShadowBias();
    shadow_data.intensity = light->ShadowIntensity();

    auto proj_matrix = light->ShadowProjectionMatrix();
    auto world_pos = light->GetEntity()->transform->WorldPosition();
    shadow_data.light_pos = Vector4F(world_pos.x, world_pos.y, world_pos.z, 1.0);

    for (int i = 0; i < 6; i++) {
        light->GetEntity()->transform->LookAt(world_pos + cube_map_direction_[i].first, cube_map_direction_[i].second);
        auto view_matrix = light->GetEntity()->transform->WorldMatrix().inverse();
        shadow_data.vp[i] = proj_matrix * view_matrix;
    }
}

}  // namespace vox
