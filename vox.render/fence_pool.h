//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/error.h"

namespace vox {
class Device;

class FencePool {
public:
    explicit FencePool(Device &device);

    FencePool(const FencePool &) = delete;

    FencePool(FencePool &&other) = delete;

    ~FencePool();

    FencePool &operator=(const FencePool &) = delete;

    FencePool &operator=(FencePool &&) = delete;

    VkFence request_fence();

    VkResult wait(uint32_t timeout = std::numeric_limits<uint32_t>::max()) const;

    VkResult reset();

private:
    Device &device_;

    std::vector<VkFence> fences_;

    uint32_t active_fence_count_{0};
};

}  // namespace vox
