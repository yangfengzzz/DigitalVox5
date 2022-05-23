//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/sampler.h"

#include "vox.render/core/device.h"

namespace vox::core {
Sampler::Sampler(Device const &d, const VkSamplerCreateInfo &info) : VulkanResource{VK_NULL_HANDLE, &d} {
    VK_CHECK(vkCreateSampler(device_->GetHandle(), &info, nullptr, &handle_));
}

Sampler::Sampler(Sampler &&other) noexcept : VulkanResource{std::move(other)} {}

Sampler::~Sampler() {
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroySampler(device_->GetHandle(), handle_, nullptr);
    }
}

}  // namespace vox::core
