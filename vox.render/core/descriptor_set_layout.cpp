//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/descriptor_set_layout.h"

#include <utility>

#include "vox.render/core/device.h"
#include "vox.render/core/physical_device.h"
#include "vox.render/shader/shader_module.h"

namespace vox {
namespace {
inline VkDescriptorType find_descriptor_type(ShaderResourceType resource_type, bool dynamic) {
    switch (resource_type) {
        case ShaderResourceType::INPUT_ATTACHMENT:
            return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        case ShaderResourceType::IMAGE:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case ShaderResourceType::IMAGE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case ShaderResourceType::IMAGE_STORAGE:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case ShaderResourceType::SAMPLER:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
        case ShaderResourceType::BUFFER_UNIFORM:
            if (dynamic) {
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            } else {
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            }
        case ShaderResourceType::BUFFER_STORAGE:
            if (dynamic) {
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            } else {
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            }
        default:
            throw std::runtime_error("No conversion possible for the shader resource type.");
    }
}

inline bool validate_binding(const VkDescriptorSetLayoutBinding &binding,
                             const std::vector<VkDescriptorType> &blacklist) {
    return !(std::find_if(blacklist.begin(), blacklist.end(), [binding](const VkDescriptorType &type) {
                 return type == binding.descriptorType;
             }) != blacklist.end());
}

inline bool validate_flags(const PhysicalDevice &gpu,
                           const std::vector<VkDescriptorSetLayoutBinding> &bindings,
                           const std::vector<VkDescriptorBindingFlagsEXT> &flags) {
    // Assume bindings are valid if there are no flags
    if (flags.empty()) {
        return true;
    }

    // Binding count has to equal flag count as it's a 1:1 mapping
    if (bindings.size() != flags.size()) {
        LOGE("Binding count has to be equal to flag count.")
        return false;
    }

    return true;
}
}  // namespace

DescriptorSetLayout::DescriptorSetLayout(Device &device,
                                         const uint32_t set_index,
                                         std::vector<ShaderModule *> shader_modules,
                                         const std::vector<ShaderResource> &resource_set)
    : device_{device}, set_index_{set_index}, shader_modules_{std::move(shader_modules)} {
    // NOTE: `shader_modules` is passed in mainly for hashing their handles in `request_resource`.
    //        This way, different pipelines (with different shaders / shader variants) will get
    //        different descriptor set layouts (incl. appropriate name -> binding lookups)

    for (auto &resource : resource_set) {
        // Skip shader resources without a binding point
        if (resource.type == ShaderResourceType::INPUT || resource.type == ShaderResourceType::OUTPUT ||
            resource.type == ShaderResourceType::PUSH_CONSTANT ||
            resource.type == ShaderResourceType::SPECIALIZATION_CONSTANT) {
            continue;
        }

        // Convert from ShaderResourceType to VkDescriptorType.
        auto descriptor_type = find_descriptor_type(resource.type, resource.mode == ShaderResourceMode::DYNAMIC);

        if (resource.mode == ShaderResourceMode::UPDATE_AFTER_BIND) {
            binding_flags_.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT);
        } else {
            // When creating a descriptor set layout, if we give a structure to create_info.pNext, each binding needs to
            // have a binding flag (pBindings[i] uses the flags in pBindingFlags[i]) Adding 0 ensures the bindings that
            // don't use any flags are mapped correctly.
            binding_flags_.push_back(0);
        }

        // Convert ShaderResource to VkDescriptorSetLayoutBinding
        VkDescriptorSetLayoutBinding layout_binding{};

        layout_binding.binding = resource.binding;
        layout_binding.descriptorCount = resource.array_size;
        layout_binding.descriptorType = descriptor_type;
        layout_binding.stageFlags = static_cast<VkShaderStageFlags>(resource.stages);

        bindings_.push_back(layout_binding);

        // Store mapping between binding and the binding point
        bindings_lookup_.emplace(resource.binding, layout_binding);

        binding_flags_lookup_.emplace(resource.binding, binding_flags_.back());

        resources_lookup_.emplace(resource.name, resource.binding);
    }

