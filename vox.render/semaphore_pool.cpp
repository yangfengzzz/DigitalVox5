/* Copyright (c) 2019-2021, Arm Limited and Contributors
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

#include "semaphore_pool.h"

#include "core/device.h"

namespace vox {
SemaphorePool::SemaphorePool(Device &device) :
device_{device} {
}

SemaphorePool::~SemaphorePool() {
    reset();
    
    // Destroy all semaphores
    for (VkSemaphore semaphore : semaphores_) {
        vkDestroySemaphore(device_.get_handle(), semaphore, nullptr);
    }
    
    semaphores_.clear();
}

VkSemaphore SemaphorePool::request_semaphore_with_ownership() {
    // Check if there is an available semaphore, if so, just pilfer one.
    if (active_semaphore_count_ < semaphores_.size()) {
        VkSemaphore semaphore = semaphores_.back();
        semaphores_.pop_back();
        return semaphore;
    }
    
    // Otherwise, we need to create one, and don't keep track of it, app will release.
    VkSemaphore semaphore{VK_NULL_HANDLE};
    
    VkSemaphoreCreateInfo create_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    
    VkResult result = vkCreateSemaphore(device_.get_handle(), &create_info, nullptr, &semaphore);
    
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore.");
    }
    
    return semaphore;
}

void SemaphorePool::release_owned_semaphore(VkSemaphore semaphore) {
    // We cannot reuse this semaphore until ::reset().
    released_semaphores_.push_back(semaphore);
}

VkSemaphore SemaphorePool::request_semaphore() {
    // Check if there is an available semaphore
    if (active_semaphore_count_ < semaphores_.size()) {
        return semaphores_.at(active_semaphore_count_++);
    }
    
    VkSemaphore semaphore{VK_NULL_HANDLE};
    
    VkSemaphoreCreateInfo create_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    
    VkResult result = vkCreateSemaphore(device_.get_handle(), &create_info, nullptr, &semaphore);
    
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore.");
    }
    
    semaphores_.push_back(semaphore);
    
    active_semaphore_count_++;
    
    return semaphore;
}

void SemaphorePool::reset() {
    active_semaphore_count_ = 0;
    
    // Now we can safely recycle the released semaphores.
    for (auto &sem : released_semaphores_) {
        semaphores_.push_back(sem);
    }
    
    released_semaphores_.clear();
}

uint32_t SemaphorePool::get_active_semaphore_count() const {
    return active_semaphore_count_;
}

}        // namespace vox
