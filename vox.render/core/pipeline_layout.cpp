//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/pipeline_layout.h"

#include "vox.render/core/descriptor_set_layout.h"
#include "vox.render/core/device.h"
#include "vox.render/shader/shader_module.h"

namespace vox {
PipelineLayout::PipelineLayout(Device &device, const std::vector<ShaderModule *> &shader_modules)
    : device_{device}, shader_modules_{shader_modules} {
    // Collect and combine all the shader resources from each of the shader modules
    // Collate them all into a map that is indexed by the name of the resource
    for (auto *shader_module : shader_modules) {
        for (const auto &shader_resource : shader_module->GetResources()) {
            std::string key = shader_resource.name;

            // Since 'Input' and 'Output' resources can have the same name, we modify the key string
            if (shader_resource.type == ShaderResourceType::INPUT ||
                shader_resource.type == ShaderResourceType::OUTPUT) {
                key = std::to_string(shader_resource.stages) + "_" + key;
            }

            auto it = shader_resources_.find(key);

            if (it != shader_resources_.end()) {
                // Append stage flags if resource already exists
                it->second.stages |= shader_resource.stages;
            } else {
                // Create a new entry in the map
                shader_resources_.emplace(key, shader_resource);
            }
        }
    }

    // Sift through the map of name indexed shader resources
    // Separate them into their respective sets
    for (auto &it : shader_resources_) {
        auto &shader_resource = it.second;

        // Find binding by set index in the map.
        auto it_2 = shader_sets_.find(shader_resource.set);

        if (it_2 != shader_sets_.end()) {
            // Add resource to the found set index
            it_2->second.push_back(shader_resource);
        } else {
            // Create a new set index and with the first resource
            shader_sets_.emplace(shader_resource.set, std::vector<ShaderResource>{shader_resource});
        }
    }

    // Create a descriptor set layout for each shader set in the shader modules
    for (auto &shader_set_it : shader_sets_) {
        descriptor_set_layouts_.emplace_back(&device.GetResourceCache().RequestDescriptorSetLayout(
                shader_set_it.first, shader_modules, shader_set_it.second));
    }

    // Collect all the descriptor set layout handles, maintaining set order
    std::vector<VkDescriptorSetLayout> descriptor_set_layout_handles;
    for (auto &descriptor_set_layout : descriptor_set_layouts_) {
        if (descriptor_set_layout) {
            descriptor_set_layout_handles.push_back(descriptor_set_layout->GetHandle());
        } else {
            descriptor_set_layout_handles.push_back(VK_NULL_HANDLE);
        }
    }

    // Collect all the push constant shader resources
    std::vector<VkPushConstantRange> push_constant_ranges;
    for (auto &push_constant_resource : GetResources(ShaderResourceType::PUSH_CONSTANT)) {
        push_constant_ranges.push_back(
                {push_constant_resource.stages, push_constant_resource.offset, push_constant_resource.size});
    }

    VkPipelineLayoutCreateInfo create_info{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    create_info.setLayoutCount = utility::ToU32(descriptor_set_layout_handles.size());
    create_info.pSetLayouts = descriptor_set_layout_handles.data();
    create_info.pushConstantRangeCount = utility::ToU32(push_constant_ranges.size());
    create_info.pPushConstantRanges = push_constant_ranges.data();

    // Create the Vulkan pipeline layout handle_
    auto result = vkCreatePipelineLayout(device.GetHandle(), &create_info, nullptr, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create PipelineLayout"};
    }
}

PipelineLayout::PipelineLayout(PipelineLayout &&other) noexcept
    : device_{other.device_},
      handle_{other.handle_},
      shader_modules_{std::move(other.shader_modules_)},
      shader_resources_{std::move(other.shader_resources_)},
      shader_sets_{std::move(other.shader_sets_)},
      descriptor_set_layouts_{std::move(other.descriptor_set_layouts_)} {
    other.handle_ = VK_NULL_HANDLE;
}

PipelineLayout::~PipelineLayout() {
    // Destroy pipeline layout
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device_.GetHandle(), handle_, nullptr);
    }
}

VkPipelineLayout PipelineLayout::GetHandle() const { return handle_; }

const std::vector<ShaderModule *> &PipelineLayout::GetShaderModules() const { return shader_modules_; }

std::vector<ShaderResource> PipelineLayout::GetResources(const ShaderResourceType &type,
                                                         VkShaderStageFlagBits stage) const {
    std::vector<ShaderResource> found_resources;

    for (auto &it : shader_resources_) {
        auto &shader_resource = it.second;

        if (shader_resource.type == type || type == ShaderResourceType::ALL) {
            if (shader_resource.stages == stage || stage == VK_SHADER_STAGE_ALL) {
                found_resources.push_back(shader_resource);
            }
        }
    }

    return found_resources;
}

const std::unordered_map<uint32_t, std::vector<ShaderResource>> &PipelineLayout::GetShaderSets() const {
    return shader_sets_;
}

bool PipelineLayout::HasDescriptorSetLayout(uint32_t set_index) const {
    return set_index < descriptor_set_layouts_.size();
}

DescriptorSetLayout &PipelineLayout::GetDescriptorSetLayout(uint32_t set_index) const {
    for (auto &descriptor_set_layout : descriptor_set_layouts_) {
        if (descriptor_set_layout->GetIndex() == set_index) {
            return *descriptor_set_layout;
        }
    }
    throw std::runtime_error("Couldn't find descriptor set layout at set index " + vox::ToString(set_index));
}

VkShaderStageFlags PipelineLayout::GetPushConstantRangeStage(uint32_t size, uint32_t offset) const {
    VkShaderStageFlags stages = 0;

    for (auto &push_constant_resource : GetResources(ShaderResourceType::PUSH_CONSTANT)) {
        if (offset >= push_constant_resource.offset &&
            offset + size <= push_constant_resource.offset + push_constant_resource.size) {
            stages |= push_constant_resource.stages;
        }
    }
    return stages;
}

}  // namespace vox
