//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/descriptor_pool.h"
#include "vox.render/core/descriptor_set.h"
#include "vox.render/core/descriptor_set_layout.h"
#include "vox.render/core/framebuffer.h"
#include "vox.render/core/pipeline.h"
#include "vox.render/rendering/pipeline_state.h"
#include "vox.render/rendering/render_target.h"
#include "vox.render/resource_record.h"

inline bool operator==(const VkSamplerCreateInfo &x, const VkSamplerCreateInfo &y) {
    return x.magFilter == y.magFilter && x.minFilter == y.minFilter && x.mipmapMode == y.mipmapMode &&
           x.addressModeU == y.addressModeU && x.addressModeV == y.addressModeV && x.addressModeW == y.addressModeW &&
           x.mipLodBias == y.mipLodBias && x.anisotropyEnable == y.anisotropyEnable &&
           x.maxAnisotropy == y.maxAnisotropy && x.compareEnable == y.compareEnable && x.compareOp == y.compareOp &&
           x.minLod == y.minLod && x.maxLod == y.maxLod && x.borderColor == y.borderColor;
}

namespace std {
template <>
struct hash<VkSamplerCreateInfo> {
    std::size_t operator()(const VkSamplerCreateInfo &sampler) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, sampler.magFilter);
        vox::utility::hash_combine(result, sampler.minFilter);
        vox::utility::hash_combine(result, sampler.mipmapMode);
        vox::utility::hash_combine(result, sampler.addressModeU);
        vox::utility::hash_combine(result, sampler.addressModeV);
        vox::utility::hash_combine(result, sampler.addressModeW);
        vox::utility::hash_combine(result, sampler.mipLodBias);
        vox::utility::hash_combine(result, sampler.anisotropyEnable);
        vox::utility::hash_combine(result, sampler.maxAnisotropy);
        vox::utility::hash_combine(result, sampler.compareEnable);
        vox::utility::hash_combine(result, sampler.compareOp);
        vox::utility::hash_combine(result, sampler.minLod);
        vox::utility::hash_combine(result, sampler.maxLod);
        vox::utility::hash_combine(result, sampler.borderColor);
        return result;
    }
};

template <>
struct hash<vox::ShaderSource> {
    std::size_t operator()(const vox::ShaderSource &shader_source) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, shader_source.GetId());

        return result;
    }
};

template <>
struct hash<vox::ShaderVariant> {
    std::size_t operator()(const vox::ShaderVariant &shader_variant) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, shader_variant.GetId());

        return result;
    }
};

template <>
struct hash<vox::ShaderModule> {
    std::size_t operator()(const vox::ShaderModule &shader_module) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, shader_module.GetId());

        return result;
    }
};

template <>
struct hash<vox::DescriptorSetLayout> {
    std::size_t operator()(const vox::DescriptorSetLayout &descriptor_set_layout) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, descriptor_set_layout.GetHandle());

        return result;
    }
};

template <>
struct hash<vox::DescriptorPool> {
    std::size_t operator()(const vox::DescriptorPool &descriptor_pool) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, descriptor_pool.GetDescriptorSetLayout());

        return result;
    }
};

template <>
struct hash<vox::PipelineLayout> {
    std::size_t operator()(const vox::PipelineLayout &pipeline_layout) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, pipeline_layout.GetHandle());

        return result;
    }
};

template <>
struct hash<vox::RenderPass> {
    std::size_t operator()(const vox::RenderPass &render_pass) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, render_pass.GetHandle());

        return result;
    }
};

template <>
struct hash<vox::Attachment> {
    std::size_t operator()(const vox::Attachment &attachment) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkFormat>::type>(attachment.format));
        vox::utility::hash_combine(result,
                                   static_cast<std::underlying_type<VkSampleCountFlagBits>::type>(attachment.samples));
        vox::utility::hash_combine(result, attachment.usage);
        vox::utility::hash_combine(result,
                                   static_cast<std::underlying_type<VkImageLayout>::type>(attachment.initial_layout));

        return result;
    }
};

template <>
struct hash<vox::LoadStoreInfo> {
    std::size_t operator()(const vox::LoadStoreInfo &load_store_info) const {
        std::size_t result = 0;

        vox::utility::hash_combine(
                result, static_cast<std::underlying_type<VkAttachmentLoadOp>::type>(load_store_info.load_op));
        vox::utility::hash_combine(
                result, static_cast<std::underlying_type<VkAttachmentStoreOp>::type>(load_store_info.store_op));

        return result;
    }
};

