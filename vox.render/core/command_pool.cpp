//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/command_pool.h"

#include "vox.render/core/device.h"
#include "vox.render/rendering/render_frame.h"

namespace vox {
CommandPool::CommandPool(Device &d,
                         uint32_t queue_family_index,
                         RenderFrame *render_frame,
                         size_t thread_index,
                         CommandBuffer::ResetMode reset_mode)
    : device_{d}, render_frame_{render_frame}, thread_index_{thread_index}, reset_mode_{reset_mode} {
    VkCommandPoolCreateFlags flags;
    switch (reset_mode) {
        case CommandBuffer::ResetMode::RESET_INDIVIDUALLY:
        case CommandBuffer::ResetMode::ALWAYS_ALLOCATE:
            flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            break;
        case CommandBuffer::ResetMode::RESET_POOL:
        default:
            flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            break;
    }

    VkCommandPoolCreateInfo create_info{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};

    create_info.queueFamilyIndex = queue_family_index;
    create_info.flags = flags;

    auto result = vkCreateCommandPool(device_.GetHandle(), &create_info, nullptr, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Failed to create command pool"};
    }
}

CommandPool::~CommandPool() {
    primary_command_buffers_.clear();
    secondary_command_buffers_.clear();

    // Destroy command pool
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device_.GetHandle(), handle_, nullptr);
    }
}

CommandPool::CommandPool(CommandPool &&other) noexcept
    : device_{other.device_},
      handle_{other.handle_},
      queue_family_index_{other.queue_family_index_},
      primary_command_buffers_{std::move(other.primary_command_buffers_)},
      active_primary_command_buffer_count_{other.active_primary_command_buffer_count_},
      secondary_command_buffers_{std::move(other.secondary_command_buffers_)},
      active_secondary_command_buffer_count_{other.active_secondary_command_buffer_count_},
      render_frame_{other.render_frame_},
      thread_index_{other.thread_index_},
      reset_mode_{other.reset_mode_} {
    other.handle_ = VK_NULL_HANDLE;

    other.queue_family_index_ = 0;

    other.active_primary_command_buffer_count_ = 0;

    other.active_secondary_command_buffer_count_ = 0;
}

Device &CommandPool::GetDevice() { return device_; }

uint32_t CommandPool::GetQueueFamilyIndex() const { return queue_family_index_; }

VkCommandPool CommandPool::GetHandle() const { return handle_; }

RenderFrame *CommandPool::GetRenderFrame() { return render_frame_; }

size_t CommandPool::GetThreadIndex() const { return thread_index_; }

VkResult CommandPool::ResetPool() {
    VkResult result = VK_SUCCESS;

    switch (reset_mode_) {
        case CommandBuffer::ResetMode::RESET_INDIVIDUALLY: {
            result = ResetCommandBuffers();

            break;
        }
        case CommandBuffer::ResetMode::RESET_POOL: {
            result = vkResetCommandPool(device_.GetHandle(), handle_, 0);

            if (result != VK_SUCCESS) {
                return result;
            }

            result = ResetCommandBuffers();

            break;
        }
        case CommandBuffer::ResetMode::ALWAYS_ALLOCATE: {
            primary_command_buffers_.clear();
            active_primary_command_buffer_count_ = 0;

            secondary_command_buffers_.clear();
            active_secondary_command_buffer_count_ = 0;

            break;
        }
        default:
            throw std::runtime_error("Unknown reset mode for command pools");
    }

    return result;
}

VkResult CommandPool::ResetCommandBuffers() {
    VkResult result = VK_SUCCESS;

    for (auto &cmd_buf : primary_command_buffers_) {
        result = cmd_buf->Reset(reset_mode_);

        if (result != VK_SUCCESS) {
            return result;
        }
    }

    active_primary_command_buffer_count_ = 0;

    for (auto &cmd_buf : secondary_command_buffers_) {
        result = cmd_buf->Reset(reset_mode_);

        if (result != VK_SUCCESS) {
            return result;
        }
    }

    active_secondary_command_buffer_count_ = 0;

    return result;
}

CommandBuffer &CommandPool::RequestCommandBuffer(VkCommandBufferLevel level) {
    if (level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
        if (active_primary_command_buffer_count_ < primary_command_buffers_.size()) {
            return *primary_command_buffers_.at(active_primary_command_buffer_count_++);
        }

        primary_command_buffers_.emplace_back(std::make_unique<CommandBuffer>(*this, level));

        active_primary_command_buffer_count_++;

        return *primary_command_buffers_.back();
    } else {
        if (active_secondary_command_buffer_count_ < secondary_command_buffers_.size()) {
            return *secondary_command_buffers_.at(active_secondary_command_buffer_count_++);
        }

        secondary_command_buffers_.emplace_back(std::make_unique<CommandBuffer>(*this, level));

        active_secondary_command_buffer_count_++;

        return *secondary_command_buffers_.back();
    }
}

CommandBuffer::ResetMode CommandPool::GetResetMode() const { return reset_mode_; }

}  // namespace vox
