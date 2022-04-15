//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "helpers.h"
#include "vk_common.h"
#include "core/swapchain.h"

namespace vox {
class Device;

class CommandBuffer;

class Queue {
public:
    Queue(Device &device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present,
          uint32_t index);
    
    Queue(const Queue &) = default;
    
    Queue(Queue &&other) noexcept;
    
    Queue &operator=(const Queue &) = delete;
    
    Queue &operator=(Queue &&) = delete;
    
    [[nodiscard]] const Device &get_device() const;
    
    [[nodiscard]] VkQueue get_handle() const;
    
    [[nodiscard]] uint32_t get_family_index() const;
    
    [[nodiscard]] uint32_t get_index() const;
    
    [[nodiscard]] const VkQueueFamilyProperties &get_properties() const;
    
    [[nodiscard]] VkBool32 support_present() const;
    
    VkResult submit(const std::vector<VkSubmitInfo> &submit_infos, VkFence fence) const;
    
    VkResult submit(const CommandBuffer &command_buffer, VkFence fence) const;
    
    [[nodiscard]] VkResult present(const VkPresentInfoKHR &present_infos) const;
    
    [[nodiscard]] VkResult wait_idle() const;
    
private:
    Device &device_;
    
    VkQueue handle_{VK_NULL_HANDLE};
    
    uint32_t family_index_{0};
    
    uint32_t index_{0};
    
    VkBool32 can_present_{VK_FALSE};
    
    VkQueueFamilyProperties properties_{};
};

}        // namespace vox