template <>
struct hash<vox::SubpassInfo> {
    std::size_t operator()(const vox::SubpassInfo &subpass_info) const {
        std::size_t result = 0;

        for (uint32_t output_attachment : subpass_info.output_attachments) {
            vox::utility::hash_combine(result, output_attachment);
        }

        for (uint32_t input_attachment : subpass_info.input_attachments) {
            vox::utility::hash_combine(result, input_attachment);
        }

        for (uint32_t resolve_attachment : subpass_info.color_resolve_attachments) {
            vox::utility::hash_combine(result, resolve_attachment);
        }

        vox::utility::hash_combine(result, subpass_info.disable_depth_stencil_attachment);
        vox::utility::hash_combine(result, subpass_info.depth_stencil_resolve_attachment);
        vox::utility::hash_combine(result, subpass_info.depth_stencil_resolve_mode);

        return result;
    }
};

template <>
struct hash<vox::SpecializationConstantState> {
    std::size_t operator()(const vox::SpecializationConstantState &specialization_constant_state) const {
        std::size_t result = 0;

        for (const auto &constants : specialization_constant_state.GetSpecializationConstantState()) {
            vox::utility::hash_combine(result, constants.first);
            for (const auto kData : constants.second) {
                vox::utility::hash_combine(result, kData);
            }
        }

        return result;
    }
};

template <>
struct hash<vox::ShaderResource> {
    std::size_t operator()(const vox::ShaderResource &shader_resource) const {
        std::size_t result = 0;

        if (shader_resource.type == vox::ShaderResourceType::INPUT ||
            shader_resource.type == vox::ShaderResourceType::OUTPUT ||
            shader_resource.type == vox::ShaderResourceType::PUSH_CONSTANT ||
            shader_resource.type == vox::ShaderResourceType::SPECIALIZATION_CONSTANT) {
            return result;
        }

        vox::utility::hash_combine(result, shader_resource.set);
        vox::utility::hash_combine(result, shader_resource.binding);
        vox::utility::hash_combine(
                result, static_cast<std::underlying_type<vox::ShaderResourceType>::type>(shader_resource.type));
        vox::utility::hash_combine(result, shader_resource.mode);

        return result;
    }
};

template <>
struct hash<VkDescriptorBufferInfo> {
    std::size_t operator()(const VkDescriptorBufferInfo &descriptor_buffer_info) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, descriptor_buffer_info.buffer);
        vox::utility::hash_combine(result, descriptor_buffer_info.range);
        vox::utility::hash_combine(result, descriptor_buffer_info.offset);

        return result;
    }
};

template <>
struct hash<VkDescriptorImageInfo> {
    std::size_t operator()(const VkDescriptorImageInfo &descriptor_image_info) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, descriptor_image_info.imageView);
        vox::utility::hash_combine(
                result, static_cast<std::underlying_type<VkImageLayout>::type>(descriptor_image_info.imageLayout));
        vox::utility::hash_combine(result, descriptor_image_info.sampler);

        return result;
    }
};

template <>
struct hash<VkWriteDescriptorSet> {
    std::size_t operator()(const VkWriteDescriptorSet &write_descriptor_set) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, write_descriptor_set.dstSet);
        vox::utility::hash_combine(result, write_descriptor_set.dstBinding);
        vox::utility::hash_combine(result, write_descriptor_set.dstArrayElement);
        vox::utility::hash_combine(result, write_descriptor_set.descriptorCount);
        vox::utility::hash_combine(result, write_descriptor_set.descriptorType);

        switch (write_descriptor_set.descriptorType) {
            case VK_DESCRIPTOR_TYPE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                for (uint32_t i = 0; i < write_descriptor_set.descriptorCount; i++) {
                    vox::utility::hash_combine(result, write_descriptor_set.pImageInfo[i]);
                }
                break;

            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                for (uint32_t i = 0; i < write_descriptor_set.descriptorCount; i++) {
                    vox::utility::hash_combine(result, write_descriptor_set.pTexelBufferView[i]);
                }
                break;

            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                for (uint32_t i = 0; i < write_descriptor_set.descriptorCount; i++) {
                    vox::utility::hash_combine(result, write_descriptor_set.pBufferInfo[i]);
                }
                break;

            default:
                // Not implemented
                break;
        }

        return result;
    }
};

template <>
struct hash<VkVertexInputAttributeDescription> {
    std::size_t operator()(const VkVertexInputAttributeDescription &vertex_attrib) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, vertex_attrib.binding);
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkFormat>::type>(vertex_attrib.format));
        vox::utility::hash_combine(result, vertex_attrib.location);
        vox::utility::hash_combine(result, vertex_attrib.offset);

        return result;
    }
};

