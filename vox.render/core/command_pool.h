//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/command_buffer.h"
#include "vox.render/vk_common.h"

namespace vox {
class Device;

class RenderFrame;

class CommandPool {
public:
    CommandPool(Device &device,
                uint32_t queue_family_index,
                RenderFrame *render_frame = nullptr,
                size_t thread_index = 0,
                CommandBuffer::ResetMode reset_mode = CommandBuffer::ResetMode::RESET_POOL);

    CommandPool(const CommandPool &) = delete;

    CommandPool(CommandPool &&other) noexcept;

    ~CommandPool();

    CommandPool &operator=(const CommandPool &) = delete;

    CommandPool &operator=(CommandPool &&) = delete;

    Device &GetDevice();

    [[nodiscard]] uint32_t GetQueueFamilyIndex() const;

    [[nodiscard]] VkCommandPool GetHandle() const;

    RenderFrame *GetRenderFrame();

    [[nodiscard]] size_t GetThreadIndex() const;

    VkResult ResetPool();

    CommandBuffer &RequestCommandBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    [[nodiscard]] CommandBuffer::ResetMode GetResetMode() const;

private:
    Device &device_;

    VkCommandPool handle_{VK_NULL_HANDLE};

    RenderFrame *render_frame_{nullptr};

    size_t thread_index_{0};

    uint32_t queue_family_index_{0};

    std::vector<std::unique_ptr<CommandBuffer>> primary_command_buffers_;

    uint32_t active_primary_command_buffer_count_{0};

    std::vector<std::unique_ptr<CommandBuffer>> secondary_command_buffers_;

    uint32_t active_secondary_command_buffer_count_{0};

    CommandBuffer::ResetMode reset_mode_{CommandBuffer::ResetMode::RESET_POOL};

    VkResult ResetCommandBuffers();
};

}  // namespace vox
