//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/vk_common.h"

namespace vox {
class DescriptorPool;

class Device;

class ShaderModule;

struct ShaderResource;

/**
 * @brief Caches DescriptorSet objects for the shader's set index.
 *        Creates a DescriptorPool to allocate the DescriptorSet objects
 */
class DescriptorSetLayout {
public:
    /**
     * @brief Creates a descriptor set layout from a set of resources
     * @param device A valid Vulkan device
     * @param set_index The descriptor set index this layout maps to
     * @param shader_modules The shader modules this set layout will be used for
     * @param resource_set A grouping of shader resources belonging to the same set
     */
    DescriptorSetLayout(Device &device,
                        uint32_t set_index,
                        std::vector<ShaderModule *> shader_modules,
                        const std::vector<ShaderResource> &resource_set);

    DescriptorSetLayout(const DescriptorSetLayout &) = delete;

    DescriptorSetLayout(DescriptorSetLayout &&other) noexcept;

    ~DescriptorSetLayout();

    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    DescriptorSetLayout &operator=(DescriptorSetLayout &&) = delete;

    [[nodiscard]] uint32_t GetIndex() const;

    [[nodiscard]] VkDescriptorSetLayout GetHandle() const;

    [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding> &GetBindings() const;

    [[nodiscard]] const std::vector<VkDescriptorBindingFlagsEXT> &GetBindingFlags() const;

    [[nodiscard]] const std::vector<ShaderModule *> &GetShaderModules() const;

public:
    [[nodiscard]] std::unique_ptr<VkDescriptorSetLayoutBinding> GetLayoutBinding(uint32_t binding_index) const;

    [[nodiscard]] std::unique_ptr<VkDescriptorSetLayoutBinding> GetLayoutBinding(const std::string &name) const;

    [[nodiscard]] VkDescriptorBindingFlagsEXT GetLayoutBindingFlag(uint32_t binding_index) const;

private:
    Device &device_;

    const uint32_t set_index_;

    VkDescriptorSetLayout handle_{VK_NULL_HANDLE};

    std::vector<VkDescriptorSetLayoutBinding> bindings_;

    std::vector<VkDescriptorBindingFlagsEXT> binding_flags_;

    std::vector<ShaderModule *> shader_modules_;

    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_lookup_;

    std::unordered_map<uint32_t, VkDescriptorBindingFlagsEXT> binding_flags_lookup_;

    std::unordered_map<std::string, uint32_t> resources_lookup_;
};

}  // namespace vox