template <>
struct hash<VkVertexInputBindingDescription> {
    std::size_t operator()(const VkVertexInputBindingDescription &vertex_binding) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, vertex_binding.binding);
        vox::utility::hash_combine(
                result, static_cast<std::underlying_type<VkVertexInputRate>::type>(vertex_binding.inputRate));
        vox::utility::hash_combine(result, vertex_binding.stride);

        return result;
    }
};

template <>
struct hash<vox::StencilOpState> {
    std::size_t operator()(const vox::StencilOpState &stencil) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkCompareOp>::type>(stencil.compare_op));
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkStencilOp>::type>(stencil.depth_fail_op));
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkStencilOp>::type>(stencil.fail_op));
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkStencilOp>::type>(stencil.pass_op));

        return result;
    }
};

template <>
struct hash<VkExtent2D> {
    size_t operator()(const VkExtent2D &extent) const {
        size_t result = 0;

        vox::utility::hash_combine(result, extent.width);
        vox::utility::hash_combine(result, extent.height);

        return result;
    }
};

template <>
struct hash<VkOffset2D> {
    size_t operator()(const VkOffset2D &offset) const {
        size_t result = 0;

        vox::utility::hash_combine(result, offset.x);
        vox::utility::hash_combine(result, offset.y);

        return result;
    }
};

template <>
struct hash<VkRect2D> {
    size_t operator()(const VkRect2D &rect) const {
        size_t result = 0;

        vox::utility::hash_combine(result, rect.extent);
        vox::utility::hash_combine(result, rect.offset);

        return result;
    }
};

template <>
struct hash<VkViewport> {
    size_t operator()(const VkViewport &viewport) const {
        size_t result = 0;

        vox::utility::hash_combine(result, viewport.width);
        vox::utility::hash_combine(result, viewport.height);
        vox::utility::hash_combine(result, viewport.maxDepth);
        vox::utility::hash_combine(result, viewport.minDepth);
        vox::utility::hash_combine(result, viewport.x);
        vox::utility::hash_combine(result, viewport.y);

        return result;
    }
};

template <>
struct hash<vox::ColorBlendAttachmentState> {
    std::size_t operator()(const vox::ColorBlendAttachmentState &color_blend_attachment) const {
        std::size_t result = 0;

        vox::utility::hash_combine(
                result, static_cast<std::underlying_type<VkBlendOp>::type>(color_blend_attachment.alpha_blend_op));
        vox::utility::hash_combine(result, color_blend_attachment.blend_enable);
        vox::utility::hash_combine(
                result, static_cast<std::underlying_type<VkBlendOp>::type>(color_blend_attachment.color_blend_op));
        vox::utility::hash_combine(result, color_blend_attachment.color_write_mask);
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkBlendFactor>::type>(
                                                   color_blend_attachment.dst_alpha_blend_factor));
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkBlendFactor>::type>(
                                                   color_blend_attachment.dst_color_blend_factor));
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkBlendFactor>::type>(
                                                   color_blend_attachment.src_alpha_blend_factor));
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkBlendFactor>::type>(
                                                   color_blend_attachment.src_color_blend_factor));

        return result;
    }
};

template <>
struct hash<vox::RenderTarget> {
    std::size_t operator()(const vox::RenderTarget &render_target) const {
        std::size_t result = 0;

        for (auto &view : render_target.GetViews()) {
            vox::utility::hash_combine(result, view.GetHandle());
            vox::utility::hash_combine(result, view.GetImage().GetHandle());
        }

        return result;
    }
};

template <>
struct hash<vox::PipelineState> {
    std::size_t operator()(const vox::PipelineState &pipeline_state) const {
        std::size_t result = 0;

        vox::utility::hash_combine(result, pipeline_state.GetPipelineLayout().GetHandle());

        // For graphics only
        if (auto render_pass = pipeline_state.GetRenderPass()) {
            vox::utility::hash_combine(result, render_pass->GetHandle());
        }

        vox::utility::hash_combine(result, pipeline_state.GetSpecializationConstantState());

        vox::utility::hash_combine(result, pipeline_state.GetSubpassIndex());

        for (auto shader_module : pipeline_state.GetPipelineLayout().GetShaderModules()) {
            vox::utility::hash_combine(result, shader_module->GetId());
        }

        // VkPipelineVertexInputStateCreateInfo
        for (auto &attribute : pipeline_state.GetVertexInputState().attributes) {
            vox::utility::hash_combine(result, attribute);
        }

        for (auto &binding : pipeline_state.GetVertexInputState().bindings) {
            vox::utility::hash_combine(result, binding);
        }

        // VkPipelineInputAssemblyStateCreateInfo
        vox::utility::hash_combine(result, pipeline_state.GetInputAssemblyState().primitive_restart_enable);
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkPrimitiveTopology>::type>(
                                                   pipeline_state.GetInputAssemblyState().topology));

