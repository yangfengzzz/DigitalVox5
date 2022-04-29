//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "resource_cache.h"

#include "resource_caching.h"
#include "core/device.h"

namespace vox {
namespace {
template<class T, class... A>
T &request_resource(Device &device, ResourceRecord &recorder, std::mutex &resource_mutex,
                    std::unordered_map<std::size_t, T> &resources, A &... args) {
    std::lock_guard<std::mutex> guard(resource_mutex);
    
    auto &res = request_resource(device, &recorder, resources, args...);
    
    return res;
}
}        // namespace

ResourceCache::ResourceCache(Device &device) :
device_{device} {
}

void ResourceCache::warmup(const std::vector<uint8_t> &data) {
    recorder_.set_data(data);
    
    replayer_.play(*this, recorder_);
}

std::vector<uint8_t> ResourceCache::serialize() {
    return recorder_.get_data();
}

void ResourceCache::set_pipeline_cache(VkPipelineCache new_pipeline_cache) {
    pipeline_cache_ = new_pipeline_cache;
}

ShaderModule &ResourceCache::request_shader_module(VkShaderStageFlagBits stage, const ShaderSource &glsl_source,
                                                   const ShaderVariant &shader_variant) {
    std::string entry_point{"main"};
    return request_resource(device_, recorder_, shader_module_mutex_, state_.shader_modules, stage, glsl_source,
                            entry_point, shader_variant);
}

PipelineLayout &ResourceCache::request_pipeline_layout(const std::vector<ShaderModule *> &shader_modules) {
    return request_resource(device_, recorder_, pipeline_layout_mutex_, state_.pipeline_layouts, shader_modules);
}

DescriptorSetLayout &ResourceCache::request_descriptor_set_layout(const uint32_t set_index,
                                                                  const std::vector<ShaderModule *> &shader_modules,
                                                                  const std::vector<ShaderResource> &set_resources) {
    return request_resource(device_, recorder_, descriptor_set_layout_mutex_, state_.descriptor_set_layouts, set_index,
                            shader_modules, set_resources);
}

GraphicsPipeline &ResourceCache::request_graphics_pipeline(PipelineState &pipeline_state) {
    return request_resource(device_, recorder_, graphics_pipeline_mutex_, state_.graphics_pipelines, pipeline_cache_,
                            pipeline_state);
}

ComputePipeline &ResourceCache::request_compute_pipeline(PipelineState &pipeline_state) {
    return request_resource(device_, recorder_, compute_pipeline_mutex_, state_.compute_pipelines, pipeline_cache_,
                            pipeline_state);
}

DescriptorSet &ResourceCache::request_descriptor_set(DescriptorSetLayout &descriptor_set_layout,
                                                     const BindingMap<VkDescriptorBufferInfo> &buffer_infos,
                                                     const BindingMap<VkDescriptorImageInfo> &image_infos) {
    auto &descriptor_pool = request_resource(device_, recorder_, descriptor_set_mutex_, state_.descriptor_pools,
                                             descriptor_set_layout);
    return request_resource(device_, recorder_, descriptor_set_mutex_, state_.descriptor_sets, descriptor_set_layout,
                            descriptor_pool, buffer_infos, image_infos);
}

RenderPass &ResourceCache::request_render_pass(const std::vector<Attachment> &attachments,
                                               const std::vector<LoadStoreInfo> &load_store_infos,
                                               const std::vector<SubpassInfo> &subpasses) {
    return request_resource(device_, recorder_, render_pass_mutex_, state_.render_passes, attachments, load_store_infos,
                            subpasses);
}

Framebuffer &ResourceCache::request_framebuffer(const RenderTarget &render_target, const RenderPass &render_pass) {
    return request_resource(device_, recorder_, framebuffer_mutex_, state_.framebuffers, render_target, render_pass);
}

core::Sampler &ResourceCache::request_sampler(const VkSamplerCreateInfo& info) {
    return request_resource(device_, recorder_, sampler_mutex_, state_.samplers, info);
}

void ResourceCache::clear_pipelines() {
    state_.graphics_pipelines.clear();
    state_.compute_pipelines.clear();
}

void ResourceCache::update_descriptor_sets(const std::vector<core::ImageView> &old_views,
                                           const std::vector<core::ImageView> &new_views) {
    // Find descriptor sets referring to the old image view
    std::vector<VkWriteDescriptorSet> set_updates;
    std::set<size_t> matches;
    
    for (size_t i = 0; i < old_views.size(); ++i) {
        auto &old_view = old_views[i];
        auto &new_view = new_views[i];
        
        for (auto &kd_pair : state_.descriptor_sets) {
            auto &key = kd_pair.first;
            auto &descriptor_set = kd_pair.second;
            
            auto &image_infos = descriptor_set.get_image_infos();
            
            for (auto &ba_pair : image_infos) {
                auto &binding = ba_pair.first;
                auto &array = ba_pair.second;
                
                for (auto &ai_pair : array) {
                    auto &array_element = ai_pair.first;
                    auto &image_info = ai_pair.second;
                    
                    if (image_info.imageView == old_view.get_handle()) {
                        // Save key to remove old descriptor set
                        matches.insert(key);
                        
                        // Update image info with new view
                        image_info.imageView = new_view.get_handle();
                        
                        // Save struct for writing the update later
                        {
                            VkWriteDescriptorSet write_descriptor_set{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
                            
                            if (auto binding_info = descriptor_set.get_layout().get_layout_binding(binding)) {
                                write_descriptor_set.dstBinding = binding;
                                write_descriptor_set.descriptorType = binding_info->descriptorType;
                                write_descriptor_set.pImageInfo = &image_info;
                                write_descriptor_set.dstSet = descriptor_set.get_handle();
                                write_descriptor_set.dstArrayElement = array_element;
                                write_descriptor_set.descriptorCount = 1;
                                
                                set_updates.push_back(write_descriptor_set);
                            } else {
                                LOGE("Shader layout set does not use image binding at #{}", binding)
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (!set_updates.empty()) {
        vkUpdateDescriptorSets(device_.get_handle(), to_u32(set_updates.size()), set_updates.data(),
                               0, nullptr);
    }
    
    // Delete old entries (moved out descriptor sets)
    for (auto &match : matches) {
        // Move out of the map
        auto it = state_.descriptor_sets.find(match);
        auto descriptor_set = std::move(it->second);
        state_.descriptor_sets.erase(match);
        
        // Generate new key
        size_t new_key = 0U;
        hash_param(new_key, descriptor_set.get_layout(), descriptor_set.get_buffer_infos(),
                   descriptor_set.get_image_infos());
        
        // Add (key, resource) to the cache
        state_.descriptor_sets.emplace(new_key, std::move(descriptor_set));
    }
}

void ResourceCache::clear_framebuffers() {
    state_.framebuffers.clear();
}

void ResourceCache::clear() {
    state_.shader_modules.clear();
    state_.pipeline_layouts.clear();
    state_.descriptor_sets.clear();
    state_.descriptor_set_layouts.clear();
    state_.render_passes.clear();
    state_.samplers.clear();
    clear_pipelines();
    clear_framebuffers();
}

const ResourceCacheState &ResourceCache::get_internal_state() const {
    return state_;
}

}        // namespace vox
