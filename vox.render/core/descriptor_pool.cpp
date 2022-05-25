//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/descriptor_pool.h"

#include "vox.render/core/descriptor_set_layout.h"
#include "vox.render/core/device.h"
#include "vox.render/error.h"

namespace vox {
DescriptorPool::DescriptorPool(Device &device, const DescriptorSetLayout &descriptor_set_layout, uint32_t pool_size)
    : device_{device}, descriptor_set_layout_{&descriptor_set_layout} {
    const auto &bindings = descriptor_set_layout.GetBindings();

    std::map<VkDescriptorType, std::uint32_t> descriptor_type_counts;

    // Count each type of descriptor set
    for (auto &binding : bindings) {
        descriptor_type_counts[binding.descriptorType] += binding.descriptorCount;
    }

    // Allocate pool sizes array
    pool_sizes_.resize(descriptor_type_counts.size());

    auto pool_size_it = pool_sizes_.begin();

    // Fill pool size for each descriptor type count multiplied by the pool size
    for (auto &it : descriptor_type_counts) {
        pool_size_it->type = it.first;

        pool_size_it->descriptorCount = it.second * pool_size;

        ++pool_size_it;
    }

    pool_max_sets_ = pool_size;
}

DescriptorPool::~DescriptorPool() {
    // Destroy all descriptor pools
    for (auto pool : pools_) {
        vkDestroyDescriptorPool(device_.GetHandle(), pool, nullptr);
    }
}

void DescriptorPool::Reset() {
    // Reset all descriptor pools
    for (auto pool : pools_) {
        vkResetDescriptorPool(device_.GetHandle(), pool, 0);
    }

    // Clear internal tracking of descriptor set allocations
    std::fill(pool_sets_count_.begin(), pool_sets_count_.end(), 0);
    set_pool_mapping_.clear();

    // Reset the pool index from which descriptor sets are allocated
    pool_index_ = 0;
}

const DescriptorSetLayout &DescriptorPool::GetDescriptorSetLayout() const {
    assert(descriptor_set_layout_ && "Descriptor set layout is invalid");
    return *descriptor_set_layout_;
}

void DescriptorPool::SetDescriptorSetLayout(const DescriptorSetLayout &set_layout) {
    descriptor_set_layout_ = &set_layout;
}

VkDescriptorSet DescriptorPool::Allocate() {
    pool_index_ = FindAvailablePool(pool_index_);

    // Increment allocated set count for the current pool
    ++pool_sets_count_[pool_index_];

    VkDescriptorSetLayout set_layout = GetDescriptorSetLayout().GetHandle();

    VkDescriptorSetAllocateInfo alloc_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    alloc_info.descriptorPool = pools_[pool_index_];
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &set_layout;

    VkDescriptorSet handle = VK_NULL_HANDLE;

    // Allocate a new descriptor set from the current pool
    auto result = vkAllocateDescriptorSets(device_.GetHandle(), &alloc_info, &handle);

    if (result != VK_SUCCESS) {
        // Decrement allocated set count for the current pool
        --pool_sets_count_[pool_index_];

        return VK_NULL_HANDLE;
    }

    // Store mapping between the descriptor set and the pool
    set_pool_mapping_.emplace(handle, pool_index_);

    return handle;
}

VkResult DescriptorPool::Free(VkDescriptorSet descriptor_set) {
    // Get the pool index of the descriptor set
    auto it = set_pool_mapping_.find(descriptor_set);

    if (it == set_pool_mapping_.end()) {
        return VK_INCOMPLETE;
    }

    auto desc_pool_index = it->second;

    // Free descriptor set from the pool
    vkFreeDescriptorSets(device_.GetHandle(), pools_[desc_pool_index], 1, &descriptor_set);

    // Remove descriptor set mapping to the pool
    set_pool_mapping_.erase(it);

    // Decrement allocated set count for the pool
    --pool_sets_count_[desc_pool_index];

    // Change the current pool index to use the available pool
    pool_index_ = desc_pool_index;

    return VK_SUCCESS;
}

std::uint32_t DescriptorPool::FindAvailablePool(std::uint32_t pool_index) {
    // Create a new pool
    if (pools_.size() <= pool_index) {
        VkDescriptorPoolCreateInfo create_info{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};

        create_info.poolSizeCount = utility::ToU32(pool_sizes_.size());
        create_info.pPoolSizes = pool_sizes_.data();
        create_info.maxSets = pool_max_sets_;

        // We do not set FREE_DESCRIPTOR_SET_BIT as we do not need to free individual descriptor sets
        create_info.flags = 0;

        // Check descriptor set layout and enable the required flags
        auto &binding_flags = descriptor_set_layout_->GetBindingFlags();
        for (auto binding_flag : binding_flags) {
            if (binding_flag & VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT) {
                create_info.flags |= VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
            }
        }

        VkDescriptorPool handle = VK_NULL_HANDLE;

        // Create the Vulkan descriptor pool
        auto result = vkCreateDescriptorPool(device_.GetHandle(), &create_info, nullptr, &handle);

        if (result != VK_SUCCESS) {
            return 0;
        }

        // Store internally the Vulkan handle_
        pools_.push_back(handle);

        // Add set count for the descriptor pool
        pool_sets_count_.push_back(0);

        return pool_index;
    } else if (pool_sets_count_[pool_index] < pool_max_sets_) {
        return pool_index;
    }

    // Increment pool index
    return FindAvailablePool(++pool_index);
}

}  // namespace vox
