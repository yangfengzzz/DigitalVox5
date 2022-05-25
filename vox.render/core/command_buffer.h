//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <list>

#include "vox.base/helper.h"
#include "vox.base/logging.h"
#include "vox.render/core/buffer.h"
#include "vox.render/core/image.h"
#include "vox.render/core/image_view.h"
#include "vox.render/core/query_pool.h"
#include "vox.render/core/sampler.h"
#include "vox.render/core/vulkan_resource.h"
#include "vox.render/rendering/pipeline_state.h"
#include "vox.render/rendering/render_target.h"
#include "vox.render/resource_binding_state.h"
#include "vox.render/vk_common.h"

namespace vox {
class CommandPool;

class DescriptorSet;

class Framebuffer;

class Pipeline;

class PipelineLayout;

class PipelineState;

class RenderTarget;

class Subpass;

/**
 * @brief Helper class to manage and record a command buffer, building and
 *        keeping track of pipeline state and resource bindings
 */
class CommandBuffer : public core::VulkanResource<VkCommandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER> {
public:
    enum class ResetMode {
        RESET_POOL,
        RESET_INDIVIDUALLY,
        ALWAYS_ALLOCATE,
    };

    enum class State {
        INVALID,
        INITIAL,
        RECORDING,
        EXECUTABLE,
    };

    /**
     * @brief Helper structure used to track render pass state
     */
    struct RenderPassBinding {
        const RenderPass *render_pass;

        const Framebuffer *framebuffer;
    };

    CommandBuffer(CommandPool &command_pool, VkCommandBufferLevel level);

    CommandBuffer(const CommandBuffer &) = delete;

    CommandBuffer(CommandBuffer &&other) noexcept;

    ~CommandBuffer() override;

    CommandBuffer &operator=(const CommandBuffer &) = delete;

    CommandBuffer &operator=(CommandBuffer &&) = delete;

    [[nodiscard]] bool IsRecording() const;

    /**
     * @brief Flushes the command buffer, pushing the new changes
     * @param pipeline_bind_point The type of pipeline we want to flush
     */
    void Flush(VkPipelineBindPoint pipeline_bind_point);

    /**
     * @brief Sets the command buffer so that it is ready for recording
     *        If it is a secondary command buffer, a pointer to the
     *        primary command buffer it inherits from must be provided
     * @param flags Usage behavior for the command buffer
     * @param primary_cmd_buf (optional)
     * @return Whether it succeeded or not
     */
    VkResult Begin(VkCommandBufferUsageFlags flags, CommandBuffer *primary_cmd_buf = nullptr);

    /**
     * @brief Sets the command buffer so that it is ready for recording
     *        If it is a secondary command buffer, pointers to the
     *        render pass and framebuffer as well as subpass index must be provided
     * @param flags Usage behavior for the command buffer
     * @param render_pass render_pass
     * @param framebuffer framebuffer
     * @param subpass_index subpass_index
     * @return Whether it succeeded or not
     */
    VkResult Begin(VkCommandBufferUsageFlags flags,
                   const RenderPass *render_pass,
                   const Framebuffer *framebuffer,
                   uint32_t subpass_index);

    VkResult End();

    void Clear(VkClearAttachment info, VkClearRect rect);

    void BeginRenderPass(const RenderTarget &render_target,
                         const std::vector<LoadStoreInfo> &load_store_infos,
                         const std::vector<VkClearValue> &clear_values,
                         const std::vector<std::unique_ptr<Subpass>> &subpasses,
                         VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

    void BeginRenderPass(const RenderTarget &render_target,
                         const RenderPass &render_pass,
                         const Framebuffer &framebuffer,
                         const std::vector<VkClearValue> &clear_values,
                         VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

    void NextSubpass();

    void ExecuteCommands(CommandBuffer &secondary_command_buffer);

    void ExecuteCommands(std::vector<CommandBuffer *> &secondary_command_buffers);

    void EndRenderPass();

    void BindPipelineLayout(PipelineLayout &pipeline_layout);

    template <class T>
    void SetSpecializationConstant(uint32_t constant_id, const T &data);

    void SetSpecializationConstant(uint32_t constant_id, const std::vector<uint8_t> &data);

    /**
     * @brief Records byte data into the command buffer to be pushed as push constants to each draw call
     * @param values The byte data to store
     */
    void PushConstants(const std::vector<uint8_t> &values);

    template <typename T>
    void PushConstants(const T &value) {
        auto data = to_bytes(value);

        uint32_t size = ToU32(stored_push_constants_.size() + data.size());

        if (size > max_push_constants_size_) {
            LOGE("Push constant limit exceeded ({} / {} bytes)", size, max_push_constants_size_)
            throw std::runtime_error("Cannot overflow push constant limit");
        }

        stored_push_constants_.insert(stored_push_constants_.end(), data.begin(), data.end());
    }

    void BindBuffer(const core::Buffer &buffer,
                    VkDeviceSize offset,
                    VkDeviceSize range,
                    uint32_t set,
                    uint32_t binding,
                    uint32_t array_element);

    void BindImage(const core::ImageView &image_view,
                   const core::Sampler &sampler,
                   uint32_t set,
                   uint32_t binding,
                   uint32_t array_element);

    void BindImage(const core::ImageView &image_view, uint32_t set, uint32_t binding, uint32_t array_element);

    void BindInput(const core::ImageView &image_view, uint32_t set, uint32_t binding, uint32_t array_element);

    void BindVertexBuffers(uint32_t first_binding,
                           const std::vector<std::reference_wrapper<const vox::core::Buffer>> &buffers,
                           const std::vector<VkDeviceSize> &offsets);

    void BindIndexBuffer(const core::Buffer &buffer, VkDeviceSize offset, VkIndexType index_type);

    void SetViewportState(const ViewportState &state_info);

    void SetVertexInputState(const VertexInputState &state_info);

    void SetInputAssemblyState(const InputAssemblyState &state_info);

    void SetRasterizationState(const RasterizationState &state_info);

    void SetMultisampleState(const MultisampleState &state_info);

    void SetDepthStencilState(const DepthStencilState &state_info);

    void SetColorBlendState(const ColorBlendState &state_info);

    void SetViewport(uint32_t first_viewport, const std::vector<VkViewport> &viewports);

    void SetScissor(uint32_t first_scissor, const std::vector<VkRect2D> &scissors);

    void SetLineWidth(float line_width);

    void SetDepthBias(float depth_bias_constant_factor, float depth_bias_clamp, float depth_bias_slope_factor);

    void SetBlendConstants(const std::array<float, 4> &blend_constants);

    void SetDepthBounds(float min_depth_bounds, float max_depth_bounds);

    void Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);

    void DrawIndexed(uint32_t index_count,
                     uint32_t instance_count,
                     uint32_t first_index,
                     int32_t vertex_offset,
                     uint32_t first_instance);

    void DrawIndexedIndirect(const core::Buffer &buffer, VkDeviceSize offset, uint32_t draw_count, uint32_t stride);

    void Dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);

