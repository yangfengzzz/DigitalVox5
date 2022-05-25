//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_map>

#include "vox.base/helper.h"
#include "vox.render/vk_common.h"

namespace vox {
class Device;

class DescriptorSetLayout;

/**
 * @brief Manages an array of fixed size VkDescriptorPool and is able to allocate descriptor sets
 */
class DescriptorPool {
public:
    static const uint32_t max_sets_per_pool_ = 16;

    DescriptorPool(Device &device,
                   const DescriptorSetLayout &descriptor_set_layout,
                   uint32_t pool_size = max_sets_per_pool_);

    DescriptorPool(const DescriptorPool &) = delete;

    DescriptorPool(DescriptorPool &&) = default;

    ~DescriptorPool();

    DescriptorPool &operator=(const DescriptorPool &) = delete;

    DescriptorPool &operator=(DescriptorPool &&) = delete;

    void Reset();

    [[nodiscard]] const DescriptorSetLayout &GetDescriptorSetLayout() const;

    void SetDescriptorSetLayout(const DescriptorSetLayout &set_layout);

    VkDescriptorSet Allocate();

    VkResult Free(VkDescriptorSet descriptor_set);

private:
    Device &device_;

    const DescriptorSetLayout *descriptor_set_layout_{nullptr};

    // Descriptor pool size
    std::vector<VkDescriptorPoolSize> pool_sizes_;

    // Number of sets to allocate for each pool
    uint32_t pool_max_sets_{0};

    // Total descriptor pools created
    std::vector<VkDescriptorPool> pools_;

    // Count sets for each pool
    std::vector<uint32_t> pool_sets_count_;

    // Current pool index to allocate descriptor set
    uint32_t pool_index_{0};

    // Map between descriptor set and pool index
    std::unordered_map<VkDescriptorSet, uint32_t> set_pool_mapping_;

    // Find next pool index or create new pool
    uint32_t FindAvailablePool(uint32_t pool_index);
};

}  // namespace vox
