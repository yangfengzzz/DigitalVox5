//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/render_frame.h"

#include "vox.base/logging.h"
#include "vox.render/utils.h"

namespace vox {
RenderFrame::RenderFrame(Device &device, std::unique_ptr<RenderTarget> &&render_target, size_t thread_count)
    : device_{device},
      fence_pool_{device},
      semaphore_pool_{device},
      swapchain_render_target_{std::move(render_target)},
      thread_count_{thread_count} {
    for (auto &usage_it : supported_usage_map_) {
        std::vector<std::pair<BufferPool, BufferBlock *>> usage_buffer_pools;
        for (size_t i = 0; i < thread_count; ++i) {
            usage_buffer_pools.emplace_back(
                    BufferPool{device, buffer_pool_block_size_ * 1024 * usage_it.second, usage_it.first}, nullptr);
        }

        auto res_ins_it = buffer_pools_.emplace(usage_it.first, std::move(usage_buffer_pools));

        if (!res_ins_it.second) {
            throw std::runtime_error("Failed to insert buffer pool");
        }
    }

    for (size_t i = 0; i < thread_count; ++i) {
        descriptor_pools_.push_back(std::make_unique<std::unordered_map<std::size_t, DescriptorPool>>());
        descriptor_sets_.push_back(std::make_unique<std::unordered_map<std::size_t, DescriptorSet>>());
    }
}

Device &RenderFrame::GetDevice() { return device_; }

void RenderFrame::UpdateRenderTarget(std::unique_ptr<RenderTarget> &&render_target) {
    swapchain_render_target_ = std::move(render_target);
}

void RenderFrame::Reset() {
    VK_CHECK(fence_pool_.wait());

    fence_pool_.reset();

    for (auto &command_pools_per_queue : command_pools_) {
        for (auto &command_pool : command_pools_per_queue.second) {
            command_pool->ResetPool();
        }
    }

    for (auto &buffer_pools_per_usage : buffer_pools_) {
        for (auto &buffer_pool : buffer_pools_per_usage.second) {
            buffer_pool.first.Reset();
            buffer_pool.second = nullptr;
        }
    }

    semaphore_pool_.Reset();
}

std::vector<std::unique_ptr<CommandPool>> &RenderFrame::GetCommandPools(const Queue &queue,
                                                                        CommandBuffer::ResetMode reset_mode) {
    auto command_pool_it = command_pools_.find(queue.GetFamilyIndex());

    if (command_pool_it != command_pools_.end()) {
        if (command_pool_it->second.at(0)->GetResetMode() != reset_mode) {
            device_.WaitIdle();

            // Delete pools
            command_pools_.erase(command_pool_it);
        } else {
            return command_pool_it->second;
        }
    }

    std::vector<std::unique_ptr<CommandPool>> queue_command_pools;
    for (size_t i = 0; i < thread_count_; i++) {
        queue_command_pools.push_back(
                std::make_unique<CommandPool>(device_, queue.GetFamilyIndex(), this, i, reset_mode));
    }

    auto res_ins_it = command_pools_.emplace(queue.GetFamilyIndex(), std::move(queue_command_pools));

    if (!res_ins_it.second) {
        throw std::runtime_error("Failed to insert command pool");
    }

    command_pool_it = res_ins_it.first;

    return command_pool_it->second;
}

const FencePool &RenderFrame::GetFencePool() const { return fence_pool_; }

VkFence RenderFrame::RequestFence() { return fence_pool_.request_fence(); }

const SemaphorePool &RenderFrame::GetSemaphorePool() const { return semaphore_pool_; }

VkSemaphore RenderFrame::RequestSemaphore() { return semaphore_pool_.RequestSemaphore(); }

VkSemaphore RenderFrame::RequestSemaphoreWithOwnership() { return semaphore_pool_.RequestSemaphoreWithOwnership(); }

void RenderFrame::ReleaseOwnedSemaphore(VkSemaphore semaphore) { semaphore_pool_.ReleaseOwnedSemaphore(semaphore); }

RenderTarget &RenderFrame::GetRenderTarget() { return *swapchain_render_target_; }

const RenderTarget &RenderFrame::GetRenderTargetConst() const { return *swapchain_render_target_; }

CommandBuffer &RenderFrame::RequestCommandBuffer(const Queue &queue,
                                                 CommandBuffer::ResetMode reset_mode,
                                                 VkCommandBufferLevel level,
                                                 size_t thread_index) {
    assert(thread_index < thread_count_ && "Thread index is out of bounds");

    auto &command_pools = GetCommandPools(queue, reset_mode);

    auto command_pool_it = std::find_if(command_pools.begin(), command_pools.end(),
                                        [&thread_index](std::unique_ptr<CommandPool> &cmd_pool) {
                                            return cmd_pool->GetThreadIndex() == thread_index;
                                        });

    return (*command_pool_it)->RequestCommandBuffer(level);
}

DescriptorSet &RenderFrame::RequestDescriptorSet(DescriptorSetLayout &descriptor_set_layout,
                                                 const BindingMap<VkDescriptorBufferInfo> &buffer_infos,
                                                 const BindingMap<VkDescriptorImageInfo> &image_infos,
                                                 size_t thread_index) {
    assert(thread_index < thread_count_ && "Thread index is out of bounds");

    auto &descriptor_pool =
            RequestResource(device_, nullptr, *descriptor_pools_.at(thread_index), descriptor_set_layout);
    return RequestResource(device_, nullptr, *descriptor_sets_.at(thread_index), descriptor_set_layout, descriptor_pool,
                           buffer_infos, image_infos);
}

void RenderFrame::UpdateDescriptorSets(size_t thread_index) {
    auto &thread_descriptor_sets = *descriptor_sets_.at(thread_index);
    for (auto &descriptor_set_it : thread_descriptor_sets) {
        descriptor_set_it.second.Update();
    }
}

void RenderFrame::ClearDescriptors() {
    for (auto &desc_sets_per_thread : descriptor_sets_) {
        desc_sets_per_thread->clear();
    }

    for (auto &desc_pools_per_thread : descriptor_pools_) {
        for (auto &desc_pool : *desc_pools_per_thread) {
            desc_pool.second.Reset();
        }
    }
}

void RenderFrame::SetBufferAllocationStrategy(BufferAllocationStrategy new_strategy) {
    buffer_allocation_strategy_ = new_strategy;
}

BufferAllocation RenderFrame::AllocateBuffer(VkBufferUsageFlags usage, VkDeviceSize size, size_t thread_index) {
    assert(thread_index < thread_count_ && "Thread index is out of bounds");

    uint32_t block_multiplier = supported_usage_map_.at(usage);

    if (size > buffer_pool_block_size_ * 1024 * block_multiplier) {
        LOGE("Trying to allocate {} buffer of size {}KB which is larger than the buffer pool block size ({} KB)!",
             BufferUsageToString(usage), size / 1024, buffer_pool_block_size_ * block_multiplier)
        throw std::runtime_error("Couldn't allocate render frame buffer.");
    }

    // Find a pool for this usage
    auto buffer_pool_it = buffer_pools_.find(usage);
    if (buffer_pool_it == buffer_pools_.end()) {
        LOGE("No buffer pool for buffer usage {}", usage)
        return BufferAllocation{};
    }

    auto &buffer_pool = buffer_pool_it->second.at(thread_index).first;
    auto &buffer_block = buffer_pool_it->second.at(thread_index).second;

    if (buffer_allocation_strategy_ == BufferAllocationStrategy::ONE_ALLOCATION_PER_BUFFER || !buffer_block) {
        // If there is no block associated with the pool, we are creating a buffer for each allocation,
        // request a new buffer block
        buffer_block = &buffer_pool.RequestBufferBlock(utility::ToU32(size));
    }

    auto data = buffer_block->Allocate(utility::ToU32(size));

    // Check if the buffer block can allocate the requested size
    if (data.Empty()) {
        buffer_block = &buffer_pool.RequestBufferBlock(utility::ToU32(size));

        data = buffer_block->Allocate(utility::ToU32(size));
    }

    return data;
}

}  // namespace vox
