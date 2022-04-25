//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rendering/render_pipeline.h"
#include "lighting/point_light.h"
//#include "shadow_subpass.h"
#include "singleton.h"

namespace vox {
class ShadowManager : public Singleton<ShadowManager> {
public:
    static constexpr uint32_t shadow_map_cascade_count_ = 4;
    static constexpr uint32_t max_shadow_ = 10;
    static constexpr uint32_t max_cube_shadow_ = 5;
    static constexpr uint32_t shadow_map_resolution_ = 4000;
    static constexpr VkFormat shadow_map_format_ = VK_FORMAT_D32_SFLOAT;
    
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
    
    static uint32_t shadow_count();
    
    static uint32_t cube_shadow_count();
    
    static ShadowManager &get_singleton();
    
    static ShadowManager *get_singleton_ptr();
    
public:
    ShadowManager(Scene *scene, Camera *camera);
    
    float cascade_split_lambda();
    
    void set_cascade_split_lambda(float value);
    
    void draw(CommandBuffer &command_buffer);
    
private:
    void draw_spot_shadow_map(CommandBuffer &command_buffer);
    
    void draw_direct_shadow_map(CommandBuffer &command_buffer);
    
    void draw_point_shadow_map(CommandBuffer &command_buffer);
    
    static void update_spot_shadow(SpotLight *light, ShadowManager::ShadowData &shadow_data);
    
    /*
     * Calculate frustum split depths and matrices for the shadow map cascades
     * Based on https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
     */
    void update_cascades_shadow(DirectLight *light, ShadowManager::ShadowData &shadow_data);
    
    void update_point_shadow(PointLight *light, ShadowManager::CubeShadowData &shadow_data);
    
private:
    Scene *scene_{nullptr};
    Camera *camera_{nullptr};
    
    std::unique_ptr<RenderPipeline> render_pipeline_{nullptr};
    
    float cascade_split_lambda_ = 0.5f;
    
    static uint32_t shadow_count_;
    std::vector<std::shared_ptr<Image>> shadow_maps_{};
    std::shared_ptr<Image> packed_texture_{nullptr};
    const std::string shadow_map_prop_;
    const std::string shadow_sampler_prop_;
    const std::string shadow_data_prop_;
    std::array<ShadowManager::ShadowData, ShadowManager::max_shadow_> shadow_datas_{};
    
    static uint32_t cube_shadow_count_;
    std::vector<std::shared_ptr<Image>> cube_shadow_maps_{};
    std::shared_ptr<Image> packed_cube_texture_{nullptr};
    const std::string cube_shadow_map_prop_;
    const std::string cube_shadow_sampler_prop_;
    const std::string cube_shadow_data_prop_;
    std::array<ShadowManager::CubeShadowData, ShadowManager::max_cube_shadow_> cube_shadow_datas_{};
    
    uint32_t num_ofdraw_call_ = 0;
    
    const std::array<std::pair<Vector3F, Vector3F>, 6> cube_map_direction_ = {
        std::make_pair(Vector3F(10, 0, 0), Vector3F(0, 1, 0)),
        std::make_pair(Vector3F(-10, 0, 0), Vector3F(0, 1, 0)),
        std::make_pair(Vector3F(0, 10, 0), Vector3F(1, 0, 0)),
        std::make_pair(Vector3F(0, -10, 0), Vector3F(1, 0, 0)),
        std::make_pair(Vector3F(0, 0, 10), Vector3F(0, 1, 0)),
        std::make_pair(Vector3F(0, 0, -10), Vector3F(0, 1, 0)),
    };
    
    const std::array<Vector4F, shadow_map_cascade_count_> viewport_ = {
        Vector4F(0, 0, shadow_map_resolution_ / 2, shadow_map_resolution_ / 2),
        Vector4F(shadow_map_resolution_ / 2, 0, shadow_map_resolution_ / 2, shadow_map_resolution_ / 2),
        Vector4F(0, shadow_map_resolution_ / 2, shadow_map_resolution_ / 2, shadow_map_resolution_ / 2),
        Vector4F(shadow_map_resolution_ / 2, shadow_map_resolution_ / 2, shadow_map_resolution_ / 2, shadow_map_resolution_ / 2),
    };
};

template<> inline ShadowManager *Singleton<ShadowManager>::ms_singleton_{nullptr};

}
