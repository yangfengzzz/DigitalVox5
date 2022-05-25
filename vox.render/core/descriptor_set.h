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

class DescriptorSetLayout;

class DescriptorPool;

/**
 * @brief A descriptor set handle allocated from a \ref DescriptorPool.
 *        Destroying the handle has no effect, as the pool manages the lifecycle of its descriptor sets.
 *
 *        Keeps track of what bindings were written to prevent a double write.
 */
class DescriptorSet {
public:
    /**
     * @brief Constructs a descriptor set from buffer infos and image infos
     *        Implicitly calls prepare()
     * @param device A valid Vulkan device
     * @param descriptor_set_layout The Vulkan descriptor set layout this descriptor set has
     * @param descriptor_pool The Vulkan descriptor pool the descriptor set is allocated from
     * @param buffer_infos The descriptors that describe buffer data
     * @param image_infos The descriptors that describe image data
     */
    DescriptorSet(Device &device,
                  DescriptorSetLayout &descriptor_set_layout,
                  DescriptorPool &descriptor_pool,
                  const BindingMap<VkDescriptorBufferInfo> &buffer_infos = {},
                  const BindingMap<VkDescriptorImageInfo> &image_infos = {});

    DescriptorSet(const DescriptorSet &) = delete;

    DescriptorSet(DescriptorSet &&other) noexcept;

    // The descriptor set handle is managed by the pool, and will be destroyed when the pool is reset
    ~DescriptorSet() = default;

    DescriptorSet &operator=(const DescriptorSet &) = delete;

    DescriptorSet &operator=(DescriptorSet &&) = delete;

    /**
     * @brief Resets the DescriptorSet state
     *        Optionally prepares a new set of buffer infos and/or image infos
     * @param new_buffer_infos A map of buffer descriptors and their respective bindings
     * @param new_image_infos A map of image descriptors and their respective bindings
     */
    void Reset(const BindingMap<VkDescriptorBufferInfo> &new_buffer_infos = {},
               const BindingMap<VkDescriptorImageInfo> &new_image_infos = {});

    /**
     * @brief Updates the contents of the DescriptorSet by performing the write operations
     * @param bindings_to_update If empty. we update all bindings. Otherwise, only write the specified bindings if they
     * haven't already been written
     */
    void Update(const std::vector<uint32_t> &bindings_to_update = {});

    [[nodiscard]] const DescriptorSetLayout &GetLayout() const;

    [[nodiscard]] VkDescriptorSet GetHandle() const;

    BindingMap<VkDescriptorBufferInfo> &GetBufferInfos();

    BindingMap<VkDescriptorImageInfo> &GetImageInfos();

protected:
    /**
     * @brief Prepares the descriptor set to have its contents updated by loading a vector of write operations
     *        Cannot be called twice during the lifetime of a DescriptorSet
     */
    void Prepare();

private:
    Device &device_;

    DescriptorSetLayout &descriptor_set_layout_;

    DescriptorPool &descriptor_pool_;

    BindingMap<VkDescriptorBufferInfo> buffer_infos_;

    BindingMap<VkDescriptorImageInfo> image_infos_;

    VkDescriptorSet handle_{VK_NULL_HANDLE};

    // The list of write operations for the descriptor set
    std::vector<VkWriteDescriptorSet> write_descriptor_sets_;

    // The bindings of write descriptors that have had vkUpdateDescriptorSets since the last call to update().
    // Each binding number is mapped to a hash of the binding description that it will be updated to.
    std::unordered_map<uint32_t, size_t> updated_bindings_;
};

}  // namespace vox
