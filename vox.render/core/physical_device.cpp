//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/physical_device.h"

#include "vox.render/error.h"

namespace vox {
PhysicalDevice::PhysicalDevice(Instance &instance, VkPhysicalDevice physical_device)
    : instance_{instance}, handle_{physical_device} {
    vkGetPhysicalDeviceFeatures(physical_device, &features_);
    vkGetPhysicalDeviceProperties(physical_device, &properties_);
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties_);

    LOGI("Found GPU: {}", properties_.deviceName)

    uint32_t queue_family_properties_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, nullptr);
    queue_family_properties_ = std::vector<VkQueueFamilyProperties>(queue_family_properties_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count,
                                             queue_family_properties_.data());
}

Instance &PhysicalDevice::GetInstance() const { return instance_; }

VkBool32 PhysicalDevice::IsPresentSupported(VkSurfaceKHR surface, uint32_t queue_family_index) const {
    VkBool32 present_supported{VK_FALSE};

    if (surface != VK_NULL_HANDLE) {
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(handle_, queue_family_index, surface, &present_supported));
    }

    return present_supported;
}

VkFormatProperties PhysicalDevice::GetFormatProperties(VkFormat format) const {
    VkFormatProperties format_properties;

    vkGetPhysicalDeviceFormatProperties(handle_, format, &format_properties);

    return format_properties;
}

VkPhysicalDevice PhysicalDevice::GetHandle() const { return handle_; }

const VkPhysicalDeviceFeatures &PhysicalDevice::GetFeatures() const { return features_; }

const VkPhysicalDeviceProperties &PhysicalDevice::GetProperties() const { return properties_; }

const VkPhysicalDeviceMemoryProperties &PhysicalDevice::GetMemoryProperties() const { return memory_properties_; }

const std::vector<VkQueueFamilyProperties> &PhysicalDevice::GetQueueFamilyProperties() const {
    return queue_family_properties_;
}

uint32_t PhysicalDevice::GetQueueFamilyPerformanceQueryPasses(
        const VkQueryPoolPerformanceCreateInfoKHR *perf_query_create_info) const {
    uint32_t passes_needed;
    vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(GetHandle(), perf_query_create_info, &passes_needed);
    return passes_needed;
}

void PhysicalDevice::EnumerateQueueFamilyPerformanceQueryCounters(
        uint32_t queue_family_index,
        uint32_t *count,
        VkPerformanceCounterKHR *counters,
        VkPerformanceCounterDescriptionKHR *descriptions) const {
    VK_CHECK(vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(GetHandle(), queue_family_index, count,
                                                                             counters, descriptions));
}

VkPhysicalDeviceFeatures PhysicalDevice::GetRequestedFeatures() const { return requested_features_; }

VkPhysicalDeviceFeatures &PhysicalDevice::GetMutableRequestedFeatures() { return requested_features_; }

void *PhysicalDevice::GetExtensionFeatureChain() const { return last_requested_extension_feature_; }

}  // namespace vox
