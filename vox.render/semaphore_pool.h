//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/vk_common.h"

namespace vox {
class Device;

class SemaphorePool {
public:
    explicit SemaphorePool(Device &device);

    SemaphorePool(const SemaphorePool &) = delete;

    SemaphorePool(SemaphorePool &&other) = delete;

    ~SemaphorePool();

    SemaphorePool &operator=(const SemaphorePool &) = delete;

    SemaphorePool &operator=(SemaphorePool &&) = delete;

    VkSemaphore RequestSemaphore();

    VkSemaphore RequestSemaphoreWithOwnership();

    void ReleaseOwnedSemaphore(VkSemaphore semaphore);

    void Reset();

    [[nodiscard]] uint32_t GetActiveSemaphoreCount() const;

private:
    Device &device_;

    std::vector<VkSemaphore> semaphores_;
    std::vector<VkSemaphore> released_semaphores_;

    uint32_t active_semaphore_count_{0};
};

}  // namespace vox
