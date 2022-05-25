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
 * @brief Extended buffer class to simplify ray tracing shader binding table usage
 */
class ShaderBindingTable {
public:
    /**
     * @brief Creates a shader binding table
     * @param device A valid Vulkan device
     * @param handle_count Shader group handle count
     * @param handle_size_aligned Aligned shader group handle size
     * @param memory_usage The memory usage of the shader binding table
     */
    ShaderBindingTable(Device &device,
                       uint32_t handle_count,
                       VkDeviceSize handle_size_aligned,
                       VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU);

    ~ShaderBindingTable();

    [[nodiscard]] const VkStridedDeviceAddressRegionKHR *GetStridedDeviceAddressRegion() const;

    [[nodiscard]] uint8_t *GetData() const;

private:
    Device &device_;

    VkStridedDeviceAddressRegionKHR strided_device_address_region_{};

    uint64_t device_address_{0};

    VkBuffer handle_{VK_NULL_HANDLE};

    VmaAllocation allocation_{VK_NULL_HANDLE};

    VkDeviceMemory memory_{VK_NULL_HANDLE};

    uint8_t *mapped_data_{nullptr};
};

}  // namespace core
}  // namespace vox
