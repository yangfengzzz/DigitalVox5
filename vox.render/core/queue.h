//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/swapchain.h"
#include "vox.render/vk_common.h"

namespace vox {
class Device;

class CommandBuffer;

class Queue {
public:
    Queue(Device &device,
          uint32_t family_index,
          VkQueueFamilyProperties properties,
          VkBool32 can_present,
          uint32_t index);

    Queue(const Queue &) = default;

    Queue(Queue &&other) noexcept;

    Queue &operator=(const Queue &) = delete;

    Queue &operator=(Queue &&) = delete;

    [[nodiscard]] const Device &GetDevice() const;

    [[nodiscard]] VkQueue GetHandle() const;

    [[nodiscard]] uint32_t GetFamilyIndex() const;

    [[nodiscard]] uint32_t GetIndex() const;

    [[nodiscard]] const VkQueueFamilyProperties &GetProperties() const;

    [[nodiscard]] VkBool32 SupportPresent() const;

    VkResult Submit(const std::vector<VkSubmitInfo> &submit_infos, VkFence fence) const;

    VkResult Submit(const CommandBuffer &command_buffer, VkFence fence) const;

    [[nodiscard]] VkResult Present(const VkPresentInfoKHR &present_infos) const;

    VkResult WaitIdle() const;

private:
    Device &device_;

    VkQueue handle_{VK_NULL_HANDLE};

    uint32_t family_index_{0};

    uint32_t index_{0};

    VkBool32 can_present_{VK_FALSE};

    VkQueueFamilyProperties properties_{};
};

}  // namespace vox
