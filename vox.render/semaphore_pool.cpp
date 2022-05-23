//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/semaphore_pool.h"

#include "vox.render/core/device.h"

namespace vox {
SemaphorePool::SemaphorePool(Device &device) : device_{device} {}

SemaphorePool::~SemaphorePool() {
    Reset();

    // Destroy all semaphores
    for (VkSemaphore semaphore : semaphores_) {
        vkDestroySemaphore(device_.GetHandle(), semaphore, nullptr);
    }

    semaphores_.clear();
}

VkSemaphore SemaphorePool::RequestSemaphoreWithOwnership() {
    // Check if there is an available semaphore, if so, just pilfer one.
    if (active_semaphore_count_ < semaphores_.size()) {
        VkSemaphore semaphore = semaphores_.back();
        semaphores_.pop_back();
        return semaphore;
    }

    // Otherwise, we need to create one, and don't keep track of it, app will release.
    VkSemaphore semaphore{VK_NULL_HANDLE};

    VkSemaphoreCreateInfo create_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkResult result = vkCreateSemaphore(device_.GetHandle(), &create_info, nullptr, &semaphore);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore.");
    }

    return semaphore;
}

void SemaphorePool::ReleaseOwnedSemaphore(VkSemaphore semaphore) {
    // We cannot reuse this semaphore until ::reset().
    released_semaphores_.push_back(semaphore);
}

VkSemaphore SemaphorePool::RequestSemaphore() {
    // Check if there is an available semaphore
    if (active_semaphore_count_ < semaphores_.size()) {
        return semaphores_.at(active_semaphore_count_++);
    }

    VkSemaphore semaphore{VK_NULL_HANDLE};

    VkSemaphoreCreateInfo create_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkResult result = vkCreateSemaphore(device_.GetHandle(), &create_info, nullptr, &semaphore);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore.");
    }

    semaphores_.push_back(semaphore);

    active_semaphore_count_++;

    return semaphore;
}

void SemaphorePool::Reset() {
    active_semaphore_count_ = 0;

    // Now we can safely recycle the released semaphores.
    for (auto &sem : released_semaphores_) {
        semaphores_.push_back(sem);
    }

    released_semaphores_.clear();
}

uint32_t SemaphorePool::GetActiveSemaphoreCount() const { return active_semaphore_count_; }

}  // namespace vox
