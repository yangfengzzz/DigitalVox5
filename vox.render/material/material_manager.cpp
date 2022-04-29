//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "material_manager.h"
#include "core/device.h"

namespace vox {
MaterialManager *MaterialManager::get_singleton_ptr() {
    return ms_singleton_;
}

MaterialManager &MaterialManager::get_singleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

MaterialManager::MaterialManager(Device& device):
device_(device) {
    // Create a default sampler
    last_sampler_create_info_ = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    last_sampler_create_info_.magFilter = VK_FILTER_LINEAR;
    last_sampler_create_info_.minFilter = VK_FILTER_LINEAR;
    last_sampler_create_info_.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    last_sampler_create_info_.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    last_sampler_create_info_.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    last_sampler_create_info_.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    last_sampler_create_info_.mipLodBias = 0.0f;
    last_sampler_create_info_.compareOp = VK_COMPARE_OP_NEVER;
    last_sampler_create_info_.minLod = 0.0f;
    // Max level-of-detail should match mip level count
    last_sampler_create_info_.maxLod = 0.0f;
    // Only enable anisotropic filtering if enabled on the device
    // Note that for simplicity, we will always be using max. available anisotropy level for the current device
    // This may have an impact on performance, esp. on lower-specced devices
    // In a real-world scenario the level of anisotropy should be a user setting or e.g. lowered for mobile devices by default
    last_sampler_create_info_.maxAnisotropy = device.get_gpu().get_features().samplerAnisotropy
    ? (device.get_gpu().get_properties().limits.maxSamplerAnisotropy)
    : 1.0f;
    last_sampler_create_info_.anisotropyEnable = device.get_gpu().get_features().samplerAnisotropy;
    last_sampler_create_info_.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
}

core::Sampler* MaterialManager::get_sampler(const VkSamplerCreateInfo& info) {
    auto iter = sampler_pool_.find(info);
    if (iter != sampler_pool_.end()) {
        return iter->second.get();
    } else {
        auto pair = std::make_pair(info, std::make_unique<core::Sampler>(device_, info));
        auto sampler = pair.second.get();
        sampler_pool_.insert(std::move(pair));
        return sampler;
    }
}

}

