//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/fence_pool.h"

#include "vox.render/core/device.h"

namespace vox {
FencePool::FencePool(Device &device) : device_{device} {}

FencePool::~FencePool() {
    wait();
    reset();

    // Destroy all fences
    for (VkFence fence : fences_) {
        vkDestroyFence(device_.GetHandle(), fence, nullptr);
    }

    fences_.clear();
}

VkFence FencePool::request_fence() {
    // Check if there is an available fence
    if (active_fence_count_ < fences_.size()) {
        return fences_.at(active_fence_count_++);
    }

    VkFence fence{VK_NULL_HANDLE};

    VkFenceCreateInfo create_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

    VkResult result = vkCreateFence(device_.GetHandle(), &create_info, nullptr, &fence);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create fence.");
    }

    fences_.push_back(fence);

    active_fence_count_++;

    return fences_.back();
}

VkResult FencePool::wait(uint32_t timeout) const {
    if (active_fence_count_ < 1 || fences_.empty()) {
        return VK_SUCCESS;
    }

    return vkWaitForFences(device_.GetHandle(), active_fence_count_, fences_.data(), true, timeout);
}

VkResult FencePool::reset() {
    if (active_fence_count_ < 1 || fences_.empty()) {
        return VK_SUCCESS;
    }

    VkResult result = vkResetFences(device_.GetHandle(), active_fence_count_, fences_.data());

    if (result != VK_SUCCESS) {
        return result;
    }

    active_fence_count_ = 0;

    return VK_SUCCESS;
}

}  // namespace vox
