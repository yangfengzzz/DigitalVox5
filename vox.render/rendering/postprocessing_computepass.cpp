//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/postprocessing_computepass.h"

#include <utility>

#include "vox.render/rendering/postprocessing_pipeline.h"

namespace vox {
PostProcessingComputePass::PostProcessingComputePass(PostProcessingPipeline *parent,
                                                     std::shared_ptr<ShaderSource> cs_source)
    : PostProcessingPass{parent}, cs_source_{std::move(cs_source)} {}

void PostProcessingComputePass::AttachShaderData(ShaderData *data) {
    auto iter = std::find(data_.begin(), data_.end(), data);
    if (iter == data_.end()) {
        data_.push_back(data);
    } else {
        LOGE("ShaderData already attached.")
    }
}

void PostProcessingComputePass::DetachShaderData(ShaderData *data) {
    auto iter = std::find(data_.begin(), data_.end(), data);
    if (iter != data_.end()) {
        data_.erase(iter);
    }
}

void PostProcessingComputePass::Prepare(CommandBuffer &command_buffer, RenderTarget &default_render_target) {}

void PostProcessingComputePass::Draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) {
    ShaderVariant cs_variant;
    for (const auto &data : data_) {
        data->MergeVariants(cs_variant, cs_variant);
    }

    TransitionImages(command_buffer, default_render_target, cs_variant);

    // Get compute shader from cache
    auto &resource_cache = command_buffer.GetDevice().GetResourceCache();
    auto &shader_module = resource_cache.RequestShaderModule(VK_SHADER_STAGE_COMPUTE_BIT, *cs_source_, cs_variant);

    // Create pipeline layout and bind it
    auto &pipeline_layout = resource_cache.RequestPipelineLayout({&shader_module});
    command_buffer.BindPipelineLayout(pipeline_layout);

    auto &bindings = pipeline_layout.GetDescriptorSetLayout(0);
    // Bind samplers to set = 0, binding = <according to name>
    for (const auto &data : data_) {
        data->BindData(command_buffer, bindings);
    }

    if (!push_constants_data_.empty()) {
        command_buffer.PushConstants(push_constants_data_);
    }

    // Dispatch compute
    command_buffer.Dispatch(n_workgroups_[0], n_workgroups_[1], n_workgroups_[2]);
}

// MARK: - Transition
void PostProcessingComputePass::TransitionImages(CommandBuffer &command_buffer,
                                                 RenderTarget &default_render_target,
                                                 const ShaderVariant &cs_variant) {
    BarrierInfo fallback_barrier_src{};
    fallback_barrier_src.pipeline_stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    fallback_barrier_src.image_read_access = 0;  // For UNDEFINED -> STORAGE in first CP
    fallback_barrier_src.image_write_access = 0;
    const auto kPrevPassBarrierInfo = GetPredecessorSrcBarrierInfo(fallback_barrier_src);

    // Get compute shader from cache
    auto &resource_cache = command_buffer.GetDevice().GetResourceCache();
    auto &shader_module = resource_cache.RequestShaderModule(VK_SHADER_STAGE_COMPUTE_BIT, *cs_source_, cs_variant);
    auto &pipeline_layout = resource_cache.RequestPipelineLayout({&shader_module});

    for (const auto &data : data_) {
        for (const auto &sampled : data->SampledTextures()) {
            if (const uint32_t *attachment = sampled.second->GetTargetAttachment()) {
                auto *sampled_rt = sampled.second->GetRenderTarget();
                if (sampled_rt == nullptr) {
                    sampled_rt = &default_render_target;
                }

                if (sampled_rt->GetLayout(*attachment) == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                    // No-op
                    continue;
                }

                vox::ImageMemoryBarrier barrier;
                barrier.old_layout = sampled_rt->GetLayout(*attachment);
                barrier.new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.src_access_mask = kPrevPassBarrierInfo.image_write_access;
                barrier.dst_access_mask = VK_ACCESS_SHADER_READ_BIT;
                barrier.src_stage_mask = kPrevPassBarrierInfo.pipeline_stage;
                barrier.dst_stage_mask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

                command_buffer.ImageMemoryBarrier(sampled_rt->GetViews().at(*attachment), barrier);
                sampled_rt->SetLayout(*attachment, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
        }
    }

    for (const auto &data : data_) {
        for (const auto &storage : data->StorageTextures()) {
            if (const uint32_t *attachment = storage.second->GetTargetAttachment()) {
                auto *storage_rt = storage.second->GetRenderTarget();
                if (storage_rt == nullptr) {
                    storage_rt = &default_render_target;
                }

                // A storage image is either read-only or write-only;
                // use shader reflection to figure out which case, then transition
                // NOTE: Could add a <name -> readonly?> cache to make this faster?
                auto resource =
                        std::find_if(pipeline_layout.GetResources().begin(), pipeline_layout.GetResources().end(),
                                     [&storage](const auto &res) { return res.set == 0 && res.name == storage.first; });
                if (resource == pipeline_layout.GetResources().end()) {
                    // No such storage image to bind
                    continue;
                }

                const bool kReadable = !(resource->qualifiers & ShaderResourceQualifiers::NON_READABLE);
                const bool kWritable = !(resource->qualifiers & ShaderResourceQualifiers::NON_READABLE);

                vox::ImageMemoryBarrier barrier;
                barrier.old_layout = storage_rt->GetLayout(*attachment);
                barrier.new_layout =
                        (kReadable && !kWritable) ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_GENERAL;

                if (storage_rt->GetLayout(*attachment) == barrier.new_layout) {
                    // No-op
                    continue;
                }

                barrier.src_stage_mask = kPrevPassBarrierInfo.pipeline_stage;
                barrier.dst_stage_mask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

                barrier.src_access_mask = kPrevPassBarrierInfo.image_write_access;
                barrier.dst_access_mask = 0;
                if (kReadable) {
                    barrier.dst_access_mask |= VK_ACCESS_SHADER_READ_BIT;
                }
                if (kWritable) {
                    barrier.dst_access_mask |= VK_ACCESS_SHADER_WRITE_BIT;
                }

                command_buffer.ImageMemoryBarrier(storage_rt->GetViews().at(*attachment), barrier);
                storage_rt->SetLayout(*attachment, barrier.new_layout);
            }
        }
    }
}

PostProcessingComputePass::BarrierInfo PostProcessingComputePass::GetSrcBarrierInfo() const {
    BarrierInfo info{};
    info.pipeline_stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    info.image_read_access = VK_ACCESS_SHADER_READ_BIT;
    info.image_write_access = VK_ACCESS_SHADER_WRITE_BIT;
    return info;
}

PostProcessingComputePass::BarrierInfo PostProcessingComputePass::GetDstBarrierInfo() const {
    BarrierInfo info{};
    info.pipeline_stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    info.image_read_access = VK_ACCESS_SHADER_READ_BIT;
    info.image_write_access = VK_ACCESS_SHADER_WRITE_BIT;
    return info;
}

}  // namespace vox