    VkDescriptorSetLayoutCreateInfo create_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    create_info.flags = 0;
    create_info.bindingCount = utility::ToU32(bindings_.size());
    create_info.pBindings = bindings_.data();

    // Handle update-after-bind extensions
    if (std::find_if(resource_set.begin(), resource_set.end(), [](const ShaderResource &shader_resource) {
            return shader_resource.mode == ShaderResourceMode::UPDATE_AFTER_BIND;
        }) != resource_set.end()) {
        // Spec states you can't have ANY dynamic resources if you have one of the bindings set to update-after-bind
        if (std::find_if(resource_set.begin(), resource_set.end(), [](const ShaderResource &shader_resource) {
                return shader_resource.mode == ShaderResourceMode::DYNAMIC;
            }) != resource_set.end()) {
            throw std::runtime_error(
                    "Cannot create descriptor set layout, dynamic resources are not allowed if at least one resource "
                    "is update-after-bind.");
        }

        if (!validate_flags(device.GetGpu(), bindings_, binding_flags_)) {
            throw std::runtime_error("Invalid binding, couldn't create descriptor set layout.");
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT binding_flags_create_info{
                VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT};
        binding_flags_create_info.bindingCount = utility::ToU32(binding_flags_.size());
        binding_flags_create_info.pBindingFlags = binding_flags_.data();

        create_info.pNext = &binding_flags_create_info;
        create_info.flags |= std::find(binding_flags_.begin(), binding_flags_.end(),
                                       VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT) != binding_flags_.end()
                                     ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT
                                     : 0;
    }

    // Create the Vulkan descriptor set layout handle_
    VkResult result = vkCreateDescriptorSetLayout(device.GetHandle(), &create_info, nullptr, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create DescriptorSetLayout"};
    }
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout &&other) noexcept
    : device_{other.device_},
      shader_modules_{other.shader_modules_},
      handle_{other.handle_},
      set_index_{other.set_index_},
      bindings_{std::move(other.bindings_)},
      binding_flags_{std::move(other.binding_flags_)},
      bindings_lookup_{std::move(other.bindings_lookup_)},
      binding_flags_lookup_{std::move(other.binding_flags_lookup_)},
      resources_lookup_{std::move(other.resources_lookup_)} {
    other.handle_ = VK_NULL_HANDLE;
}

DescriptorSetLayout::~DescriptorSetLayout() {
    // Destroy descriptor set layout
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device_.GetHandle(), handle_, nullptr);
    }
}

uint32_t DescriptorSetLayout::GetIndex() const { return set_index_; }

VkDescriptorSetLayout DescriptorSetLayout::GetHandle() const { return handle_; }

const std::vector<VkDescriptorSetLayoutBinding> &DescriptorSetLayout::GetBindings() const { return bindings_; }

const std::vector<VkDescriptorBindingFlagsEXT> &DescriptorSetLayout::GetBindingFlags() const { return binding_flags_; }

const std::vector<ShaderModule *> &DescriptorSetLayout::GetShaderModules() const { return shader_modules_; }

// MARK: -
std::unique_ptr<VkDescriptorSetLayoutBinding> DescriptorSetLayout::GetLayoutBinding(uint32_t binding_index) const {
    auto it = bindings_lookup_.find(binding_index);

    if (it == bindings_lookup_.end()) {
        return nullptr;
    }

    return std::make_unique<VkDescriptorSetLayoutBinding>(it->second);
}

std::unique_ptr<VkDescriptorSetLayoutBinding> DescriptorSetLayout::GetLayoutBinding(const std::string &name) const {
    auto it = resources_lookup_.find(name);

    if (it == resources_lookup_.end()) {
        return nullptr;
    }

    return GetLayoutBinding(it->second);
}

VkDescriptorBindingFlagsEXT DescriptorSetLayout::GetLayoutBindingFlag(uint32_t binding_index) const {
    auto it = binding_flags_lookup_.find(binding_index);

    if (it == binding_flags_lookup_.end()) {
        return 0;
    }

    return it->second;
}

}  // namespace vox