        // VkPipelineViewportStateCreateInfo
        vox::utility::hash_combine(result, pipeline_state.GetViewportState().viewport_count);
        vox::utility::hash_combine(result, pipeline_state.GetViewportState().scissor_count);

        // VkPipelineRasterizationStateCreateInfo
        vox::utility::hash_combine(result, pipeline_state.GetRasterizationState().cull_mode);
        vox::utility::hash_combine(result, pipeline_state.GetRasterizationState().depth_bias_enable);
        vox::utility::hash_combine(result, pipeline_state.GetRasterizationState().depth_clamp_enable);
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkFrontFace>::type>(
                                                   pipeline_state.GetRasterizationState().front_face));
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkPolygonMode>::type>(
                                                   pipeline_state.GetRasterizationState().polygon_mode));
        vox::utility::hash_combine(result, pipeline_state.GetRasterizationState().rasterizer_discard_enable);

        // VkPipelineMultisampleStateCreateInfo
        vox::utility::hash_combine(result, pipeline_state.GetMultisampleState().alpha_to_coverage_enable);
        vox::utility::hash_combine(result, pipeline_state.GetMultisampleState().alpha_to_one_enable);
        vox::utility::hash_combine(result, pipeline_state.GetMultisampleState().min_sample_shading);
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkSampleCountFlagBits>::type>(
                                                   pipeline_state.GetMultisampleState().rasterization_samples));
        vox::utility::hash_combine(result, pipeline_state.GetMultisampleState().sample_shading_enable);
        vox::utility::hash_combine(result, pipeline_state.GetMultisampleState().sample_mask);

        // VkPipelineDepthStencilStateCreateInfo
        vox::utility::hash_combine(result, pipeline_state.GetDepthStencilState().back);
        vox::utility::hash_combine(result, pipeline_state.GetDepthStencilState().depth_bounds_test_enable);
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkCompareOp>::type>(
                                                   pipeline_state.GetDepthStencilState().depth_compare_op));
        vox::utility::hash_combine(result, pipeline_state.GetDepthStencilState().depth_test_enable);
        vox::utility::hash_combine(result, pipeline_state.GetDepthStencilState().depth_write_enable);
        vox::utility::hash_combine(result, pipeline_state.GetDepthStencilState().front);
        vox::utility::hash_combine(result, pipeline_state.GetDepthStencilState().stencil_test_enable);

        // VkPipelineColorBlendStateCreateInfo
        vox::utility::hash_combine(result, static_cast<std::underlying_type<VkLogicOp>::type>(
                                                   pipeline_state.GetColorBlendState().logic_op));
        vox::utility::hash_combine(result, pipeline_state.GetColorBlendState().logic_op_enable);

        for (auto &attachment : pipeline_state.GetColorBlendState().attachments) {
            vox::utility::hash_combine(result, attachment);
        }

        return result;
    }
};
}  // namespace std

