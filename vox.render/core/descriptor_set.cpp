//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "descriptor_set.h"

#include "logging.h"
#include "resource_caching.h"
#include "descriptor_pool.h"
#include "descriptor_set_layout.h"
#include "device.h"

namespace vox {
DescriptorSet::DescriptorSet(Device &device,
                             DescriptorSetLayout &descriptor_set_layout,
                             DescriptorPool &descriptor_pool,
                             const BindingMap<VkDescriptorBufferInfo> &buffer_infos,
                             const BindingMap<VkDescriptorImageInfo> &image_infos) :
device_{device},
descriptor_set_layout_{descriptor_set_layout},
descriptor_pool_{descriptor_pool},
buffer_infos_{buffer_infos},
image_infos_{image_infos},
handle_{descriptor_pool.allocate()} {
    prepare();
}

void DescriptorSet::reset(const BindingMap<VkDescriptorBufferInfo> &new_buffer_infos,
                          const BindingMap<VkDescriptorImageInfo> &new_image_infos) {
    if (!new_buffer_infos.empty() || !new_image_infos.empty()) {
        buffer_infos_ = new_buffer_infos;
        image_infos_ = new_image_infos;
    } else {
        LOGW("Calling reset on Descriptor Set with no new buffer infos and no new image infos.")
    }
    
    write_descriptor_sets_.clear();
    updated_bindings_.clear();
    
    prepare();
}

void DescriptorSet::prepare() {
    // We don't want to prepare twice during the life cycle of a Descriptor Set
    if (!write_descriptor_sets_.empty()) {
        LOGW("Trying to prepare a descriptor set that has already been prepared, skipping.")
        return;
    }
    
    // Iterate over all buffer bindings
    for (auto &binding_it : buffer_infos_) {
        auto binding_index = binding_it.first;
        auto &buffer_bindings = binding_it.second;
        
        if (auto binding_info = descriptor_set_layout_.get_layout_binding(binding_index)) {
            // Iterate over all binding buffers in array
            for (auto &element_it : buffer_bindings) {
                auto &buffer_info = element_it.second;
                
                size_t uniform_buffer_range_limit = device_.get_gpu().get_properties().limits.maxUniformBufferRange;
                size_t storage_buffer_range_limit = device_.get_gpu().get_properties().limits.maxStorageBufferRange;
                
                auto buffer_range_limit = static_cast<size_t>(buffer_info.range);
                
                if ((binding_info->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
                     binding_info->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) &&
                    buffer_range_limit > uniform_buffer_range_limit) {
                    LOGE("Set {} binding {} cannot be updated: buffer size {} exceeds the uniform buffer range limit {}",
                         descriptor_set_layout_.get_index(), binding_index, buffer_info.range,
                         uniform_buffer_range_limit)
                    buffer_range_limit = uniform_buffer_range_limit;
                } else if ((binding_info->descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
                            binding_info->descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) &&
                           buffer_range_limit > storage_buffer_range_limit) {
                    LOGE("Set {} binding {} cannot be updated: buffer size {} exceeds the storage buffer range limit {}",
                         descriptor_set_layout_.get_index(), binding_index, buffer_info.range,
                         storage_buffer_range_limit)
                    buffer_range_limit = storage_buffer_range_limit;
                }
                
                // Clip the buffers range to the limit if one exists as otherwise we will receive a Vulkan validation error
                buffer_info.range = buffer_range_limit;
                
                VkWriteDescriptorSet write_descriptor_set{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
                
                write_descriptor_set.dstBinding = binding_index;
                write_descriptor_set.descriptorType = binding_info->descriptorType;
                write_descriptor_set.pBufferInfo = &buffer_info;
                write_descriptor_set.dstSet = handle_;
                write_descriptor_set.dstArrayElement = element_it.first;
                write_descriptor_set.descriptorCount = 1;
                
                write_descriptor_sets_.push_back(write_descriptor_set);
            }
        } else {
            LOGE("Shader layout set does not use buffer binding at #{}", binding_index)
        }
    }
    
    // Iterate over all image bindings
    for (auto &binding_it : image_infos_) {
        auto binding_index = binding_it.first;
        auto &binding_resources = binding_it.second;
        
        if (auto binding_info = descriptor_set_layout_.get_layout_binding(binding_index)) {
            // Iterate over all binding images in array
            for (auto &element_it : binding_resources) {
                auto &image_info = element_it.second;
                
                VkWriteDescriptorSet write_descriptor_set{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
                
                write_descriptor_set.dstBinding = binding_index;
                write_descriptor_set.descriptorType = binding_info->descriptorType;
                write_descriptor_set.pImageInfo = &image_info;
                write_descriptor_set.dstSet = handle_;
                write_descriptor_set.dstArrayElement = element_it.first;
                write_descriptor_set.descriptorCount = 1;
                
                write_descriptor_sets_.push_back(write_descriptor_set);
            }
        } else {
            LOGE("Shader layout set does not use image binding at #{}", binding_index)
        }
    }
}

void DescriptorSet::update(const std::vector<uint32_t> &bindings_to_update) {
    std::vector<VkWriteDescriptorSet> write_operations;
    std::vector<size_t> write_operation_hashes;
    
    // If the 'bindings_to_update' vector is empty, we want to write to all the bindings
    // (but skipping all to-update bindings that haven't been written yet)
    if (bindings_to_update.empty()) {
        for (auto &write_operation : write_descriptor_sets_) {
            size_t write_operation_hash = 0;
            hash_param(write_operation_hash, write_operation);
            
            auto update_pair_it = updated_bindings_.find(write_operation.dstBinding);
            if (update_pair_it == updated_bindings_.end() || update_pair_it->second != write_operation_hash) {
                write_operations.push_back(write_operation);
                write_operation_hashes.push_back(write_operation_hash);
            }
        }
    } else {
        // Otherwise, we want to update the binding indices present in the 'bindings_to_update' vector.
        // (again, skipping those to update but not updated yet)
        for (auto &write_operation : write_descriptor_sets_) {
            if (std::find(bindings_to_update.begin(), bindings_to_update.end(), write_operation.dstBinding) !=
                bindings_to_update.end()) {
                size_t write_operation_hash = 0;
                hash_param(write_operation_hash, write_operation);
                
                auto update_pair_it = updated_bindings_.find(write_operation.dstBinding);
                if (update_pair_it == updated_bindings_.end() || update_pair_it->second != write_operation_hash) {
                    write_operations.push_back(write_operation);
                    write_operation_hashes.push_back(write_operation_hash);
                }
            }
        }
    }
    
    // Perform the Vulkan call to update the DescriptorSet by executing the write operations
    if (!write_operations.empty()) {
        vkUpdateDescriptorSets(device_.get_handle(),
                               to_u32(write_operations.size()),
                               write_operations.data(),
                               0,
                               nullptr);
    }
    
    // Store the bindings from the write operations that were executed by vkUpdateDescriptorSets (and their hash)
    // to prevent overwriting by future calls to "update()"
    for (size_t i = 0; i < write_operations.size(); i++) {
        updated_bindings_[write_operations[i].dstBinding] = write_operation_hashes[i];
    }
}

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept:
device_{other.device_},
descriptor_set_layout_{other.descriptor_set_layout_},
descriptor_pool_{other.descriptor_pool_},
buffer_infos_{std::move(other.buffer_infos_)},
image_infos_{std::move(other.image_infos_)},
handle_{other.handle_},
write_descriptor_sets_{std::move(other.write_descriptor_sets_)},
updated_bindings_{std::move(other.updated_bindings_)} {
    other.handle_ = VK_NULL_HANDLE;
}

VkDescriptorSet DescriptorSet::get_handle() const {
    return handle_;
}

const DescriptorSetLayout &DescriptorSet::get_layout() const {
    return descriptor_set_layout_;
}

BindingMap<VkDescriptorBufferInfo> &DescriptorSet::get_buffer_infos() {
    return buffer_infos_;
}

BindingMap<VkDescriptorImageInfo> &DescriptorSet::get_image_infos() {
    return image_infos_;
}

}        // namespace vox
