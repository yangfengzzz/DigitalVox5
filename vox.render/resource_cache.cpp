//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/resource_cache.h"

#include "vox.render/core/device.h"
#include "vox.render/resource_caching.h"

namespace vox {
namespace {
template <class T, class... A>
T &RequestResource(Device &device,
                   ResourceRecord &recorder,
                   std::mutex &resource_mutex,
                   std::unordered_map<std::size_t, T> &resources,
                   A &...args) {
    std::lock_guard<std::mutex> guard(resource_mutex);

    auto &res = RequestResource(device, &recorder, resources, args...);

    return res;
}
}  // namespace

ResourceCache::ResourceCache(Device &device) : device_{device} {}

void ResourceCache::WarmUp(const std::vector<uint8_t> &data) {
    recorder_.SetData(data);

    replayer_.Play(*this, recorder_);
}

std::vector<uint8_t> ResourceCache::Serialize() { return recorder_.GetData(); }

void ResourceCache::SetPipelineCache(VkPipelineCache pipeline_cache) { pipeline_cache_ = pipeline_cache; }

ShaderModule &ResourceCache::RequestShaderModule(VkShaderStageFlagBits stage,
                                                 const ShaderSource &glsl_source,
                                                 const ShaderVariant &shader_variant) {
    std::string entry_point{"main"};
    return RequestResource(device_, recorder_, shader_module_mutex_, state_.shader_modules, stage, glsl_source,
                           entry_point, shader_variant);
}

PipelineLayout &ResourceCache::RequestPipelineLayout(const std::vector<ShaderModule *> &shader_modules) {
    return RequestResource(device_, recorder_, pipeline_layout_mutex_, state_.pipeline_layouts, shader_modules);
}

DescriptorSetLayout &ResourceCache::RequestDescriptorSetLayout(uint32_t set_index,
                                                               const std::vector<ShaderModule *> &shader_modules,
                                                               const std::vector<ShaderResource> &set_resources) {
    return RequestResource(device_, recorder_, descriptor_set_layout_mutex_, state_.descriptor_set_layouts, set_index,
                           shader_modules, set_resources);
}

GraphicsPipeline &ResourceCache::RequestGraphicsPipeline(PipelineState &pipeline_state) {
    return RequestResource(device_, recorder_, graphics_pipeline_mutex_, state_.graphics_pipelines, pipeline_cache_,
                           pipeline_state);
}

ComputePipeline &ResourceCache::RequestComputePipeline(PipelineState &pipeline_state) {
    return RequestResource(device_, recorder_, compute_pipeline_mutex_, state_.compute_pipelines, pipeline_cache_,
                           pipeline_state);
}

DescriptorSet &ResourceCache::RequestDescriptorSet(DescriptorSetLayout &descriptor_set_layout,
                                                   const BindingMap<VkDescriptorBufferInfo> &buffer_infos,
                                                   const BindingMap<VkDescriptorImageInfo> &image_infos) {
    auto &descriptor_pool =
            RequestResource(device_, recorder_, descriptor_set_mutex_, state_.descriptor_pools, descriptor_set_layout);
    return RequestResource(device_, recorder_, descriptor_set_mutex_, state_.descriptor_sets, descriptor_set_layout,
                           descriptor_pool, buffer_infos, image_infos);
}

RenderPass &ResourceCache::RequestRenderPass(const std::vector<Attachment> &attachments,
                                             const std::vector<LoadStoreInfo> &load_store_infos,
                                             const std::vector<SubpassInfo> &subpasses) {
    return RequestResource(device_, recorder_, render_pass_mutex_, state_.render_passes, attachments, load_store_infos,
                           subpasses);
}

Framebuffer &ResourceCache::RequestFramebuffer(const RenderTarget &render_target, const RenderPass &render_pass) {
    return RequestResource(device_, recorder_, framebuffer_mutex_, state_.framebuffers, render_target, render_pass);
}

core::Sampler &ResourceCache::RequestSampler(const VkSamplerCreateInfo &info) {
    return RequestResource(device_, recorder_, sampler_mutex_, state_.samplers, info);
}

void ResourceCache::ClearPipelines() {
    state_.graphics_pipelines.clear();
    state_.compute_pipelines.clear();
}

void ResourceCache::UpdateDescriptorSets(const std::vector<core::ImageView> &old_views,
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

            auto &image_infos = descriptor_set.GetImageInfos();

            for (auto &ba_pair : image_infos) {
                auto &binding = ba_pair.first;
                auto &array = ba_pair.second;

                for (auto &ai_pair : array) {
                    auto &array_element = ai_pair.first;
                    auto &image_info = ai_pair.second;

                    if (image_info.imageView == old_view.GetHandle()) {
                        // Save key to remove old descriptor set
                        matches.insert(key);

                        // Update image info with new view
                        image_info.imageView = new_view.GetHandle();

                        // Save struct for writing the update later
                        {
                            VkWriteDescriptorSet write_descriptor_set{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

                            if (auto binding_info = descriptor_set.GetLayout().GetLayoutBinding(binding)) {
                                write_descriptor_set.dstBinding = binding;
                                write_descriptor_set.descriptorType = binding_info->descriptorType;
                                write_descriptor_set.pImageInfo = &image_info;
                                write_descriptor_set.dstSet = descriptor_set.GetHandle();
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
        vkUpdateDescriptorSets(device_.GetHandle(), utility::ToU32(set_updates.size()), set_updates.data(), 0, nullptr);
    }

    // Delete old entries (moved out descriptor sets)
    for (auto &match : matches) {
        // Move out of the map
        auto it = state_.descriptor_sets.find(match);
        auto descriptor_set = std::move(it->second);
        state_.descriptor_sets.erase(match);

        // Generate new key
        size_t new_key = 0U;
        HashParam(new_key, descriptor_set.GetLayout(), descriptor_set.GetBufferInfos(), descriptor_set.GetImageInfos());

        // Add (key, resource) to the cache
        state_.descriptor_sets.emplace(new_key, std::move(descriptor_set));
    }
}

void ResourceCache::ClearFramebuffers() { state_.framebuffers.clear(); }

void ResourceCache::Clear() {
    state_.shader_modules.clear();
    state_.pipeline_layouts.clear();
    state_.descriptor_sets.clear();
    state_.descriptor_set_layouts.clear();
    state_.render_passes.clear();
    state_.samplers.clear();
    ClearPipelines();
    ClearFramebuffers();
}

const ResourceCacheState &ResourceCache::GetInternalState() const { return state_; }

}  // namespace vox
