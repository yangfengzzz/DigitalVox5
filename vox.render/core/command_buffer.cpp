//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/command_buffer.h"

#include "vox.render/core/command_pool.h"
#include "vox.render/core/device.h"
#include "vox.render/error.h"
#include "vox.render/rendering/subpass.h"

namespace vox {
CommandBuffer::CommandBuffer(CommandPool &command_pool, VkCommandBufferLevel level)
    : VulkanResource{VK_NULL_HANDLE, &command_pool.GetDevice()},
      command_pool_{command_pool},
      max_push_constants_size_{device_->GetGpu().GetProperties().limits.maxPushConstantsSize},
      level_{level} {
    VkCommandBufferAllocateInfo allocate_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};

    allocate_info.commandPool = command_pool.GetHandle();
    allocate_info.commandBufferCount = 1;
    allocate_info.level = level;

    VkResult result = vkAllocateCommandBuffers(device_->GetHandle(), &allocate_info, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Failed to allocate command buffer"};
    }
}

CommandBuffer::~CommandBuffer() {
    // Destroy command buffer
    if (handle_ != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(command_pool_.GetDevice().GetHandle(), command_pool_.GetHandle(), 1, &handle_);
    }
}

CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept
    : VulkanResource{std::move(other)},
      command_pool_{other.command_pool_},
      level_{other.level_},
      state_{other.state_},
      update_after_bind_{other.update_after_bind_} {
    other.state_ = State::INVALID;
}

bool CommandBuffer::IsRecording() const { return state_ == State::RECORDING; }

void CommandBuffer::Clear(VkClearAttachment attachment, VkClearRect rect) {
    vkCmdClearAttachments(handle_, 1, &attachment, 1, &rect);
}

VkResult CommandBuffer::Begin(VkCommandBufferUsageFlags flags, CommandBuffer *primary_cmd_buf) {
    if (level_ == VK_COMMAND_BUFFER_LEVEL_SECONDARY) {
        assert(primary_cmd_buf &&
               "A primary command buffer pointer must be provided when calling begin from a secondary one");
        auto render_pass_binding = primary_cmd_buf->GetCurrentRenderPass();

        return Begin(flags, render_pass_binding.render_pass, render_pass_binding.framebuffer,
                     primary_cmd_buf->GetCurrentSubpassIndex());
    }

    return Begin(flags, nullptr, nullptr, 0);
}

VkResult CommandBuffer::Begin(VkCommandBufferUsageFlags flags,
                              const RenderPass *render_pass,
                              const Framebuffer *framebuffer,
                              uint32_t subpass_index) {
    assert(!IsRecording() && "Command buffer is already recording, please call end before beginning again");

    if (IsRecording()) {
        return VK_NOT_READY;
    }

    state_ = State::RECORDING;

    // Reset state
    pipeline_state_.Reset();
    resource_binding_state_.Reset();
    descriptor_set_layout_binding_state_.clear();
    stored_push_constants_.clear();

    VkCommandBufferBeginInfo begin_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    VkCommandBufferInheritanceInfo inheritance = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO};
    begin_info.flags = flags;

    if (level_ == VK_COMMAND_BUFFER_LEVEL_SECONDARY) {
        assert((render_pass && framebuffer) &&
               "Render pass and framebuffer must be provided when calling begin from a secondary one");

        current_render_pass_.render_pass = render_pass;
        current_render_pass_.framebuffer = framebuffer;

        inheritance.renderPass = current_render_pass_.render_pass->GetHandle();
        inheritance.framebuffer = current_render_pass_.framebuffer->GetHandle();
        inheritance.subpass = subpass_index;

        begin_info.pInheritanceInfo = &inheritance;
    }

    return vkBeginCommandBuffer(GetHandle(), &begin_info);
}

VkResult CommandBuffer::End() {
    assert(IsRecording() && "Command buffer is not recording, please call begin before end");

    if (!IsRecording()) {
        return VK_NOT_READY;
    }

    vkEndCommandBuffer(GetHandle());

    state_ = State::EXECUTABLE;

    return VK_SUCCESS;
}

