//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "point_light.h"
#include "spot_light.h"
#include "direct_light.h"
#include "shader/shader_data.h"
//#include "rendering/compute_pass.h"
#include "singleton.h"

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
    static constexpr std::array<uint32_t, 3> dispatch_size_ = {
        tile_count_[0] / workgroup_size_[0],
        tile_count_[1] / workgroup_size_[1],
        tile_count_[2] / workgroup_size_[2]
    };
    
    // Each cluster tracks up to MAX_LIGHTS_PER_CLUSTER light indices (ints) and one light count.
    // This limitation should be able to go away when we have atomic methods in Vulkan.
    static constexpr uint32_t max_lights_per_cluster_ = 50;
    
    static LightManager &get_singleton();
    
    static LightManager *get_singleton_ptr();
    
    explicit LightManager(Scene *scene);
    
    void set_camera(Camera *camera);
    
    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attach_point_light(PointLight *light);
    
    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detach_point_light(PointLight *light);
    
    [[nodiscard]] const std::vector<PointLight *> &point_lights() const;
    
public:
    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attach_spot_light(SpotLight *light);
    
    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detach_spot_light(SpotLight *light);
    
    [[nodiscard]] const std::vector<SpotLight *> &spot_lights() const;
    
public:
    /**
     * Register a light object to the current scene.
     * @param light direct light
     */
    void attach_direct_light(DirectLight *light);
    
    /**
     * Remove a light object from the current scene.
     * @param light direct light
     */
    void detach_direct_light(DirectLight *light);
    
    [[nodiscard]] const std::vector<DirectLight *> &direct_lights() const;
    
public:
    [[nodiscard]] bool enable_forward_plus() const;
    
    void draw(CommandBuffer &command_buffer);
    
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
    
    void update_shader_data(ShaderData &shader_data);
    
private:
    bool enable_forward_plus_{false};
    
    struct ProjectionUniforms {
        Matrix4x4F matrix;
        Matrix4x4F inverse_matrix;
        std::array<uint32_t, 2> output_size;
        float z_near;
        float z_far;
        Matrix4x4F view_matrix;
    };
    ProjectionUniforms forward_plus_uniforms_;
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
    //    std::unique_ptr<ComputePass> _clusterBoundsCompute{nullptr};
    //    std::unique_ptr<ComputePass> _clusterLightsCompute{nullptr};
};

template<> inline LightManager *Singleton<LightManager>::ms_singleton_{nullptr};

}
