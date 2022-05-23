//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/core/instance.h"

namespace vox {
class Instance;

/**
 * @brief A wrapper class for VkPhysicalDevice
 *
 * This class is responsible for handling gpu features, properties, and queue families for the device creation.
 */
class PhysicalDevice {
public:
    PhysicalDevice(Instance &instance, VkPhysicalDevice physical_device);

    PhysicalDevice(const PhysicalDevice &) = delete;

    PhysicalDevice(PhysicalDevice &&) = delete;

    PhysicalDevice &operator=(const PhysicalDevice &) = delete;

    PhysicalDevice &operator=(PhysicalDevice &&) = delete;

    [[nodiscard]] Instance &GetInstance() const;

    VkBool32 IsPresentSupported(VkSurfaceKHR surface, uint32_t queue_family_index) const;

    [[nodiscard]] VkFormatProperties GetFormatProperties(VkFormat format) const;

    [[nodiscard]] VkPhysicalDevice GetHandle() const;

    [[nodiscard]] const VkPhysicalDeviceFeatures &GetFeatures() const;

    [[nodiscard]] const VkPhysicalDeviceProperties &GetProperties() const;

    [[nodiscard]] const VkPhysicalDeviceMemoryProperties &GetMemoryProperties() const;

    [[nodiscard]] const std::vector<VkQueueFamilyProperties> &GetQueueFamilyProperties() const;

    uint32_t GetQueueFamilyPerformanceQueryPasses(
            const VkQueryPoolPerformanceCreateInfoKHR *perf_query_create_info) const;

    void EnumerateQueueFamilyPerformanceQueryCounters(uint32_t queue_family_index,
                                                      uint32_t *count,
                                                      VkPerformanceCounterKHR *counters,
                                                      VkPerformanceCounterDescriptionKHR *descriptions) const;

    [[nodiscard]] VkPhysicalDeviceFeatures GetRequestedFeatures() const;

    VkPhysicalDeviceFeatures &GetMutableRequestedFeatures();

    /**
     * @brief Used at logical device creation to pass the extensions feature chain to vkCreateDevice
     * @returns A void pointer to the start of the extension linked list
     */
    [[nodiscard]] void *GetExtensionFeatureChain() const;

    /**
     * @brief Requests a third party extension to be used by the framework
     *
     *        To have the features enabled, this function must be called before the logical device
     *        is created. To do this request sample specific features inside
     *        GraphicsApplication::RequestGpuFeatures(vox::PhysicalDevice &gpu).
     *
     *        If the feature extension requires you to ask for certain features to be enabled, you can
     *        modify the struct returned by this function, it will propagate the changes to the logical
     *        device.
     * @param type The VkStructureType for the extension you are requesting
     * @returns The extension feature struct
     */
    template <typename T>
    T &RequestExtensionFeatures(VkStructureType type) {
        // We cannot request extension features if the physical device properties 2 instance extension isn't enabled
        if (!instance_.IsEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            throw std::runtime_error("Couldn't request feature from device as " +
                                     std::string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) +
                                     " isn't enabled!");
        }

        // If the type already exists in the map, return a casted pointer to get the extension feature struct
        auto extension_features_it = extension_features_.find(type);
        if (extension_features_it != extension_features_.end()) {
            return *static_cast<T *>(extension_features_it->second.get());
        }

        // Get the extension feature
        VkPhysicalDeviceFeatures2KHR physical_device_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR};
        T extension{type};
        physical_device_features.pNext = &extension;
        vkGetPhysicalDeviceFeatures2KHR(handle_, &physical_device_features);

        // Insert the extension feature into the extension feature map so its ownership is held
        extension_features_.insert({type, std::make_shared<T>(extension)});

        // Pull out the dereferenced void pointer, we can assume its type based on the template
        auto *extension_ptr = static_cast<T *>(extension_features_.find(type)->second.get());

        // If an extension feature has already been requested, we shift the linked list down by one
        // Making this current extension the new base pointer
        if (last_requested_extension_feature_) {
            extension_ptr->pNext = last_requested_extension_feature_;
        }
        last_requested_extension_feature_ = extension_ptr;

        return *extension_ptr;
    }

    /**
     * @brief Sets whether or not the first graphics queue should have higher priority than other queues.
     * Very specific feature which is used by async compute samples.
     * @param enable If true, present queue will have prio 1.0 and other queues have prio 0.5.
     * Default state is false, where all queues have 0.5 priority.
     */
    void SetHighPriorityGraphicsQueueEnable(bool enable) { high_priority_graphics_queue_ = enable; }

    /**
     * @brief Returns high priority graphics queue state.
     * @return High priority state.
     */
    [[nodiscard]] bool HasHighPriorityGraphicsQueue() const { return high_priority_graphics_queue_; }

private:
    // Handle to the Vulkan instance
    Instance &instance_;

    // Handle to the Vulkan physical device
    VkPhysicalDevice handle_{VK_NULL_HANDLE};

    // The features that this GPU supports
    VkPhysicalDeviceFeatures features_{};

    // The GPU properties
    VkPhysicalDeviceProperties properties_{};

    // The GPU memory properties
    VkPhysicalDeviceMemoryProperties memory_properties_{};

    // The GPU queue family properties
    std::vector<VkQueueFamilyProperties> queue_family_properties_;

    // The features that will be requested to be enabled in the logical device
    VkPhysicalDeviceFeatures requested_features_{};

    // The extension feature pointer
    void *last_requested_extension_feature_{nullptr};

    // Holds the extension feature structures, we use a map to retain an order of requested structures
    std::map<VkStructureType, std::shared_ptr<void>> extension_features_;

    bool high_priority_graphics_queue_{};
};

}  // namespace vox
