//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "postprocessing_renderpass.h"

#include "postprocessing_pipeline.h"

namespace vox {
constexpr uint32_t kDepthResolveBitmask = 0x80000000;
constexpr uint32_t kAttachmentBitmask = 0x7FFFFFFF;

PostProcessingSubpass::PostProcessingSubpass(PostProcessingRenderPass *parent,
                                             RenderContext &render_context,
                                             std::shared_ptr<ShaderSource> triangle_vs,
                                             std::shared_ptr<ShaderSource> fs,
                                             ShaderVariant &&fs_variant) :
Subpass(render_context, nullptr, nullptr),
vertex_shader_{std::move(triangle_vs)},
fragment_shader_{std::move(fs)},
parent_{parent},
fs_variant_{std::move(fs_variant)} {
    set_disable_depth_stencil_attachment(true);
    
    std::vector<uint32_t> input_attachments{};
    for (const auto &it : input_attachments_) {
        input_attachments.push_back(it.second);
    }
    set_input_attachments(input_attachments);
}

PostProcessingSubpass::PostProcessingSubpass(PostProcessingSubpass &&to_move) noexcept:
Subpass{std::move(to_move)},
parent_{to_move.parent_},
fs_variant_{std::move(to_move.fs_variant_)},
input_attachments_{std::move(to_move.input_attachments_)},
sampled_images_{std::move(to_move.sampled_images_)} {}

PostProcessingSubpass &PostProcessingSubpass::bind_input_attachment(const std::string &name,
                                                                    uint32_t new_input_attachment) {
    input_attachments_[name] = new_input_attachment;
    
    std::vector<uint32_t> input_attachments{};
    for (const auto &it : input_attachments_) {
        input_attachments.push_back(it.second);
    }
    set_input_attachments(input_attachments);
    
    parent_->load_stores_dirty_ = true;
    return *this;
}

PostProcessingSubpass &PostProcessingSubpass::bind_sampled_image(const std::string &name,
                                                                 core::SampledImage &&new_image) {
    auto it = sampled_images_.find(name);
    if (it != sampled_images_.end()) {
        it->second = std::move(new_image);
    } else {
        sampled_images_.emplace(name, std::move(new_image));
    }
    
    parent_->load_stores_dirty_ = true;
    return *this;
}

PostProcessingSubpass &PostProcessingSubpass::bind_storage_image(const std::string &name,
                                                                 const core::ImageView &new_image) {
    auto it = storage_images_.find(name);
    if (it != storage_images_.end()) {
        it->second = &new_image;
    } else {
        storage_images_.emplace(name, &new_image);
    }
    
    return *this;
}

PostProcessingSubpass &PostProcessingSubpass::set_push_constants(const std::vector<uint8_t> &data) {
    push_constants_data_ = data;
    return *this;
}

PostProcessingSubpass &PostProcessingSubpass::set_draw_func(DrawFunc &&new_func) {
    draw_func_ = std::move(new_func);
    return *this;
}

void PostProcessingSubpass::prepare() {
    // Build all shaders upfront
    auto &resource_cache = render_context_.get_device().get_resource_cache();
    resource_cache.request_shader_module(VK_SHADER_STAGE_VERTEX_BIT, *vertex_shader_);
    resource_cache.request_shader_module(VK_SHADER_STAGE_FRAGMENT_BIT, *fragment_shader_, fs_variant_);
}

void PostProcessingSubpass::draw(CommandBuffer &command_buffer) {
    // Get shaders from cache
    auto &resource_cache = command_buffer.get_device().get_resource_cache();
    auto &vert_shader_module = resource_cache.request_shader_module(VK_SHADER_STAGE_VERTEX_BIT, *vertex_shader_);
    auto &frag_shader_module =
    resource_cache.request_shader_module(VK_SHADER_STAGE_FRAGMENT_BIT, *fragment_shader_, fs_variant_);
    
    std::vector<ShaderModule *> shader_modules{&vert_shader_module, &frag_shader_module};
    
    // Create pipeline layout and bind it
    auto &pipeline_layout = resource_cache.request_pipeline_layout(shader_modules);
    command_buffer.bind_pipeline_layout(pipeline_layout);
    
    // Disable culling
    RasterizationState rasterization_state;
    rasterization_state.cull_mode = VK_CULL_MODE_NONE;
    command_buffer.set_rasterization_state(rasterization_state);
    
    auto &render_target = *parent_->draw_render_target_;
    const auto &target_views = render_target.get_views();
    
    if (parent_->uniform_buffer_alloc_ != nullptr) {
        // Bind buffer to set = 0, binding = 0
        auto &uniform_alloc = *parent_->uniform_buffer_alloc_;
        command_buffer
            .bind_buffer(uniform_alloc.get_buffer(), uniform_alloc.get_offset(), uniform_alloc.get_size(), 0, 0, 0);
    }
    
    const auto &bindings = pipeline_layout.get_descriptor_set_layout(0);
    
    // Bind subpass inputs to set = 0, binding = <according to name>
    for (const auto &it : input_attachments_) {
        if (auto layout_binding = bindings.get_layout_binding(it.first)) {
            command_buffer.bind_input(target_views.at(it.second), 0, layout_binding->binding, 0);
        }
    }
    
    // Bind samplers to set = 0, binding = <according to name>
    for (const auto &it : sampled_images_) {
        if (auto layout_binding = bindings.get_layout_binding(it.first)) {
            const auto &view = it.second.get_image_view(render_target);
            const auto &sampler = it.second.get_sampler() ? *it.second.get_sampler() : *parent_->default_sampler_;
            
            command_buffer.bind_image(view, sampler, 0, layout_binding->binding, 0);
        }
    }
    
    // Bind storage images to set = 0, binding = <according to name>
    for (const auto &it : storage_images_) {
        if (auto layout_binding = bindings.get_layout_binding(it.first)) {
            command_buffer.bind_image(*it.second, 0, layout_binding->binding, 0);
        }
    }
    
    // Per-draw push constants
    command_buffer.push_constants(push_constants_data_);
    
    // draw full screen triangle
    draw_func_(command_buffer, render_target);
}

void PostProcessingSubpass::default_draw_func(vox::CommandBuffer &command_buffer, vox::RenderTarget &) {
    command_buffer.draw(3, 1, 0, 0);
}

//MARK: - PostProcessingRenderPass
PostProcessingRenderPass::PostProcessingRenderPass(PostProcessingPipeline *parent,
                                                   std::unique_ptr<core::Sampler> &&default_sampler) :
PostProcessingPass{parent},
default_sampler_{std::move(default_sampler)} {
    if (default_sampler_ == nullptr) {
        // Setup a sane default sampler if none was passed
        VkSamplerCreateInfo sampler_info{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.compareOp = VK_COMPARE_OP_NEVER;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;
        sampler_info.anisotropyEnable = VK_FALSE;
        sampler_info.maxAnisotropy = 0.0f;
        sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        
        default_sampler_ = std::make_unique<vox::core::Sampler>(get_render_context().get_device(), sampler_info);
    }
}

void PostProcessingRenderPass::update_load_stores(
                                                  const AttachmentSet &input_attachments,
                                                  const SampledAttachmentSet &sampled_attachments,
                                                  const AttachmentSet &output_attachments,
                                                  const RenderTarget &fallback_render_target) {
    if (!load_stores_dirty_) {
        return;
    }
    
    const auto &render_target = render_target_ ? *render_target_ : fallback_render_target;
    
    // Update load/stores accordingly
    load_stores_.clear();
    
    for (uint32_t j = 0; j < uint32_t(render_target.get_attachments().size()); j++) {
        const bool kIsInput = input_attachments.find(j) != input_attachments.end();
        const bool kIsSampled = std::find_if(sampled_attachments.begin(), sampled_attachments.end(),
                                             [&render_target, j](auto &pair) {
            // NOTE: if RT not set, default is the currently-active one
            auto *sampled_rt = pair.first ? pair.first : &render_target;
            // unpack attachment
            uint32_t attachment = pair.second & kAttachmentBitmask;
            return attachment == j && sampled_rt == &render_target;
        }) != sampled_attachments.end();
        const bool kIsOutput = output_attachments.find(j) != output_attachments.end();
        
        VkAttachmentLoadOp load;
        if (kIsInput || kIsSampled) {
            load = VK_ATTACHMENT_LOAD_OP_LOAD;
        } else if (kIsOutput) {
            load = VK_ATTACHMENT_LOAD_OP_CLEAR;
        } else {
            load = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
        
        VkAttachmentStoreOp store;
        if (kIsOutput) {
            store = VK_ATTACHMENT_STORE_OP_STORE;
        } else {
            store = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
        
        load_stores_.push_back({load, store});
    }
    
    pipeline_.set_load_store(load_stores_);
    load_stores_dirty_ = false;
}

PostProcessingRenderPass::BarrierInfo PostProcessingRenderPass::get_src_barrier_info() const {
    BarrierInfo info{};
    info.pipeline_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    info.image_read_access = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    info.image_write_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    return info;
}

PostProcessingRenderPass::BarrierInfo PostProcessingRenderPass::get_dst_barrier_info() const {
    BarrierInfo info{};
    info.pipeline_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    info.image_read_access = VK_ACCESS_SHADER_READ_BIT;
    info.image_write_access = VK_ACCESS_SHADER_WRITE_BIT;
    return info;
}

// If the passed `src_access` is zero, guess it - and the corresponding source stage - from the src_access_mask
// of the image
static void ensure_src_access(uint32_t &src_access, uint32_t &src_stage, VkImageLayout layout) {
    if (src_access == 0) {
        switch (layout) {
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                src_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                src_access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                src_access |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                break;
            default: src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                src_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;
        }
    }
}

void PostProcessingRenderPass::transition_attachments(
                                                      const AttachmentSet &input_attachments,
                                                      const SampledAttachmentSet &sampled_attachments,
                                                      const AttachmentSet &output_attachments,
                                                      CommandBuffer &command_buffer,
                                                      RenderTarget &fallback_render_target) {
    auto &render_target = render_target_ ? *render_target_ : fallback_render_target;
    const auto &views = render_target.get_views();
    
    BarrierInfo fallback_barrier_src{};
    fallback_barrier_src.pipeline_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    fallback_barrier_src.image_read_access = 0;        // For UNDEFINED -> COLOR_ATTACHMENT_OPTIMAL in first RP
    fallback_barrier_src.image_write_access = 0;
    auto prev_pass_barrier_info = get_predecessor_src_barrier_info(fallback_barrier_src);
    
    for (uint32_t input : input_attachments) {
        const VkImageLayout kPrevLayout = render_target.get_layout(input);
        if (kPrevLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            // No-op
            continue;
        }
        
        ensure_src_access(prev_pass_barrier_info.image_write_access, prev_pass_barrier_info.pipeline_stage,
                          kPrevLayout);
        
        vox::ImageMemoryBarrier barrier;
        barrier.old_layout = render_target.get_layout(input);
        barrier.new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.src_access_mask = prev_pass_barrier_info.image_write_access;
        barrier.dst_access_mask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        barrier.src_stage_mask = prev_pass_barrier_info.pipeline_stage;
        barrier.dst_stage_mask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        
        command_buffer.image_memory_barrier(views.at(input), barrier);
        render_target.set_layout(input, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    
    for (const auto &sampled : sampled_attachments) {
        auto *sampled_rt = sampled.first ? sampled.first : &render_target;
        
        // unpack depth resolve flag and attachment
        bool is_depth_resolve = sampled.second & kDepthResolveBitmask;
        uint32_t attachment = sampled.second & kAttachmentBitmask;
        
        const auto kPrevLayout = sampled_rt->get_layout(attachment);
        
        if (kPrevLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            // No-op
            continue;
        }
        
        // The resolving depth occurs in the COLOR_ATTACHMENT_OUT stage, not in the EARLY\LATE_FRAGMENT_TESTS stage
        // and the corresponding access mask is COLOR_ATTACHMENT_WRITE_BIT, not DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
        if (is_depth_resolve && kPrevLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            prev_pass_barrier_info.pipeline_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            prev_pass_barrier_info.image_read_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        } else {
            ensure_src_access(prev_pass_barrier_info.image_read_access, prev_pass_barrier_info.pipeline_stage,
                              kPrevLayout);
        }
        
        vox::ImageMemoryBarrier barrier;
        barrier.old_layout = kPrevLayout;
        barrier.new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.src_access_mask = prev_pass_barrier_info.image_read_access;
        barrier.dst_access_mask = VK_ACCESS_SHADER_READ_BIT;
        barrier.src_stage_mask = prev_pass_barrier_info.pipeline_stage;
        barrier.dst_stage_mask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        
        command_buffer.image_memory_barrier(sampled_rt->get_views().at(attachment), barrier);
        sampled_rt->set_layout(attachment, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    
    for (uint32_t output : output_attachments) {
        const VkFormat kAttachmentFormat = views.at(output).get_format();
        const bool kIsDepthStencil =
        vox::is_depth_only_format(kAttachmentFormat) || vox::is_depth_stencil_format(kAttachmentFormat);
        const VkImageLayout kOutputLayout = kIsDepthStencil ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL :
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        if (render_target.get_layout(output) == kOutputLayout) {
            // No-op
            continue;
        }
        
        vox::ImageMemoryBarrier barrier;
        barrier.old_layout = VK_IMAGE_LAYOUT_UNDEFINED;        // = don't care about previous contents
        barrier.new_layout = kOutputLayout;
        barrier.src_access_mask = 0;
        if (kIsDepthStencil) {
            barrier.dst_access_mask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            barrier.src_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            barrier.dst_stage_mask =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        } else {
            barrier.dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            barrier.dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        
        command_buffer.image_memory_barrier(views.at(output), barrier);
        render_target.set_layout(output, kOutputLayout);
    }
    
    // NOTE: Unused attachments might be carried over to other render passes,
    //       so we don't want to transition them to UNDEFINED layout here
}

void PostProcessingRenderPass::prepare_draw(CommandBuffer &command_buffer, RenderTarget &fallback_render_target) {
    // Collect all input, output, and sampled-from attachments from all subpasses (steps)
    AttachmentSet input_attachments, output_attachments;
    SampledAttachmentSet sampled_attachments;
    
    for (auto &step_ptr : pipeline_.get_subpasses()) {
        auto &step = *dynamic_cast<PostProcessingSubpass *>(step_ptr.get());
        
        for (auto &it : step.get_input_attachments()) {
            input_attachments.insert(it.second);
        }
        
        for (auto &it : step.get_sampled_images()) {
            if (const uint32_t *sampled_attachment = it.second.get_target_attachment()) {
                auto *image_rt = it.second.get_render_target();
                auto packed_sampled_attachment = *sampled_attachment;
                
                // pack sampled attachment
                if (it.second.is_depth_resolve())
                    packed_sampled_attachment |= kDepthResolveBitmask;
                
                sampled_attachments.insert({image_rt, packed_sampled_attachment});
            }
        }
        
        for (uint32_t it : step.get_output_attachments()) {
            output_attachments.insert(it);
        }
    }
    
    transition_attachments(input_attachments, sampled_attachments, output_attachments,
                           command_buffer, fallback_render_target);
    update_load_stores(input_attachments, sampled_attachments, output_attachments,
                       fallback_render_target);
}

void PostProcessingRenderPass::draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) {
    prepare_draw(command_buffer, default_render_target);
    
    if (!uniform_data_.empty()) {
        // Allocate a buffer (using the buffer pool from the active frame to store uniform values) and bind it
        auto &render_frame = parent_->get_render_context().get_active_frame();
        uniform_buffer_alloc_ = std::make_shared<BufferAllocation>(render_frame.allocate_buffer(
                                                                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                                                uniform_data_.size()));
        uniform_buffer_alloc_->update(uniform_data_);
    }
    
    // Update render target for this draw
    draw_render_target_ = render_target_ ? render_target_ : &default_render_target;
    
    // Set appropriate viewport & scissor for this RT
    {
        auto &extent = draw_render_target_->get_extent();
        
        VkViewport viewport{};
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        command_buffer.set_viewport(0, {viewport});
        
        VkRect2D scissor{};
        scissor.extent = extent;
        command_buffer.set_scissor(0, {scissor});
    }
    
    // Finally, draw all subpasses
    pipeline_.draw(command_buffer, *draw_render_target_);
    
    if (parent_->get_current_pass_index() < (parent_->get_passes().size() - 1)) {
        // Leave the last renderpass open for user modification (e.g., drawing GUI)
        command_buffer.end_render_pass();
    }
}

}        // namespace vox
