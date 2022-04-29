//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "helpers.h"
#include "core/descriptor_pool.h"
#include "core/descriptor_set.h"
#include "core/descriptor_set_layout.h"
#include "core/framebuffer.h"
#include "core/pipeline.h"
#include "core/sampler.h"
#include "resource_record.h"
#include "resource_replay.h"

namespace vox {
class Device;

namespace core {
class ImageView;
}

/**
 * @brief Struct to hold the internal state of the Resource Cache
 *
 */
struct ResourceCacheState {
    std::unordered_map<std::size_t, ShaderModule> shader_modules;
    
    std::unordered_map<std::size_t, PipelineLayout> pipeline_layouts;
    
    std::unordered_map<std::size_t, DescriptorSetLayout> descriptor_set_layouts;
    
    std::unordered_map<std::size_t, DescriptorPool> descriptor_pools;
    
    std::unordered_map<std::size_t, RenderPass> render_passes;
    
    std::unordered_map<std::size_t, GraphicsPipeline> graphics_pipelines;
    
    std::unordered_map<std::size_t, ComputePipeline> compute_pipelines;
    
    std::unordered_map<std::size_t, DescriptorSet> descriptor_sets;
    
    std::unordered_map<std::size_t, Framebuffer> framebuffers;
    
    std::unordered_map<std::size_t, core::Sampler> samplers;
};

/**
 * @brief Cache all sorts of Vulkan objects specific to a Vulkan device.
 * Supports serialization and deserialization of cached resources.
 * There is only one cache for all these objects, with several unordered_map of hash indices
 * and objects. For every object requested, there is a templated version on request_resource.
 * Some objects may need building if they are not found in the cache.
 *
 * The resource cache is also linked with ResourceRecord and ResourceReplay. Replay can warm-up
 * the cache on app startup by creating all necessary objects.
 * The cache holds pointers to objects and has a mapping from such pointers to hashes.
 * It can only be destroyed in bulk, single elements cannot be removed.
 */
class ResourceCache {
public:
    explicit ResourceCache(Device &device);
    
    ResourceCache(const ResourceCache &) = delete;
    
    ResourceCache(ResourceCache &&) = delete;
    
    ResourceCache &operator=(const ResourceCache &) = delete;
    
    ResourceCache &operator=(ResourceCache &&) = delete;
    
    void warmup(const std::vector<uint8_t> &data);
    
    std::vector<uint8_t> serialize();
    
    void set_pipeline_cache(VkPipelineCache pipeline_cache);
    
    ShaderModule &request_shader_module(VkShaderStageFlagBits stage, const ShaderSource &glsl_source,
                                        const ShaderVariant &shader_variant = {});
    
    PipelineLayout &request_pipeline_layout(const std::vector<ShaderModule *> &shader_modules);
    
    DescriptorSetLayout &request_descriptor_set_layout(uint32_t set_index,
                                                       const std::vector<ShaderModule *> &shader_modules,
                                                       const std::vector<ShaderResource> &set_resources);
    
    GraphicsPipeline &request_graphics_pipeline(PipelineState &pipeline_state);
    
    ComputePipeline &request_compute_pipeline(PipelineState &pipeline_state);
    
    DescriptorSet &request_descriptor_set(DescriptorSetLayout &descriptor_set_layout,
                                          const BindingMap<VkDescriptorBufferInfo> &buffer_infos,
                                          const BindingMap<VkDescriptorImageInfo> &image_infos);
    
    RenderPass &request_render_pass(const std::vector<Attachment> &attachments,
                                    const std::vector<LoadStoreInfo> &load_store_infos,
                                    const std::vector<SubpassInfo> &subpasses);
    
    Framebuffer &request_framebuffer(const RenderTarget &render_target,
                                     const RenderPass &render_pass);
    
    core::Sampler &request_sampler(const VkSamplerCreateInfo& info);
    
    void clear_pipelines();
    
    /// @brief Update those descriptor sets referring to old views
    /// @param old_views Old image views referred by descriptor sets
    /// @param new_views New image views to be referred
    void update_descriptor_sets(const std::vector<core::ImageView> &old_views,
                                const std::vector<core::ImageView> &new_views);
    
    void clear_framebuffers();
    
    void clear();
    
    const ResourceCacheState &get_internal_state() const;
    
private:
    Device &device_;
    
    ResourceRecord recorder_;
    
    ResourceReplay replayer_;
    
    VkPipelineCache pipeline_cache_{VK_NULL_HANDLE};
    
    ResourceCacheState state_;
    
    std::mutex descriptor_set_mutex_;
    
    std::mutex pipeline_layout_mutex_;
    
    std::mutex shader_module_mutex_;
    
    std::mutex descriptor_set_layout_mutex_;
    
    std::mutex graphics_pipeline_mutex_;
    
    std::mutex render_pass_mutex_;
    
    std::mutex compute_pipeline_mutex_;
    
    std::mutex framebuffer_mutex_;
    
    std::mutex sampler_mutex_;
};

}        // namespace vox