namespace vox {
namespace {
template <typename T>
inline void HashParam(size_t &seed, const T &value) {
    utility::hash_combine(seed, value);
}

template <>
inline void HashParam(size_t & /*seed*/, const VkPipelineCache & /*value*/) {}

template <>
inline void HashParam<std::vector<uint8_t>>(size_t &seed, const std::vector<uint8_t> &value) {
    utility::hash_combine(seed, std::string{value.begin(), value.end()});
}

template <>
inline void HashParam<std::vector<Attachment>>(size_t &seed, const std::vector<Attachment> &value) {
    for (auto &attachment : value) {
        utility::hash_combine(seed, attachment);
    }
}

template <>
inline void HashParam<std::vector<LoadStoreInfo>>(size_t &seed, const std::vector<LoadStoreInfo> &value) {
    for (auto &load_store_info : value) {
        utility::hash_combine(seed, load_store_info);
    }
}

template <>
inline void HashParam<std::vector<SubpassInfo>>(size_t &seed, const std::vector<SubpassInfo> &value) {
    for (auto &subpass_info : value) {
        utility::hash_combine(seed, subpass_info);
    }
}

template <>
inline void HashParam<std::vector<ShaderModule *>>(size_t &seed, const std::vector<ShaderModule *> &value) {
    for (auto &shader_module : value) {
        utility::hash_combine(seed, shader_module->GetId());
    }
}

template <>
inline void HashParam<std::vector<ShaderResource>>(size_t &seed, const std::vector<ShaderResource> &value) {
    for (auto &resource : value) {
        utility::hash_combine(seed, resource);
    }
}

template <>
inline void HashParam<std::map<uint32_t, std::map<uint32_t, VkDescriptorBufferInfo>>>(
        size_t &seed, const std::map<uint32_t, std::map<uint32_t, VkDescriptorBufferInfo>> &value) {
    for (auto &binding_set : value) {
        utility::hash_combine(seed, binding_set.first);

        for (auto &binding_element : binding_set.second) {
            utility::hash_combine(seed, binding_element.first);
            utility::hash_combine(seed, binding_element.second);
        }
    }
}

template <>
inline void HashParam<std::map<uint32_t, std::map<uint32_t, VkDescriptorImageInfo>>>(
        size_t &seed, const std::map<uint32_t, std::map<uint32_t, VkDescriptorImageInfo>> &value) {
    for (auto &binding_set : value) {
        utility::hash_combine(seed, binding_set.first);

        for (auto &binding_element : binding_set.second) {
            utility::hash_combine(seed, binding_element.first);
            utility::hash_combine(seed, binding_element.second);
        }
    }
}

template <typename T, typename... Args>
inline void HashParam(size_t &seed, const T &first_arg, const Args &...args) {
    HashParam(seed, first_arg);

    HashParam(seed, args...);
}

template <class T, class... A>
struct RecordHelper {
    size_t Record(ResourceRecord & /*recorder*/, A &.../*args*/) { return 0; }

    void Index(ResourceRecord & /*recorder*/, size_t /*index*/, T & /*resource*/) {}
};

template <class... A>
struct RecordHelper<ShaderModule, A...> {
    size_t Record(ResourceRecord &recorder, A &...args) { return recorder.RegisterShaderModule(args...); }

    void Index(ResourceRecord &recorder, size_t index, ShaderModule &shader_module) {
        recorder.SetShaderModule(index, shader_module);
    }
};

template <class... A>
struct RecordHelper<PipelineLayout, A...> {
    size_t Record(ResourceRecord &recorder, A &...args) { return recorder.RegisterPipelineLayout(args...); }

    void Index(ResourceRecord &recorder, size_t index, PipelineLayout &pipeline_layout) {
        recorder.SetPipelineLayout(index, pipeline_layout);
    }
};

template <class... A>
struct RecordHelper<RenderPass, A...> {
    size_t Record(ResourceRecord &recorder, A &...args) { return recorder.RegisterRenderPass(args...); }

    void Index(ResourceRecord &recorder, size_t index, RenderPass &render_pass) {
        recorder.SetRenderPass(index, render_pass);
    }
};

template <class... A>
struct RecordHelper<GraphicsPipeline, A...> {
    size_t Record(ResourceRecord &recorder, A &...args) { return recorder.RegisterGraphicsPipeline(args...); }

    void Index(ResourceRecord &recorder, size_t index, GraphicsPipeline &graphics_pipeline) {
        recorder.SetGraphicsPipeline(index, graphics_pipeline);
    }
};
}  // namespace

template <class T, class... A>
T &RequestResource(Device &device,
                   ResourceRecord *recorder,
                   std::unordered_map<std::size_t, T> &resources,
                   A &...args) {
    RecordHelper<T, A...> record_helper;

    std::size_t hash{0U};
    HashParam(hash, args...);

    auto res_it = resources.find(hash);

    if (res_it != resources.end()) {
        return res_it->second;
    }

    // If we do not have it already, create and cache it
    const char *res_type = typeid(T).name();
    size_t res_id = resources.size();

    LOGD("Building #{} cache object ({})", res_id, res_type)

    // Only error handle in release
#ifndef DEBUG
    try {
#endif
        T resource(device, args...);

        auto res_ins_it = resources.emplace(hash, std::move(resource));

        if (!res_ins_it.second) {
            throw std::runtime_error{std::string{"Insertion error for #"} + std::to_string(res_id) + "cache object (" +
                                     res_type + ")"};
        }

        res_it = res_ins_it.first;

        if (recorder) {
            size_t index = record_helper.Record(*recorder, args...);
            record_helper.Index(*recorder, index, res_it->second);
        }
#ifndef DEBUG
    } catch (const std::exception &e) {
        LOGE("Creation error for #{} cache object ({})", res_id, res_type);
        throw e;
    }
#endif

    return res_it->second;
}

}  // namespace vox
