/* Copyright (c) 2019, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fence_pool.h"

#include "core/device.h"

namespace vox {
FencePool::FencePool(Device &device) :
device_{device} {
}

FencePool::~FencePool() {
    wait();
    reset();
    
    // Destroy all fences
    for (VkFence fence : fences_) {
        vkDestroyFence(device_.get_handle(), fence, nullptr);
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
    
    VkResult result = vkCreateFence(device_.get_handle(), &create_info, nullptr, &fence);
    
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
    
    return vkWaitForFences(device_.get_handle(), active_fence_count_, fences_.data(), true, timeout);
}

VkResult FencePool::reset() {
    if (active_fence_count_ < 1 || fences_.empty()) {
        return VK_SUCCESS;
    }
    
    VkResult result = vkResetFences(device_.get_handle(), active_fence_count_, fences_.data());
    
    if (result != VK_SUCCESS) {
        return result;
    }
    
    active_fence_count_ = 0;
    
    return VK_SUCCESS;
}

}        // namespace vox
