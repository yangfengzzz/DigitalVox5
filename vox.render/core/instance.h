//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/error.h"
#include "vox.render/vk_common.h"

namespace vox {
class PhysicalDevice;

/**
 * @brief Returns a list of Khronos/LunarG supported validation layers
 *        Attempting to enable them in order of preference, starting with later Vulkan SDK versions
 * @param supported_instance_layers A list of validation layers to check against
 */
std::vector<const char *> GetOptimalValidationLayers(const std::vector<VkLayerProperties> &supported_instance_layers);

/**
 * @brief A wrapper class for VkInstance
 *
 * This class is responsible for initializing volk, enumerating over all available extensions and validation layers
 * enabling them if they exist, setting up debug messaging and querying all the physical devices existing on the
 * machine.
 */
class Instance {
public:
    /**
     * @brief Initializes the connection to Vulkan
     * @param application_name The name of the application
     * @param required_extensions The extensions requested to be enabled
     * @param required_validation_layers The validation layers to be enabled
     * @param headless Whether the application is requesting a headless setup or not
     * @param api_version The Vulkan API version that the instance will be using
     * @throws runtime_error if the required extensions and validation layers are not found
     */
    explicit Instance(const std::string &application_name,
                      const std::unordered_map<const char *, bool> &required_extensions = {},
                      const std::vector<const char *> &required_validation_layers = {},
                      bool headless = false,
                      uint32_t api_version = VK_API_VERSION_1_0);

    /**
     * @brief Queries the GPUs of a VkInstance that is already created
     * @param instance A valid VkInstance
     */
    explicit Instance(VkInstance instance);

    Instance(const Instance &) = delete;

    Instance(Instance &&) = delete;

    ~Instance();

    Instance &operator=(const Instance &) = delete;

    Instance &operator=(Instance &&) = delete;

    /**
     * @brief Queries the instance for the physical devices on the machine
     */
    void QueryGpus();

    /**
     * @brief Tries to find the first available discrete GPU that can render to the given surface
     * @param surface to test against
     * @returns A valid physical device
     */
    PhysicalDevice &GetSuitableGpu(VkSurfaceKHR surface);

    /**
     * @brief Tries to find the first available discrete GPU
     * @returns A valid physical device
     */
    PhysicalDevice &GetFirstGpu();

    /**
     * @brief Checks if the given extension is enabled in the VkInstance
     * @param extension An extension to check
     */
    bool IsEnabled(const char *extension) const;

    [[nodiscard]] VkInstance GetHandle() const;

    const std::vector<const char *> &GetExtensions();

    /**
     * @brief Returns a const ref to the properties of all requested layers in this instance
     * @returns The VkLayerProperties for all requested layers in this instance
     */
    const std::vector<VkLayerProperties> &GetLayerProperties();

    /**
     * @brief Finds layer properties for the layer with the given name
     * @param layer_name The layer to search for
     * @param properties A reference to a VkLayerProperties struct to populate
     * @returns True if the layer was found and populated, false otherwise
     */
    bool GetLayerProperties(const char *layer_name, VkLayerProperties &properties);

private:
    /**
     * @brief The Vulkan instance
     */
    VkInstance handle_{VK_NULL_HANDLE};

    /**
     * @brief The enabled extensions
     */
    std::vector<const char *> enabled_extensions_;

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    /**
     * @brief Debug utils messenger callback for VK_EXT_Debug_Utils
     */
    VkDebugUtilsMessengerEXT debug_utils_messenger_{VK_NULL_HANDLE};

    /**
     * @brief The debug report callback
     */
    VkDebugReportCallbackEXT debug_report_callback_{VK_NULL_HANDLE};
#endif

    /**
     * @brief The physical devices found on the machine
     */
    std::vector<std::unique_ptr<PhysicalDevice>> gpus_;
};  // namespace Instance

}  // namespace vox
