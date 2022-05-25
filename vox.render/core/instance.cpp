//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/instance.h"

#include <algorithm>
#include <functional>

#include "vox.render/core/physical_device.h"

namespace vox {
namespace {
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)

VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                           VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                           const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                                                           void *user_data) {
    // Log debug message
    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOGW("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage)
    } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LOGE("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage)
    }
    return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags,
                                             VkDebugReportObjectTypeEXT /*type*/,
                                             uint64_t /*object*/,
                                             size_t /*location*/,
                                             int32_t /*message_code*/,
                                             const char *layer_prefix,
                                             const char *message,
                                             void * /*user_data*/) {
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        LOGE("{}: {}", layer_prefix, message)
    } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        LOGW("{}: {}", layer_prefix, message)
    } else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        LOGW("{}: {}", layer_prefix, message)
    } else {
        LOGI("{}: {}", layer_prefix, message)
    }
    return VK_FALSE;
}

#endif

bool ValidateLayers(const std::vector<const char *> &required, const std::vector<VkLayerProperties> &available) {
    for (auto layer : required) {
        bool found = false;
        for (auto &available_layer : available) {
            if (strcmp(available_layer.layerName, layer) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            LOGE("Validation Layer {} not found", layer)
            return false;
        }
    }

    return true;
}
}  // namespace

std::vector<const char *> GetOptimalValidationLayers(const std::vector<VkLayerProperties> &supported_instance_layers) {
    std::vector<std::vector<const char *>> validation_layer_priority_list = {
            // The preferred validation layer is "VK_LAYER_KHRONOS_validation"
            {"VK_LAYER_KHRONOS_validation"},

            // Otherwise, we fall back to using the LunarG meta layer
            {"VK_LAYER_LUNARG_standard_validation"},

            // Otherwise, we attempt to enable the individual layers that compose the LunarG meta layer since it doesn't
            // exist
            {
                    "VK_LAYER_GOOGLE_threading",
                    "VK_LAYER_LUNARG_parameter_validation",
                    "VK_LAYER_LUNARG_object_tracker",
                    "VK_LAYER_LUNARG_core_validation",
                    "VK_LAYER_GOOGLE_unique_objects",
            },

            // Otherwise, as a last resort we fall back to attempting to enable the LunarG core layer
            {"VK_LAYER_LUNARG_core_validation"}};

    for (auto &validation_layers : validation_layer_priority_list) {
        if (ValidateLayers(validation_layers, supported_instance_layers)) {
            return validation_layers;
        }

        LOGW("Couldn't enable validation layers (see log for error) - falling back")
    }

    // Else return nothing
    return {};
}

namespace {
bool EnableExtension(const char *required_ext_name,
                     const std::vector<VkExtensionProperties> &available_exts,
                     std::vector<const char *> &enabled_extensions) {
    for (auto &avail_ext_it : available_exts) {
        if (strcmp(avail_ext_it.extensionName, required_ext_name) == 0) {
            auto it = std::find_if(enabled_extensions.begin(), enabled_extensions.end(),
                                   [required_ext_name](const char *enabled_ext_name) {
                                       return strcmp(enabled_ext_name, required_ext_name) == 0;
                                   });
            if (it != enabled_extensions.end()) {
                // Extension is already enabled
            } else {
                LOGI("Extension {} found, enabling it", required_ext_name)
                enabled_extensions.emplace_back(required_ext_name);
            }
            return true;
        }
    }

    LOGI("Extension {} not found", required_ext_name)
    return false;
}

bool EnableAllExtensions(const std::vector<const char *> &required_ext_names,
                         const std::vector<VkExtensionProperties> &available_exts,
                         std::vector<const char *> &enabled_extensions) {
    using std::placeholders::_1;

    return std::all_of(required_ext_names.begin(), required_ext_names.end(),
                       std::bind(EnableExtension, _1, available_exts, enabled_extensions));
}

}  // namespace

Instance::Instance(const std::string &application_name,
                   const std::unordered_map<const char *, bool> &required_extensions,
                   const std::vector<const char *> &required_validation_layers,
                   bool headless,
                   uint32_t api_version) {
    uint32_t instance_extension_count;
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr));

    std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count,
                                                    available_instance_extensions.data()));

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    // Check if VK_EXT_debug_utils is supported, which supersedes VK_EXT_Debug_Report
    const bool kHasDebugUtils =
            EnableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, available_instance_extensions, enabled_extensions_);
    bool has_debug_report;
    if (!kHasDebugUtils) {
        has_debug_report =
                EnableExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, available_instance_extensions, enabled_extensions_);
        if (!has_debug_report) {
            LOGW("Neither of {} or {} are available; disabling debug reporting", VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                 VK_EXT_DEBUG_REPORT_EXTENSION_NAME)
        }
    }
