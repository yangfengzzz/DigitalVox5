//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/queue.h"

#include "vox.render/core/command_buffer.h"
#include "vox.render/core/device.h"

namespace vox {
Queue::Queue(
        Device &device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present, uint32_t index)
    : device_{device}, family_index_{family_index}, index_{index}, can_present_{can_present}, properties_{properties} {
    vkGetDeviceQueue(device.GetHandle(), family_index, index, &handle_);
}

Queue::Queue(Queue &&other) noexcept
    : device_{other.device_},
      handle_{other.handle_},
      family_index_{other.family_index_},
      index_{other.index_},
      can_present_{other.can_present_},
      properties_{other.properties_} {
    other.handle_ = VK_NULL_HANDLE;
    other.family_index_ = {};
    other.properties_ = {};
    other.can_present_ = VK_FALSE;
    other.index_ = 0;
}

const Device &Queue::GetDevice() const { return device_; }

VkQueue Queue::GetHandle() const { return handle_; }

uint32_t Queue::GetFamilyIndex() const { return family_index_; }

uint32_t Queue::GetIndex() const { return index_; }

const VkQueueFamilyProperties &Queue::GetProperties() const { return properties_; }

VkBool32 Queue::SupportPresent() const { return can_present_; }

VkResult Queue::Submit(const std::vector<VkSubmitInfo> &submit_infos, VkFence fence) const {
    return vkQueueSubmit(handle_, utility::ToU32(submit_infos.size()), submit_infos.data(), fence);
}

VkResult Queue::Submit(const CommandBuffer &command_buffer, VkFence fence) const {
    VkSubmitInfo submit_info{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer.GetHandle();

    return Submit({submit_info}, fence);
}

VkResult Queue::Present(const VkPresentInfoKHR &present_info) const {
    if (!can_present_) {
        return VK_ERROR_INCOMPATIBLE_DISPLAY_KHR;
    }

    return vkQueuePresentKHR(handle_, &present_info);
}  // namespace vox

VkResult Queue::WaitIdle() const { return vkQueueWaitIdle(handle_); }

}  // namespace vox
