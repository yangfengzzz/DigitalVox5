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

namespace core {
/**
 * @brief A simplified buffer class for creating temporary device local scratch buffers, used in e.g. ray tracing
 */
class ScratchBuffer {
public:
    /**
     * @brief Creates a scratch buffer using VMA with pre-defined usage flags
     * @param device A valid Vulkan device
     * @param size The size in bytes of the buffer
     */
    ScratchBuffer(Device &device, VkDeviceSize size);

    ~ScratchBuffer();

    [[nodiscard]] VkBuffer GetHandle() const;

    [[nodiscard]] uint64_t GetDeviceAddress() const;

    /**
     * @return The size of the buffer
     */
    [[nodiscard]] VkDeviceSize GetSize() const { return size_; }

private:
    Device &device_;

    uint64_t device_address_{0};

    VkBuffer handle_{VK_NULL_HANDLE};

    VmaAllocation allocation_{VK_NULL_HANDLE};

    VkDeviceMemory memory_{VK_NULL_HANDLE};

    VkDeviceSize size_{0};
};

}  // namespace core
}  // namespace vox