#endif

#if (defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)) && defined(VKB_VALIDATION_LAYERS_GPU_ASSISTED)
    bool validation_features = false;
    {
        uint32_t layer_instance_extension_count;
        VK_CHECK(vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &layer_instance_extension_count,
                                                        nullptr));

        std::vector<VkExtensionProperties> available_layer_instance_extensions(layer_instance_extension_count);
        VK_CHECK(vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &layer_instance_extension_count,
                                                        available_layer_instance_extensions.data()));

        for (auto &available_extension : available_layer_instance_extensions) {
            if (strcmp(available_extension.extensionName, VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME) == 0) {
                validation_features = true;
                LOGI("{} is available, enabling it", VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
                enabled_extensions_.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
            }
        }
    }
#endif

    // Try to enable headless surface extension if it exists
    if (headless) {
        const bool kHasHeadlessSurface = EnableExtension(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME,
                                                         available_instance_extensions, enabled_extensions_);
        if (!kHasHeadlessSurface) {
            LOGW("{} is not available, disabling swapchain creation", VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME)
        }
    } else {
        enabled_extensions_.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    }

    // VK_KHR_get_physical_device_properties2 is a prerequisite of VK_KHR_performance_query
    // which will be used for stats gathering where available.
    EnableExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, available_instance_extensions,
                    enabled_extensions_);

    auto extension_error = false;
    for (auto extension : required_extensions) {
        auto extension_name = extension.first;
        auto extension_is_optional = extension.second;
        if (!EnableExtension(extension_name, available_instance_extensions, enabled_extensions_)) {
            if (extension_is_optional) {
                LOGW("Optional instance extension {} not available, some features may be disabled", extension_name)
            } else {
                LOGE("Required instance extension {} not available, cannot run", extension_name)
                extension_error = true;
            }
            extension_error = extension_error || !extension_is_optional;
        }
    }

    if (extension_error) {
        throw std::runtime_error("Required instance extensions are missing.");
    }

    uint32_t instance_layer_count;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));

    std::vector<VkLayerProperties> supported_validation_layers(instance_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, supported_validation_layers.data()));

    std::vector<const char *> requested_validation_layers(required_validation_layers);

#ifdef VKB_VALIDATION_LAYERS
    // Determine the optimal validation layers to enable that are necessary for useful debugging
    std::vector<const char *> optimal_validation_layers = get_optimal_validation_layers(supported_validation_layers);
    requested_validation_layers.insert(requested_validation_layers.end(), optimal_validation_layers.begin(),
                                       optimal_validation_layers.end());
#endif

    if (ValidateLayers(requested_validation_layers, supported_validation_layers)) {
        LOGI("Enabled Validation Layers:")
        for (const auto &layer : requested_validation_layers) {
            LOGI("	\t{}", layer)
        }
    } else {
        throw std::runtime_error("Required validation layers are missing.");
    }

    VkApplicationInfo app_info{VK_STRUCTURE_TYPE_APPLICATION_INFO};

    app_info.pApplicationName = application_name.c_str();
    app_info.applicationVersion = 0;
    app_info.pEngineName = "Vulkan Samples";
    app_info.engineVersion = 0;
    app_info.apiVersion = api_version;

    VkInstanceCreateInfo instance_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};

    instance_info.pApplicationInfo = &app_info;

    instance_info.enabledExtensionCount = utility::ToU32(enabled_extensions_.size());
    instance_info.ppEnabledExtensionNames = enabled_extensions_.data();

    instance_info.enabledLayerCount = utility::ToU32(requested_validation_layers.size());
    instance_info.ppEnabledLayerNames = requested_validation_layers.data();

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info = {
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    VkDebugReportCallbackCreateInfoEXT debug_report_create_info = {VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT};
    if (kHasDebugUtils) {
        debug_utils_create_info.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debug_utils_create_info.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_utils_create_info.pfnUserCallback = DebugUtilsMessengerCallback;

        instance_info.pNext = &debug_utils_create_info;
    } else {
        debug_report_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                         VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_create_info.pfnCallback = DebugCallback;

        instance_info.pNext = &debug_report_create_info;
    }
#endif

#if (defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)) && defined(VKB_VALIDATION_LAYERS_GPU_ASSISTED)
    VkValidationFeaturesEXT validation_features_info = {VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT};
    if (validation_features) {
        static const VkValidationFeatureEnableEXT enable_features[2] = {
                VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
                VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
        };
        validation_features_info.enabledValidationFeatureCount = 2;
        validation_features_info.pEnabledValidationFeatures = enable_features;
        validation_features_info.pNext = instance_info.pNext;
        instance_info.pNext = &validation_features_info;
    }
#endif

    // Create the Vulkan instance
    VkResult result = vkCreateInstance(&instance_info, nullptr, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException(result, "Could not create Vulkan instance");
    }

    volkLoadInstance(handle_);

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    if (kHasDebugUtils) {
        result = vkCreateDebugUtilsMessengerEXT(handle_, &debug_utils_create_info, nullptr, &debug_utils_messenger_);
        if (result != VK_SUCCESS) {
            throw VulkanException(result, "Could not create debug utils messenger");
        }
    } else {
        result = vkCreateDebugReportCallbackEXT(handle_, &debug_report_create_info, nullptr, &debug_report_callback_);
        if (result != VK_SUCCESS) {
            throw VulkanException(result, "Could not create debug report callback");
        }
    }
#endif

    QueryGpus();
}

