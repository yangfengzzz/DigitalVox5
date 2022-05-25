//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/singleton.h"
#include "vox.render/lighting/point_light.h"
#include "vox.render/rendering/render_pipeline.h"
#include "vox.render/shadow/shadow_subpass.h"

namespace vox {
class ShadowManager : public Singleton<ShadowManager> {
public:
    static constexpr uint32_t shadow_map_cascade_count_ = 4;
    static constexpr uint32_t max_shadow_ = 10;
    static constexpr uint32_t max_cube_shadow_ = 5;
    static constexpr uint32_t shadow_map_resolution_ = 4000;

    struct ShadowData {
        /**
         * Shadow bias.
         */
        float bias = 0.005;

        /**
         * Shadow intensity, the larger the value, the clearer and darker the shadow.
         */
        float intensity = 0.2;

        /**
         * Pixel range used for shadow PCF interpolation.
         */
        float radius = 1;

        /** Alignment */
        float dump = 0;

        /**
         * Light view projection matrix.(cascade)
         */
        Matrix4x4F vp[4];

        /**
         * Light cascade depth.
         */
        Vector4F cascade_splits;
    };

    struct CubeShadowData {
        /**
         * Shadow bias.
         */
        float bias = 0.005;

        /**
         * Shadow intensity, the larger the value, the clearer and darker the shadow.
         */
        float intensity = 0.2;

        /**
         * Pixel range used for shadow PCF interpolation.
         */
        float radius = 1;

        /** Alignment */
        float dump = 0;

        /**
         * Light view projection matrix.(cascade)
         */
        Matrix4x4F vp[6];

        Vector4F light_pos;
    };

    uint32_t ShadowCount();

    static uint32_t CubeShadowCount();

    static ShadowManager &GetSingleton();

    static ShadowManager *GetSingletonPtr();

public:
    ShadowManager(Device &device, RenderContext &render_context, Scene *scene, Camera *camera);

    [[nodiscard]] float CascadeSplitLambda() const;

    void SetCascadeSplitLambda(float value);

    void Draw(CommandBuffer &command_buffer);

private:
    void DrawSpotShadowMap(CommandBuffer &command_buffer);

    void DrawDirectShadowMap(CommandBuffer &command_buffer);

    void DrawPointShadowMap(CommandBuffer &command_buffer);

    static void UpdateSpotShadow(SpotLight *light, ShadowManager::ShadowData &shadow_data);

    /*
     * Calculate frustum split depths and matrices for the shadow map cascades
     * Based on https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
     */
    void UpdateCascadesShadow(DirectLight *light, ShadowManager::ShadowData &shadow_data);

    void UpdatePointShadow(PointLight *light, ShadowManager::CubeShadowData &shadow_data);

    std::unique_ptr<RenderTarget> CreateShadowRenderTarget(uint32_t size);

    void RecordShadowPassImageMemoryBarrier(CommandBuffer &command_buffer, RenderTarget &render_target);

private:
    Device &device_;
    RenderContext &render_context_;

    Scene *scene_{nullptr};
    Camera *camera_{nullptr};

    std::unique_ptr<RenderPipeline> render_pipeline_{nullptr};
    ShadowSubpass *shadow_subpass_{nullptr};

    float cascade_split_lambda_ = 0.5f;

    VkSamplerCreateInfo sampler_create_info_;
    std::unique_ptr<core::Sampler> sampler_{nullptr};

    std::vector<RenderTarget *> used_shadow_;
    std::vector<std::vector<std::unique_ptr<RenderTarget>>> shadow_maps_{};
    const std::string shadow_map_prop_;
    const std::string shadow_data_prop_;
    std::array<ShadowManager::ShadowData, ShadowManager::max_shadow_> shadow_datas_{};

    static uint32_t cube_shadow_count_;
    std::vector<std::vector<std::unique_ptr<RenderTarget>>> cube_shadow_maps_{};
    std::shared_ptr<Texture> packed_cube_texture_{nullptr};
    const std::string cube_shadow_map_prop_;
    const std::string cube_shadow_data_prop_;
    std::array<ShadowManager::CubeShadowData, ShadowManager::max_cube_shadow_> cube_shadow_datas_{};

    const std::array<std::pair<Vector3F, Vector3F>, 6> cube_map_direction_ = {
            std::make_pair(Vector3F(10, 0, 0), Vector3F(0, 1, 0)),
            std::make_pair(Vector3F(-10, 0, 0), Vector3F(0, 1, 0)),
            std::make_pair(Vector3F(0, 10, 0), Vector3F(1, 0, 0)),
            std::make_pair(Vector3F(0, -10, 0), Vector3F(1, 0, 0)),
            std::make_pair(Vector3F(0, 0, 10), Vector3F(0, 1, 0)),
            std::make_pair(Vector3F(0, 0, -10), Vector3F(0, 1, 0)),
    };

    std::array<VkViewport, shadow_map_cascade_count_> viewport_{};
};

template <>
inline ShadowManager *Singleton<ShadowManager>::ms_singleton{nullptr};

}  // namespace vox