void CommandBuffer::Flush(VkPipelineBindPoint pipeline_bind_point) {
    FlushPipelineState(pipeline_bind_point);

    FlushPushConstants();

    FlushDescriptorState(pipeline_bind_point);
}

void CommandBuffer::BeginRenderPass(const RenderTarget &render_target,
                                    const std::vector<LoadStoreInfo> &load_store_infos,
                                    const std::vector<VkClearValue> &clear_values,
                                    const std::vector<std::unique_ptr<Subpass>> &subpasses,
                                    VkSubpassContents contents) {
    // Reset state
    pipeline_state_.Reset();
    resource_binding_state_.Reset();
    descriptor_set_layout_binding_state_.clear();

    auto &render_pass = GetRenderPass(render_target, load_store_infos, subpasses);
    auto &framebuffer = GetDevice().GetResourceCache().RequestFramebuffer(render_target, render_pass);

    BeginRenderPass(render_target, render_pass, framebuffer, clear_values, contents);
}

void CommandBuffer::BeginRenderPass(const RenderTarget &render_target,
                                    const RenderPass &render_pass,
                                    const Framebuffer &framebuffer,
                                    const std::vector<VkClearValue> &clear_values,
                                    VkSubpassContents contents) {
    current_render_pass_.render_pass = &render_pass;
    current_render_pass_.framebuffer = &framebuffer;

    // Begin render pass
    VkRenderPassBeginInfo begin_info{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    begin_info.renderPass = current_render_pass_.render_pass->GetHandle();
    begin_info.framebuffer = current_render_pass_.framebuffer->GetHandle();
    begin_info.renderArea.extent = render_target.GetExtent();
    begin_info.clearValueCount = utility::ToU32(clear_values.size());
    begin_info.pClearValues = clear_values.data();

    const auto &framebuffer_extent = current_render_pass_.framebuffer->GetExtent();

    // Test the requested render area to confirm that it is optimal and could not cause a performance reduction
    if (!IsRenderSizeOptimal(framebuffer_extent, begin_info.renderArea)) {
        // Only prints the warning if the framebuffer or render area are different since the last time the render size
        // was not optimal
        if (framebuffer_extent.width != last_framebuffer_extent_.width ||
            framebuffer_extent.height != last_framebuffer_extent_.height ||
            begin_info.renderArea.extent.width != last_render_area_extent_.width ||
            begin_info.renderArea.extent.height != last_render_area_extent_.height) {
            LOGW("Render target extent is not an optimal size, this may result in reduced performance.")
        }

        last_framebuffer_extent_ = current_render_pass_.framebuffer->GetExtent();
        last_render_area_extent_ = begin_info.renderArea.extent;
    }

    vkCmdBeginRenderPass(GetHandle(), &begin_info, contents);

    // Update blend state attachments for first subpass
    auto blend_state = pipeline_state_.GetColorBlendState();
    blend_state.attachments.resize(
            current_render_pass_.render_pass->GetColorOutputCount(pipeline_state_.GetSubpassIndex()));
    pipeline_state_.SetColorBlendState(blend_state);
}

void CommandBuffer::NextSubpass() {
    // Increment subpass index
    pipeline_state_.SetSubpassIndex(pipeline_state_.GetSubpassIndex() + 1);

    // Update blend state attachments
    auto blend_state = pipeline_state_.GetColorBlendState();
    blend_state.attachments.resize(
            current_render_pass_.render_pass->GetColorOutputCount(pipeline_state_.GetSubpassIndex()));
    pipeline_state_.SetColorBlendState(blend_state);

    // Reset descriptor sets
    resource_binding_state_.Reset();
    descriptor_set_layout_binding_state_.clear();

    // Clear stored push constants
    stored_push_constants_.clear();

    vkCmdNextSubpass(GetHandle(), VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::ExecuteCommands(CommandBuffer &secondary_command_buffer) {
    vkCmdExecuteCommands(GetHandle(), 1, &secondary_command_buffer.GetHandle());
}

void CommandBuffer::ExecuteCommands(std::vector<CommandBuffer *> &secondary_command_buffers) {
    std::vector<VkCommandBuffer> sec_cmd_buf_handles(secondary_command_buffers.size(), VK_NULL_HANDLE);
    std::transform(secondary_command_buffers.begin(), secondary_command_buffers.end(), sec_cmd_buf_handles.begin(),
                   [](const vox::CommandBuffer *sec_cmd_buf) { return sec_cmd_buf->GetHandle(); });
    vkCmdExecuteCommands(GetHandle(), utility::ToU32(sec_cmd_buf_handles.size()), sec_cmd_buf_handles.data());
}

void CommandBuffer::EndRenderPass() { vkCmdEndRenderPass(GetHandle()); }

void CommandBuffer::BindPipelineLayout(PipelineLayout &pipeline_layout) {
    pipeline_state_.SetPipelineLayout(pipeline_layout);
}

void CommandBuffer::SetSpecializationConstant(uint32_t constant_id, const std::vector<uint8_t> &data) {
    pipeline_state_.SetSpecializationConstant(constant_id, data);
}

void CommandBuffer::PushConstants(const std::vector<uint8_t> &values) {
    uint32_t push_constant_size = utility::ToU32(stored_push_constants_.size() + values.size());

    if (push_constant_size > max_push_constants_size_) {
        LOGE("Push constant limit of {} exceeded (pushing {} bytes for a total of {} bytes)", max_push_constants_size_,
             values.size(), push_constant_size)
        throw std::runtime_error("Push constant limit exceeded.");
    } else {
        stored_push_constants_.insert(stored_push_constants_.end(), values.begin(), values.end());
    }
}

void CommandBuffer::BindBuffer(const core::Buffer &buffer,
                               VkDeviceSize offset,
                               VkDeviceSize range,
                               uint32_t set,
                               uint32_t binding,
                               uint32_t array_element) {
    resource_binding_state_.BindBuffer(buffer, offset, range, set, binding, array_element);
}

void CommandBuffer::BindImage(const core::ImageView &image_view,
                              const core::Sampler &sampler,
                              uint32_t set,
                              uint32_t binding,
                              uint32_t array_element) {
    resource_binding_state_.BindImage(image_view, sampler, set, binding, array_element);
}

void CommandBuffer::BindImage(const core::ImageView &image_view,
                              uint32_t set,
                              uint32_t binding,
                              uint32_t array_element) {
    resource_binding_state_.BindImage(image_view, set, binding, array_element);
}

void CommandBuffer::BindInput(const core::ImageView &image_view,
                              uint32_t set,
                              uint32_t binding,
                              uint32_t array_element) {
    resource_binding_state_.BindInput(image_view, set, binding, array_element);
}

void CommandBuffer::BindVertexBuffers(uint32_t first_binding,
                                      const std::vector<std::reference_wrapper<const vox::core::Buffer>> &buffers,
                                      const std::vector<VkDeviceSize> &offsets) {
    std::vector<VkBuffer> buffer_handles(buffers.size(), VK_NULL_HANDLE);
    std::transform(buffers.begin(), buffers.end(), buffer_handles.begin(),
                   [](const core::Buffer &buffer) { return buffer.GetHandle(); });
    vkCmdBindVertexBuffers(GetHandle(), first_binding, utility::ToU32(buffer_handles.size()), buffer_handles.data(),
                           offsets.data());
}

void CommandBuffer::BindIndexBuffer(const core::Buffer &buffer, VkDeviceSize offset, VkIndexType index_type) {
    vkCmdBindIndexBuffer(GetHandle(), buffer.GetHandle(), offset, index_type);
}

void CommandBuffer::SetViewportState(const ViewportState &state_info) { pipeline_state_.SetViewportState(state_info); }

void CommandBuffer::SetVertexInputState(const VertexInputState &state_info) {
    pipeline_state_.SetVertexInputState(state_info);
}

void CommandBuffer::SetInputAssemblyState(const InputAssemblyState &state_info) {
    pipeline_state_.SetInputAssemblyState(state_info);
}

void CommandBuffer::SetRasterizationState(const RasterizationState &state_info) {
    pipeline_state_.SetRasterizationState(state_info);
}

void CommandBuffer::SetMultisampleState(const MultisampleState &state_info) {
    pipeline_state_.SetMultisampleState(state_info);
}

void CommandBuffer::SetDepthStencilState(const DepthStencilState &state_info) {
    pipeline_state_.SetDepthStencilState(state_info);
}

void CommandBuffer::SetColorBlendState(const ColorBlendState &state_info) {
    pipeline_state_.SetColorBlendState(state_info);
}

void CommandBuffer::SetViewport(uint32_t first_viewport, const std::vector<VkViewport> &viewports) {
    vkCmdSetViewport(GetHandle(), first_viewport, utility::ToU32(viewports.size()), viewports.data());
}

void CommandBuffer::SetScissor(uint32_t first_scissor, const std::vector<VkRect2D> &scissors) {
    vkCmdSetScissor(GetHandle(), first_scissor, utility::ToU32(scissors.size()), scissors.data());
}

void CommandBuffer::SetLineWidth(float line_width) { vkCmdSetLineWidth(GetHandle(), line_width); }

void CommandBuffer::SetDepthBias(float depth_bias_constant_factor,
                                 float depth_bias_clamp,
                                 float depth_bias_slope_factor) {
    vkCmdSetDepthBias(GetHandle(), depth_bias_constant_factor, depth_bias_clamp, depth_bias_slope_factor);
}

void CommandBuffer::SetBlendConstants(const std::array<float, 4> &blend_constants) {
    vkCmdSetBlendConstants(GetHandle(), blend_constants.data());
}

void CommandBuffer::SetDepthBounds(float min_depth_bounds, float max_depth_bounds) {
    vkCmdSetDepthBounds(GetHandle(), min_depth_bounds, max_depth_bounds);
}

void CommandBuffer::Draw(uint32_t vertex_count,
                         uint32_t instance_count,
                         uint32_t first_vertex,
                         uint32_t first_instance) {
    Flush(VK_PIPELINE_BIND_POINT_GRAPHICS);

    vkCmdDraw(GetHandle(), vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::DrawIndexed(uint32_t index_count,
                                uint32_t instance_count,
                                uint32_t first_index,
                                int32_t vertex_offset,
                                uint32_t first_instance) {
    Flush(VK_PIPELINE_BIND_POINT_GRAPHICS);

    vkCmdDrawIndexed(GetHandle(), index_count, instance_count, first_index, vertex_offset, first_instance);
}

void CommandBuffer::DrawIndexedIndirect(const core::Buffer &buffer,
                                        VkDeviceSize offset,
                                        uint32_t draw_count,
                                        uint32_t stride) {
    Flush(VK_PIPELINE_BIND_POINT_GRAPHICS);

    vkCmdDrawIndexedIndirect(GetHandle(), buffer.GetHandle(), offset, draw_count, stride);
}

void CommandBuffer::Dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) {
    Flush(VK_PIPELINE_BIND_POINT_COMPUTE);

    vkCmdDispatch(GetHandle(), group_count_x, group_count_y, group_count_z);
}

void CommandBuffer::DispatchIndirect(const core::Buffer &buffer, VkDeviceSize offset) {
    Flush(VK_PIPELINE_BIND_POINT_COMPUTE);

    vkCmdDispatchIndirect(GetHandle(), buffer.GetHandle(), offset);
}

void CommandBuffer::UpdateBuffer(const core::Buffer &buffer, VkDeviceSize offset, const std::vector<uint8_t> &data) {
    vkCmdUpdateBuffer(GetHandle(), buffer.GetHandle(), offset, data.size(), data.data());
}

void CommandBuffer::BlitImage(const core::Image &src_img,
                              const core::Image &dst_img,
                              const std::vector<VkImageBlit> &regions) {
    vkCmdBlitImage(GetHandle(), src_img.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_img.GetHandle(),
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, utility::ToU32(regions.size()), regions.data(),
                   VK_FILTER_NEAREST);
}

void CommandBuffer::ResolveImage(const core::Image &src_img,
                                 const core::Image &dst_img,
                                 const std::vector<VkImageResolve> &regions) {
    vkCmdResolveImage(GetHandle(), src_img.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_img.GetHandle(),
                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, utility::ToU32(regions.size()), regions.data());
}

void CommandBuffer::CopyBuffer(const core::Buffer &src_buffer, const core::Buffer &dst_buffer, VkDeviceSize size) {
    VkBufferCopy copy_region = {};
    copy_region.size = size;
    vkCmdCopyBuffer(GetHandle(), src_buffer.GetHandle(), dst_buffer.GetHandle(), 1, &copy_region);
}

void CommandBuffer::CopyImage(const core::Image &src_img,
                              const core::Image &dst_img,
                              const std::vector<VkImageCopy> &regions) {
    vkCmdCopyImage(GetHandle(), src_img.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_img.GetHandle(),
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, utility::ToU32(regions.size()), regions.data());
}

void CommandBuffer::CopyBufferToImage(const core::Buffer &buffer,
                                      const core::Image &image,
                                      const std::vector<VkBufferImageCopy> &regions) {
    vkCmdCopyBufferToImage(GetHandle(), buffer.GetHandle(), image.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           utility::ToU32(regions.size()), regions.data());
}

void CommandBuffer::CopyImageToBuffer(const core::Image &image,
                                      VkImageLayout image_layout,
                                      const core::Buffer &buffer,
                                      const std::vector<VkBufferImageCopy> &regions) {
    vkCmdCopyImageToBuffer(GetHandle(), image.GetHandle(), image_layout, buffer.GetHandle(),
                           utility::ToU32(regions.size()), regions.data());
}

void CommandBuffer::ImageMemoryBarrier(const core::ImageView &image_view,
                                       const vox::ImageMemoryBarrier &memory_barrier) const {
    // Adjust barrier's subresource range for depth images
    auto subresource_range = image_view.GetSubresourceRange();
    auto format = image_view.GetFormat();
    if (IsDepthOnlyFormat(format)) {
        subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    } else if (IsDepthStencilFormat(format)) {
        subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkImageMemoryBarrier image_memory_barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    image_memory_barrier.oldLayout = memory_barrier.old_layout;
    image_memory_barrier.newLayout = memory_barrier.new_layout;
    image_memory_barrier.image = image_view.GetImage().GetHandle();
    image_memory_barrier.subresourceRange = subresource_range;
    image_memory_barrier.srcAccessMask = memory_barrier.src_access_mask;
    image_memory_barrier.dstAccessMask = memory_barrier.dst_access_mask;
    image_memory_barrier.srcQueueFamilyIndex = memory_barrier.old_queue_family;
    image_memory_barrier.dstQueueFamilyIndex = memory_barrier.new_queue_family;

    VkPipelineStageFlags src_stage_mask = memory_barrier.src_stage_mask;
    VkPipelineStageFlags dst_stage_mask = memory_barrier.dst_stage_mask;

    vkCmdPipelineBarrier(GetHandle(), src_stage_mask, dst_stage_mask, 0, 0, nullptr, 0, nullptr, 1,
                         &image_memory_barrier);
}

void CommandBuffer::BufferMemoryBarrier(const core::Buffer &buffer,
                                        VkDeviceSize offset,
                                        VkDeviceSize size,
                                        const vox::BufferMemoryBarrier &memory_barrier) {
    VkBufferMemoryBarrier buffer_memory_barrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
    buffer_memory_barrier.srcAccessMask = memory_barrier.src_access_mask;
    buffer_memory_barrier.dstAccessMask = memory_barrier.dst_access_mask;
    buffer_memory_barrier.buffer = buffer.GetHandle();
    buffer_memory_barrier.offset = offset;
    buffer_memory_barrier.size = size;

    VkPipelineStageFlags src_stage_mask = memory_barrier.src_stage_mask;
    VkPipelineStageFlags dst_stage_mask = memory_barrier.dst_stage_mask;

    vkCmdPipelineBarrier(GetHandle(), src_stage_mask, dst_stage_mask, 0, 0, nullptr, 1, &buffer_memory_barrier, 0,
                         nullptr);
}

void CommandBuffer::FlushPipelineState(VkPipelineBindPoint pipeline_bind_point) {
    // Create a new pipeline only if the graphics state changed
    if (!pipeline_state_.IsDirty()) {
        return;
    }

    pipeline_state_.ClearDirty();

    // Create and bind pipeline
    if (pipeline_bind_point == VK_PIPELINE_BIND_POINT_GRAPHICS) {
        pipeline_state_.SetRenderPass(*current_render_pass_.render_pass);
        auto &pipeline = GetDevice().GetResourceCache().RequestGraphicsPipeline(pipeline_state_);

        vkCmdBindPipeline(GetHandle(), pipeline_bind_point, pipeline.GetHandle());
    } else if (pipeline_bind_point == VK_PIPELINE_BIND_POINT_COMPUTE) {
        auto &pipeline = GetDevice().GetResourceCache().RequestComputePipeline(pipeline_state_);

        vkCmdBindPipeline(GetHandle(), pipeline_bind_point, pipeline.GetHandle());
    } else {
        throw std::runtime_error("Only graphics and compute pipeline bind points are supported now");
    }
}

void CommandBuffer::FlushDescriptorState(VkPipelineBindPoint pipeline_bind_point) {
    assert(command_pool_.GetRenderFrame() && "The command pool must be associated to a render frame");

    const auto &pipeline_layout = pipeline_state_.GetPipelineLayout();

    std::unordered_set<uint32_t> update_descriptor_sets;

    // Iterate over the shader sets to check if they have already been bound
    // If they have, add the set so that the command buffer later updates it
    for (auto &set_it : pipeline_layout.GetShaderSets()) {
        uint32_t descriptor_set_id = set_it.first;

        auto descriptor_set_layout_it = descriptor_set_layout_binding_state_.find(descriptor_set_id);

        if (descriptor_set_layout_it != descriptor_set_layout_binding_state_.end()) {
            if (descriptor_set_layout_it->second->GetHandle() !=
                pipeline_layout.GetDescriptorSetLayout(descriptor_set_id).GetHandle()) {
                update_descriptor_sets.emplace(descriptor_set_id);
            }
        }
    }

    // Validate that the bound descriptor set layouts exist in the pipeline layout
    for (auto set_it = descriptor_set_layout_binding_state_.begin();
         set_it != descriptor_set_layout_binding_state_.end();) {
        if (!pipeline_layout.HasDescriptorSetLayout(set_it->first)) {
            set_it = descriptor_set_layout_binding_state_.erase(set_it);
        } else {
            ++set_it;
        }
    }

    // Check if a descriptor set needs to be created
    if (resource_binding_state_.IsDirty() || !update_descriptor_sets.empty()) {
        resource_binding_state_.ClearDirty();

        // Iterate over all the resource sets bound by the command buffer
        for (auto &resource_set_it : resource_binding_state_.GetResourceSets()) {
            uint32_t descriptor_set_id = resource_set_it.first;
            auto &resource_set = resource_set_it.second;

            // Don't update resource set if it's not in the update list OR its state hasn't changed
            if (!resource_set.IsDirty() &&
                (update_descriptor_sets.find(descriptor_set_id) == update_descriptor_sets.end())) {
                continue;
            }

            // Clear dirty flag for resource set
            resource_binding_state_.ClearDirty(descriptor_set_id);

            // Skip resource set if a descriptor set layout doesn't exist for it
            if (!pipeline_layout.HasDescriptorSetLayout(descriptor_set_id)) {
                continue;
            }

            auto &descriptor_set_layout = pipeline_layout.GetDescriptorSetLayout(descriptor_set_id);

            // Make descriptor set layout bound for current set
            descriptor_set_layout_binding_state_[descriptor_set_id] = &descriptor_set_layout;

            BindingMap<VkDescriptorBufferInfo> buffer_infos;
            BindingMap<VkDescriptorImageInfo> image_infos;

            std::vector<uint32_t> dynamic_offsets;

            // The bindings we want to update before binding, if empty we update all bindings
            std::vector<uint32_t> bindings_to_update;

            // Iterate over all resource bindings
            for (auto &binding_it : resource_set.GetResourceBindings()) {
                auto binding_index = binding_it.first;
                auto &binding_resources = binding_it.second;

                // Check if binding exists in the pipeline layout
                if (auto binding_info = descriptor_set_layout.GetLayoutBinding(binding_index)) {
                    // If update after bind is enabled, we store the binding index of each binding that need to be
                    // updated before being bound
                    if (update_after_bind_ && !(descriptor_set_layout.GetLayoutBindingFlag(binding_index) &
                                                VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT)) {
                        bindings_to_update.push_back(binding_index);
                    }

                    // Iterate over all binding resources
                    for (auto &element_it : binding_resources) {
                        auto array_element = element_it.first;
                        auto &resource_info = element_it.second;

                        // Pointer references
                        auto &buffer = resource_info.buffer;
                        auto &sampler = resource_info.sampler;
                        auto &image_view = resource_info.image_view;

                        // Get buffer info
                        if (buffer != nullptr && IsBufferDescriptorType(binding_info->descriptorType)) {
                            VkDescriptorBufferInfo buffer_info{};

                            buffer_info.buffer = resource_info.buffer->GetHandle();
                            buffer_info.offset = resource_info.offset;
                            buffer_info.range = resource_info.range;

                            if (IsDynamicBufferDescriptorType(binding_info->descriptorType)) {
                                dynamic_offsets.push_back(utility::ToU32(buffer_info.offset));

                                buffer_info.offset = 0;
                            }

                            buffer_infos[binding_index][array_element] = buffer_info;
                        }

                        // Get image info
                        else if (image_view != nullptr || sampler != VK_NULL_HANDLE) {
                            // Can be null for input attachments
                            VkDescriptorImageInfo image_info{};
                            image_info.sampler = sampler ? sampler->GetHandle() : VK_NULL_HANDLE;
                            image_info.imageView = image_view->GetHandle();

                            if (image_view != nullptr) {
                                // Add image layout info based on descriptor type
                                switch (binding_info->descriptorType) {
                                    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                                        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                        break;
                                    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                                        if (IsDepthStencilFormat(image_view->GetFormat())) {
                                            image_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                                        } else {
                                            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                        }
                                        break;
                                    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                                        image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                                        break;

                                    default:
                                        continue;
                                }
                            }

                            image_infos[binding_index][array_element] = image_info;
                        }
                    }
                }
            }

            // Request a descriptor set from the render frame, and write the buffer infos and image infos of all the
            // specified bindings
            auto &descriptor_set = command_pool_.GetRenderFrame()->RequestDescriptorSet(
                    descriptor_set_layout, buffer_infos, image_infos, command_pool_.GetThreadIndex());
            descriptor_set.Update(bindings_to_update);

            VkDescriptorSet descriptor_set_handle = descriptor_set.GetHandle();

            // Bind descriptor set
            vkCmdBindDescriptorSets(GetHandle(), pipeline_bind_point, pipeline_layout.GetHandle(), descriptor_set_id, 1,
                                    &descriptor_set_handle, utility::ToU32(dynamic_offsets.size()),
                                    dynamic_offsets.data());
        }
    }
}

void CommandBuffer::FlushPushConstants() {
    if (stored_push_constants_.empty()) {
        return;
    }

    const PipelineLayout &pipeline_layout = pipeline_state_.GetPipelineLayout();

    VkShaderStageFlags shader_stage =
            pipeline_layout.GetPushConstantRangeStage(utility::ToU32(stored_push_constants_.size()));

    if (shader_stage) {
        vkCmdPushConstants(GetHandle(), pipeline_layout.GetHandle(), shader_stage, 0,
                           utility::ToU32(stored_push_constants_.size()), stored_push_constants_.data());
    } else {
        LOGW("Push constant range [{}, {}] not found", 0, stored_push_constants_.size())
    }

    stored_push_constants_.clear();
}

CommandBuffer::State CommandBuffer::GetState() const { return state_; }

void CommandBuffer::SetUpdateAfterBind(bool update_after_bind) { update_after_bind_ = update_after_bind; }

const CommandBuffer::RenderPassBinding &CommandBuffer::GetCurrentRenderPass() const { return current_render_pass_; }

uint32_t CommandBuffer::GetCurrentSubpassIndex() const { return pipeline_state_.GetSubpassIndex(); }

bool CommandBuffer::IsRenderSizeOptimal(const VkExtent2D &extent, const VkRect2D &render_area) const {
    auto render_area_granularity = current_render_pass_.render_pass->GetRenderAreaGranularity();

    return ((render_area.offset.x % render_area_granularity.width == 0) &&
            (render_area.offset.y % render_area_granularity.height == 0) &&
            ((render_area.extent.width % render_area_granularity.width == 0) ||
             (render_area.offset.x + render_area.extent.width == extent.width)) &&
            ((render_area.extent.height % render_area_granularity.height == 0) ||
             (render_area.offset.y + render_area.extent.height == extent.height)));
}

void CommandBuffer::ResetQueryPool(const QueryPool &query_pool, uint32_t first_query, uint32_t query_count) {
    vkCmdResetQueryPool(GetHandle(), query_pool.GetHandle(), first_query, query_count);
}

void CommandBuffer::BeginQuery(const QueryPool &query_pool, uint32_t query, VkQueryControlFlags flags) {
    vkCmdBeginQuery(GetHandle(), query_pool.GetHandle(), query, flags);
}

void CommandBuffer::EndQuery(const QueryPool &query_pool, uint32_t query) {
    vkCmdEndQuery(GetHandle(), query_pool.GetHandle(), query);
}

void CommandBuffer::WriteTimestamp(VkPipelineStageFlagBits pipeline_stage,
                                   const QueryPool &query_pool,
                                   uint32_t query) {
    vkCmdWriteTimestamp(GetHandle(), pipeline_stage, query_pool.GetHandle(), query);
}

VkResult CommandBuffer::Reset(ResetMode reset_mode) {
    VkResult result = VK_SUCCESS;

    assert(reset_mode == command_pool_.GetResetMode() &&
           "Command buffer reset mode must match the one used by the pool to allocate it");

    state_ = State::INITIAL;

    if (reset_mode == ResetMode::RESET_INDIVIDUALLY) {
        result = vkResetCommandBuffer(handle_, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }

    return result;
}

RenderPass &CommandBuffer::GetRenderPass(const vox::RenderTarget &render_target,
                                         const std::vector<LoadStoreInfo> &load_store_infos,
                                         const std::vector<std::unique_ptr<Subpass>> &subpasses) {
    // Create render pass
    assert(!subpasses.empty() && "Cannot create a render pass without any subpass");

    std::vector<vox::SubpassInfo> subpass_infos(subpasses.size());
    auto subpass_info_it = subpass_infos.begin();
    for (auto &subpass : subpasses) {
        subpass_info_it->input_attachments = subpass->GetInputAttachments();
        subpass_info_it->output_attachments = subpass->GetOutputAttachments();
        subpass_info_it->color_resolve_attachments = subpass->GetColorResolveAttachments();
        subpass_info_it->disable_depth_stencil_attachment = subpass->GetDisableDepthStencilAttachment();
        subpass_info_it->depth_stencil_resolve_mode = subpass->GetDepthStencilResolveMode();
        subpass_info_it->depth_stencil_resolve_attachment = subpass->GetDepthStencilResolveAttachment();
        subpass_info_it->debug_name = subpass->GetDebugName();

        ++subpass_info_it;
    }

    return GetDevice().GetResourceCache().RequestRenderPass(render_target.GetAttachments(), load_store_infos,
                                                            subpass_infos);
}

}  // namespace vox