Instance::Instance(VkInstance instance) : handle_{instance} {
    if (handle_ != VK_NULL_HANDLE) {
        QueryGpus();
    } else {
        throw std::runtime_error("Instance not valid");
    }
}

Instance::~Instance() {
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    if (debug_utils_messenger_ != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(handle_, debug_utils_messenger_, nullptr);
    }
    if (debug_report_callback_ != VK_NULL_HANDLE) {
        vkDestroyDebugReportCallbackEXT(handle_, debug_report_callback_, nullptr);
    }
#endif

    if (handle_ != VK_NULL_HANDLE) {
        vkDestroyInstance(handle_, nullptr);
    }
}

void Instance::QueryGpus() {
    // Querying valid physical devices on the machine
    uint32_t physical_device_count{0};
    VK_CHECK(vkEnumeratePhysicalDevices(handle_, &physical_device_count, nullptr));

    if (physical_device_count < 1) {
        throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
    }

    std::vector<VkPhysicalDevice> physical_devices;
    physical_devices.resize(physical_device_count);
    VK_CHECK(vkEnumeratePhysicalDevices(handle_, &physical_device_count, physical_devices.data()));

    // Create gpus wrapper objects from the VkPhysicalDevice's
    for (auto &physical_device : physical_devices) {
        gpus_.push_back(std::make_unique<PhysicalDevice>(*this, physical_device));
    }
}

PhysicalDevice &Instance::GetFirstGpu() {
    assert(!gpus_.empty() && "No physical devices were found on the system.");

    // Find a discrete GPU
    for (auto &gpu : gpus_) {
        if (gpu->GetProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return *gpu;
        }
    }

    // Otherwise, just pick the first one
    LOGW("Couldn't find a discrete physical device, picking default GPU")
    return *gpus_.at(0);
}

PhysicalDevice &Instance::GetSuitableGpu(VkSurfaceKHR surface) {
    assert(!gpus_.empty() && "No physical devices were found on the system.");

    // Find a discrete GPU
    for (auto &gpu : gpus_) {
        if (gpu->GetProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            // See if it works with the surface
            size_t queue_count = gpu->GetQueueFamilyProperties().size();
            for (uint32_t queue_idx = 0; static_cast<size_t>(queue_idx) < queue_count; queue_idx++) {
                if (gpu->IsPresentSupported(surface, queue_idx)) {
                    return *gpu;
                }
            }
        }
    }

    // Otherwise, just pick the first one
    LOGW("Couldn't find a discrete physical device, picking default GPU")
    return *gpus_.at(0);
}

bool Instance::IsEnabled(const char *extension) const {
    return std::find_if(enabled_extensions_.begin(), enabled_extensions_.end(),
                        [extension](const char *enabled_extension) {
                            return strcmp(extension, enabled_extension) == 0;
                        }) != enabled_extensions_.end();
}

VkInstance Instance::GetHandle() const { return handle_; }

const std::vector<const char *> &Instance::GetExtensions() { return enabled_extensions_; }

}  // namespace vox
