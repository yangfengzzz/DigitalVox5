//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/singleton.h"
#include "vox.render/lighting/direct_light.h"
#include "vox.render/lighting/point_light.h"
#include "vox.render/lighting/spot_light.h"
#include "vox.render/rendering/postprocessing_computepass.h"
#include "vox.render/rendering/postprocessing_pipeline.h"
#include "vox.render/shader/shader_data.h"

namespace vox {
/**
 * Light Manager.
 */
class LightManager : public Singleton<LightManager> {
public:
    static constexpr uint32_t forward_plus_enable_min_count_ = 20;
    static constexpr std::array<uint32_t, 3> tile_count_ = {32, 18, 48};
    static constexpr uint32_t total_tiles_ = tile_count_[0] * tile_count_[1] * tile_count_[2];

    static constexpr std::array<uint32_t, 3> workgroup_size_ = {4, 2, 4};
    static constexpr std::array<uint32_t, 3> dispatch_size_ = {tile_count_[0] / workgroup_size_[0],
                                                               tile_count_[1] / workgroup_size_[1],
                                                               tile_count_[2] / workgroup_size_[2]};

    // Each cluster tracks up to MAX_LIGHTS_PER_CLUSTER light indices (ints) and one light count.
    // This limitation should be able to go away when we have atomic methods in Vulkan.
    static constexpr uint32_t max_lights_per_cluster_ = 50;

    static LightManager &GetSingleton();

    static LightManager *GetSingletonPtr();

    explicit LightManager(Scene *scene, RenderContext &render_context);

    void SetCamera(Camera *camera);

    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void AttachPointLight(PointLight *light);

    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void DetachPointLight(PointLight *light);

    [[nodiscard]] const std::vector<PointLight *> &PointLights() const;

public:
    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void AttachSpotLight(SpotLight *light);

    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void DetachSpotLight(SpotLight *light);

    [[nodiscard]] const std::vector<SpotLight *> &SpotLights() const;

public:
    /**
     * Register a light object to the current scene.
     * @param light direct light
     */
    void AttachDirectLight(DirectLight *light);

    /**
     * Remove a light object from the current scene.
     * @param light direct light
     */
    void DetachDirectLight(DirectLight *light);

    [[nodiscard]] const std::vector<DirectLight *> &DirectLights() const;

public:
    void Draw(CommandBuffer &command_buffer, RenderTarget &render_target);

private:
    Scene *scene_{nullptr};
    Camera *camera_{nullptr};

    std::vector<PointLight *> point_lights_;
    std::vector<PointLight::PointLightData> point_light_datas_;
    const std::string point_light_property_;

    std::vector<SpotLight *> spot_lights_;
    std::vector<SpotLight::SpotLightData> spot_light_datas_;
    const std::string spot_light_property_;

    std::vector<DirectLight *> direct_lights_;
    std::vector<DirectLight::DirectLightData> direct_light_datas_;
    const std::string direct_light_property_;

    void UpdateShaderData(ShaderData &shader_data);

private:
    Vector4F forward_plus_uniforms_;
    const std::string forward_plus_prop_;

    struct ClusterBounds {
        Vector3F min_aabb;
        float pad_1;
        Vector3F max_aabb;
        float pad_2;
    };
    struct Clusters {
        std::array<ClusterBounds, total_tiles_> bounds;
    };
    const std::string clusters_prop_;
    std::unique_ptr<core::Buffer> clusters_buffer_;

    struct ClusterLights {
        uint32_t offset;
        uint32_t point_count;
        uint32_t spot_count;
    };
    struct ClusterLightGroup {
        uint32_t offset;
        std::array<ClusterLights, total_tiles_> lights;
        std::array<uint32_t, max_lights_per_cluster_ * total_tiles_> indices;
    };
    const std::string cluster_lights_prop_;
    std::unique_ptr<core::Buffer> cluster_lights_buffer_;

    ShaderData shader_data_;
    PostProcessingComputePass *bounds_pass_{nullptr};
    std::unique_ptr<PostProcessingPipeline> cluster_bounds_compute_{nullptr};
    PostProcessingComputePass *lights_pass_{nullptr};
    std::unique_ptr<PostProcessingPipeline> cluster_lights_compute_{nullptr};
};

template <>
inline LightManager *Singleton<LightManager>::ms_singleton{nullptr};

}  // namespace vox