    void DispatchIndirect(const core::Buffer &buffer, VkDeviceSize offset);

    void UpdateBuffer(const core::Buffer &buffer, VkDeviceSize offset, const std::vector<uint8_t> &data);

    void BlitImage(const core::Image &src_img, const core::Image &dst_img, const std::vector<VkImageBlit> &regions);

    void ResolveImage(const core::Image &src_img,
                      const core::Image &dst_img,
                      const std::vector<VkImageResolve> &regions);

    void CopyBuffer(const core::Buffer &src_buffer, const core::Buffer &dst_buffer, VkDeviceSize size);

    void CopyImage(const core::Image &src_img, const core::Image &dst_img, const std::vector<VkImageCopy> &regions);

    void CopyBufferToImage(const core::Buffer &buffer,
                           const core::Image &image,
                           const std::vector<VkBufferImageCopy> &regions);

    void CopyImageToBuffer(const core::Image &image,
                           VkImageLayout image_layout,
                           const core::Buffer &buffer,
                           const std::vector<VkBufferImageCopy> &regions);

    void ImageMemoryBarrier(const core::ImageView &image_view, const ImageMemoryBarrier &memory_barrier) const;

    void BufferMemoryBarrier(const core::Buffer &buffer,
                             VkDeviceSize offset,
                             VkDeviceSize size,
                             const BufferMemoryBarrier &memory_barrier);

    [[nodiscard]] State GetState() const;

    void SetUpdateAfterBind(bool update_after_bind);

    void ResetQueryPool(const QueryPool &query_pool, uint32_t first_query, uint32_t query_count);

    void BeginQuery(const QueryPool &query_pool, uint32_t query, VkQueryControlFlags flags);

    void EndQuery(const QueryPool &query_pool, uint32_t query);

    void WriteTimestamp(VkPipelineStageFlagBits pipeline_stage, const QueryPool &query_pool, uint32_t query);

    /**
     * @brief Reset the command buffer to a state where it can be recorded to
     * @param reset_mode How to reset the buffer, should match the one used by the pool to allocate it
     */
    VkResult Reset(ResetMode reset_mode);

    RenderPass &GetRenderPass(const vox::RenderTarget &render_target,
                              const std::vector<LoadStoreInfo> &load_store_infos,
                              const std::vector<std::unique_ptr<Subpass>> &subpasses);

    const VkCommandBufferLevel level_;

private:
    State state_{State::INITIAL};

    CommandPool &command_pool_;

    RenderPassBinding current_render_pass_{};

    PipelineState pipeline_state_;

    ResourceBindingState resource_binding_state_;

    std::vector<uint8_t> stored_push_constants_;

    uint32_t max_push_constants_size_{};

    VkExtent2D last_framebuffer_extent_{};

    VkExtent2D last_render_area_extent_{};

    // If true, it becomes the responsibility of the caller to update ANY descriptor bindings
    // that contain update after bind, as they won't be implicitly updated
    bool update_after_bind_{false};

    std::unordered_map<uint32_t, DescriptorSetLayout *> descriptor_set_layout_binding_state_;

    [[nodiscard]] const RenderPassBinding &GetCurrentRenderPass() const;

    [[nodiscard]] uint32_t GetCurrentSubpassIndex() const;

    /**
     * @brief Check that the render area is an optimal size by comparing to the render area granularity
     */
    [[nodiscard]] bool IsRenderSizeOptimal(const VkExtent2D &extent, const VkRect2D &render_area) const;

    /**
     * @brief Flush the pipelines state
     */
    void FlushPipelineState(VkPipelineBindPoint pipeline_bind_point);

    /**
     * @brief Flush the descriptor set state
     */
    void FlushDescriptorState(VkPipelineBindPoint pipeline_bind_point);

    /**
     * @brief Flush the push constant state
     */
    void FlushPushConstants();
};

template <class T>
inline void CommandBuffer::SetSpecializationConstant(uint32_t constant_id, const T &data) {
    SetSpecializationConstant(constant_id, to_bytes(data));
}

template <>
inline void CommandBuffer::SetSpecializationConstant<bool>(std::uint32_t constant_id, const bool &data) {
    SetSpecializationConstant(constant_id, utility::ToBytes(utility::ToU32(data)));
}

}  // namespace vox
