//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/descriptor_set_layout.h"
#include "vox.render/shader/shader_module.h"
#include "vox.render/vk_common.h"

namespace vox {
class Device;

class ShaderModule;

class DescriptorSetLayout;

class PipelineLayout {
public:
    PipelineLayout(Device &device, const std::vector<ShaderModule *> &shader_modules);

    PipelineLayout(const PipelineLayout &) = delete;

    PipelineLayout(PipelineLayout &&other) noexcept;

    ~PipelineLayout();

    PipelineLayout &operator=(const PipelineLayout &) = delete;

    PipelineLayout &operator=(PipelineLayout &&) = delete;

    [[nodiscard]] VkPipelineLayout GetHandle() const;

    [[nodiscard]] const std::vector<ShaderModule *> &GetShaderModules() const;

    [[nodiscard]] std::vector<ShaderResource> GetResources(const ShaderResourceType &type = ShaderResourceType::ALL,
                                                           VkShaderStageFlagBits stage = VK_SHADER_STAGE_ALL) const;

    [[nodiscard]] const std::unordered_map<uint32_t, std::vector<ShaderResource>> &GetShaderSets() const;

    [[nodiscard]] bool HasDescriptorSetLayout(uint32_t set_index) const;

    [[nodiscard]] DescriptorSetLayout &GetDescriptorSetLayout(uint32_t set_index) const;

    [[nodiscard]] VkShaderStageFlags GetPushConstantRangeStage(uint32_t size, uint32_t offset = 0) const;

private:
    Device &device_;

    VkPipelineLayout handle_{VK_NULL_HANDLE};

    // The shader modules that this pipeline layout uses
    std::vector<ShaderModule *> shader_modules_;

    // The shader resources that this pipeline layout uses, indexed by their name
    std::unordered_map<std::string, ShaderResource> shader_resources_;

    // A map of each set and the resources it owns used by the pipeline layout
    std::unordered_map<uint32_t, std::vector<ShaderResource>> shader_sets_;

    // The different descriptor set layouts for this pipeline layout
    std::vector<DescriptorSetLayout *> descriptor_set_layouts_;
};

}  // namespace vox
