//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/buffer_pool.h"
#include "vox.render/core/buffer.h"
#include "vox.render/core/command_buffer.h"
#include "vox.render/core/command_pool.h"
#include "vox.render/core/device.h"
#include "vox.render/core/image.h"
#include "vox.render/core/query_pool.h"
#include "vox.render/core/queue.h"
#include "vox.render/fence_pool.h"
#include "vox.render/rendering/render_target.h"
#include "vox.render/resource_caching.h"
#include "vox.render/semaphore_pool.h"
#include "vox.render/vk_common.h"

namespace vox {
enum BufferAllocationStrategy { ONE_ALLOCATION_PER_BUFFER, MULTIPLE_ALLOCATIONS_PER_BUFFER };

/**
 * @brief RenderFrame is a container for per-frame data, including BufferPool objects,
 * synchronization primitives (semaphores, fences) and the swapchain RenderTarget.
 *
 * When creating a RenderTarget, we need to provide images that will be used as attachments
 * within a RenderPass. The RenderFrame is responsible for creating a RenderTarget using
 * RenderTarget::CreateFunc. A custom RenderTarget::CreateFunc can be provided if a different
 * render target is required.
 *
 * A RenderFrame cannot be destroyed individually since frames are managed by the RenderContext,
 * the whole context must be destroyed. This is because each RenderFrame holds Vulkan objects
 * such as the swapchain image.
 */
class RenderFrame {
public:
    /**
     * @brief Block size of a buffer pool in kilobytes
     */
    static constexpr uint32_t buffer_pool_block_size_ = 256;

    // A map of the supported usages to a multiplier for the BUFFER_POOL_BLOCK_SIZE
    const std::unordered_map<VkBufferUsageFlags, uint32_t> supported_usage_map_ = {
            {VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 1},
            {VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 2},  // x2 the size of BUFFER_POOL_BLOCK_SIZE since SSBOs are normally
                                                      // much larger than other types of buffers
            {VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 1},
            {VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 1}};

    RenderFrame(Device &device, std::unique_ptr<RenderTarget> &&render_target, size_t thread_count = 1);

    RenderFrame(const RenderFrame &) = delete;

    RenderFrame(RenderFrame &&) = delete;

    RenderFrame &operator=(const RenderFrame &) = delete;

    RenderFrame &operator=(RenderFrame &&) = delete;

    void Reset();

    Device &GetDevice();

    [[nodiscard]] const FencePool &GetFencePool() const;

    VkFence RequestFence();

    [[nodiscard]] const SemaphorePool &GetSemaphorePool() const;

    VkSemaphore RequestSemaphore();

    VkSemaphore RequestSemaphoreWithOwnership();

    void ReleaseOwnedSemaphore(VkSemaphore semaphore);

    /**
     * @brief Called when the swapchain changes
     * @param render_target A new render target with updated images
     */
    void UpdateRenderTarget(std::unique_ptr<RenderTarget> &&render_target);

    RenderTarget &GetRenderTarget();

    [[nodiscard]] const RenderTarget &GetRenderTargetConst() const;

    /**
     * @brief Requests a command buffer to the command pool of the active frame
     *        A frame should be active at the moment of requesting it
     * @param queue The queue command buffers will be submitted on
     * @param reset_mode Indicate how the command buffer will be used, may trigger a
     *        pool re-creation to set necessary flags
     * @param level Command buffer level, either primary or secondary
     * @param thread_index Selects the thread's command pool used to manage the buffer
     * @return A command buffer related to the current active frame
     */
    CommandBuffer &RequestCommandBuffer(const Queue &queue,
                                        CommandBuffer::ResetMode reset_mode = CommandBuffer::ResetMode::RESET_POOL,
                                        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                        size_t thread_index = 0);

    DescriptorSet &RequestDescriptorSet(DescriptorSetLayout &descriptor_set_layout,
                                        const BindingMap<VkDescriptorBufferInfo> &buffer_infos,
                                        const BindingMap<VkDescriptorImageInfo> &image_infos,
                                        size_t thread_index = 0);

    void ClearDescriptors();

    /**
     * @brief Sets a new buffer allocation strategy
     * @param new_strategy The new buffer allocation strategy
     */
    void SetBufferAllocationStrategy(BufferAllocationStrategy new_strategy);

    /**
     * @param usage Usage of the buffer
     * @param size Amount of memory required
     * @param thread_index Index of the buffer pool to be used by the current thread
     * @return The requested allocation, it may be empty
     */
    BufferAllocation AllocateBuffer(VkBufferUsageFlags usage, VkDeviceSize size, size_t thread_index = 0);

    /**
     * @brief Updates all the descriptor sets in the current frame at a specific thread index
     */
    void UpdateDescriptorSets(size_t thread_index = 0);

private:
    Device &device_;

    /**
     * @brief Retrieve the frame's command pool(s)
     * @param queue The queue command buffers will be submitted on
     * @param reset_mode Indicate how the command buffers will be reset after execution,
     *        may trigger a pool re-creation to set necessary flags
     * @return The frame's command pool(s)
     */
    std::vector<std::unique_ptr<CommandPool>> &GetCommandPools(const Queue &queue, CommandBuffer::ResetMode reset_mode);

    /// Commands pools associated to the frame
    std::map<uint32_t, std::vector<std::unique_ptr<CommandPool>>> command_pools_;

    /// Descriptor pools for the frame
    std::vector<std::unique_ptr<std::unordered_map<std::size_t, DescriptorPool>>> descriptor_pools_;

    /// Descriptor sets for the frame
    std::vector<std::unique_ptr<std::unordered_map<std::size_t, DescriptorSet>>> descriptor_sets_;

    FencePool fence_pool_;

    SemaphorePool semaphore_pool_;

    size_t thread_count_;

    std::unique_ptr<RenderTarget> swapchain_render_target_;

    BufferAllocationStrategy buffer_allocation_strategy_{BufferAllocationStrategy::MULTIPLE_ALLOCATIONS_PER_BUFFER};

    std::map<VkBufferUsageFlags, std::vector<std::pair<BufferPool, BufferBlock *>>> buffer_pools_;
};

}  // namespace vox
