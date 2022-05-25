//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/swapchain.h"

#include "vox.base/logging.h"
#include "vox.render/core/device.h"

namespace vox {
namespace {
inline uint32_t ChooseImageCount(uint32_t request_image_count, uint32_t min_image_count, uint32_t max_image_count) {
    if (max_image_count != 0) {
        request_image_count = std::min(request_image_count, max_image_count);
    }

    request_image_count = std::max(request_image_count, min_image_count);

    return request_image_count;
}

inline uint32_t ChooseImageArrayLayers(uint32_t request_image_array_layers, uint32_t max_image_array_layers) {
    request_image_array_layers = std::min(request_image_array_layers, max_image_array_layers);
    request_image_array_layers = std::max(request_image_array_layers, 1u);

    return request_image_array_layers;
}

inline VkExtent2D ChooseExtent(VkExtent2D request_extent,
                               const VkExtent2D &min_image_extent,
                               const VkExtent2D &max_image_extent,
                               const VkExtent2D &current_extent) {
    if (current_extent.width == 0xFFFFFFFF) {
        return request_extent;
    }

    if (request_extent.width < 1 || request_extent.height < 1) {
        LOGW("(Swapchain) Image extent ({}, {}) not supported. Selecting ({}, {}).", request_extent.width,
             request_extent.height, current_extent.width, current_extent.height)
        return current_extent;
    }

    request_extent.width = std::max(request_extent.width, min_image_extent.width);
    request_extent.width = std::min(request_extent.width, max_image_extent.width);

    request_extent.height = std::max(request_extent.height, min_image_extent.height);
    request_extent.height = std::min(request_extent.height, max_image_extent.height);

    return request_extent;
}

inline VkPresentModeKHR ChoosePresentMode(VkPresentModeKHR request_present_mode,
                                          const std::vector<VkPresentModeKHR> &available_present_modes,
                                          const std::vector<VkPresentModeKHR> &present_mode_priority_list) {
    auto present_mode_it =
            std::find(available_present_modes.begin(), available_present_modes.end(), request_present_mode);

    if (present_mode_it == available_present_modes.end()) {
        // If nothing found, always default to FIFO
        VkPresentModeKHR chosen_present_mode = VK_PRESENT_MODE_FIFO_KHR;

        for (auto &present_mode : present_mode_priority_list) {
            if (std::find(available_present_modes.begin(), available_present_modes.end(), present_mode) !=
                available_present_modes.end()) {
                chosen_present_mode = present_mode;
                break;
            }
        }

        LOGW("(Swapchain) Present mode '{}' not supported. Selecting '{}'.", vox::ToString(request_present_mode),
             vox::ToString(chosen_present_mode))
        return chosen_present_mode;
    } else {
        LOGI("(Swapchain) Present mode selected: {}", vox::ToString(request_present_mode))
        return *present_mode_it;
    }
}

inline VkSurfaceFormatKHR ChooseSurfaceFormat(const VkSurfaceFormatKHR requested_surface_format,
                                              const std::vector<VkSurfaceFormatKHR> &available_surface_formats,
                                              const std::vector<VkSurfaceFormatKHR> &surface_format_priority_list) {
    // Try to find the requested surface format in the supported surface formats
    auto surface_format_it = std::find_if(available_surface_formats.begin(), available_surface_formats.end(),
                                          [&requested_surface_format](const VkSurfaceFormatKHR &surface) {
                                              if (surface.format == requested_surface_format.format &&
                                                  surface.colorSpace == requested_surface_format.colorSpace) {
                                                  return true;
                                              }

                                              return false;
                                          });

    // If the requested surface format isn't found, then try to request a format from the priority list
    if (surface_format_it == available_surface_formats.end()) {
        for (auto &surface_format : surface_format_priority_list) {
            surface_format_it = std::find_if(available_surface_formats.begin(), available_surface_formats.end(),
                                             [&surface_format](const VkSurfaceFormatKHR &surface) {
                                                 if (surface.format == surface_format.format &&
                                                     surface.colorSpace == surface_format.colorSpace) {
                                                     return true;
                                                 }

                                                 return false;
                                             });
            if (surface_format_it != available_surface_formats.end()) {
                LOGW("(Swapchain) Surface format ({}) not supported. Selecting ({}).",
                     vox::ToString(requested_surface_format), vox::ToString(*surface_format_it))
                return *surface_format_it;
            }
        }

        // If nothing found, default the first supported surface format
        surface_format_it = available_surface_formats.begin();
        LOGW("(Swapchain) Surface format ({}) not supported. Selecting ({}).", vox::ToString(requested_surface_format),
             vox::ToString(*surface_format_it))
    } else {
        LOGI("(Swapchain) Surface format selected: {}", vox::ToString(requested_surface_format))
    }

    return *surface_format_it;
}

inline VkSurfaceTransformFlagBitsKHR ChooseTransform(VkSurfaceTransformFlagBitsKHR request_transform,
                                                     VkSurfaceTransformFlagsKHR supported_transform,
                                                     VkSurfaceTransformFlagBitsKHR current_transform) {
    if (request_transform & supported_transform) {
        return request_transform;
    }

    LOGW("(Swapchain) Surface transform '{}' not supported. Selecting '{}'.", vox::ToString(request_transform),
         vox::ToString(current_transform))

    return current_transform;
}

inline VkCompositeAlphaFlagBitsKHR ChooseCompositeAlpha(VkCompositeAlphaFlagBitsKHR request_composite_alpha,
                                                        VkCompositeAlphaFlagsKHR supported_composite_alpha) {
    if (request_composite_alpha & supported_composite_alpha) {
        return request_composite_alpha;
    }

    static const std::vector<VkCompositeAlphaFlagBitsKHR> kCompositeAlphaFlags = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR};

    for (VkCompositeAlphaFlagBitsKHR composite_alpha : kCompositeAlphaFlags) {
        if (composite_alpha & supported_composite_alpha) {
            LOGW("(Swapchain) Composite alpha '{}' not supported. Selecting '{}.",
                 vox::ToString(request_composite_alpha), vox::ToString(composite_alpha))
            return composite_alpha;
        }
    }

    throw std::runtime_error("No compatible composite alpha found.");
}

inline bool ValidateFormatFeature(VkImageUsageFlagBits image_usage, VkFormatFeatureFlags supported_features) {
    switch (image_usage) {
        case VK_IMAGE_USAGE_STORAGE_BIT:
            return VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT & supported_features;
        default:
            return true;
    }
}

inline std::set<VkImageUsageFlagBits> ChooseImageUsage(
        const std::set<VkImageUsageFlagBits> &requested_image_usage_flags,
        VkImageUsageFlags supported_image_usage,
        VkFormatFeatureFlags supported_features) {
    std::set<VkImageUsageFlagBits> validated_image_usage_flags;
    for (auto flag : requested_image_usage_flags) {
        if ((flag & supported_image_usage) && ValidateFormatFeature(flag, supported_features)) {
            validated_image_usage_flags.insert(flag);
        } else {
            LOGW("(Swapchain) Image usage ({}) requested but not supported.", vox::ToString(flag))
        }
    }

    if (validated_image_usage_flags.empty()) {
        // Pick the first format from list of defaults, if supported
        static const std::vector<VkImageUsageFlagBits> kImageUsageFlags = {
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_STORAGE_BIT, VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT};

        for (VkImageUsageFlagBits image_usage : kImageUsageFlags) {
            if ((image_usage & supported_image_usage) && ValidateFormatFeature(image_usage, supported_features)) {
                validated_image_usage_flags.insert(image_usage);
                break;
            }
        }
    }

    if (!validated_image_usage_flags.empty()) {
        // Log image usage flags used
        std::string usage_list;
        for (VkImageUsageFlagBits image_usage : validated_image_usage_flags) {
            usage_list += vox::ToString(image_usage) + " ";
        }
        LOGI("(Swapchain) Image usage flags: {}", usage_list)
    } else {
        throw std::runtime_error("No compatible image usage found.");
    }

    return validated_image_usage_flags;
}

inline VkImageUsageFlags CompositeImageFlags(std::set<VkImageUsageFlagBits> &image_usage_flags) {
    VkImageUsageFlags image_usage{};
    for (auto flag : image_usage_flags) {
        image_usage |= flag;
    }
    return image_usage;
}

}  // namespace

Swapchain::Swapchain(Swapchain &old_swapchain, const VkExtent2D &extent)
    : Swapchain{old_swapchain,
                old_swapchain.device_,
                old_swapchain.surface_,
                extent,
                old_swapchain.properties_.image_count,
                old_swapchain.properties_.pre_transform,
                old_swapchain.properties_.present_mode,
                old_swapchain.image_usage_flags_} {
    present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
    surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;
    Create();
}

Swapchain::Swapchain(Swapchain &old_swapchain, const uint32_t image_count)
    : Swapchain{old_swapchain,
                old_swapchain.device_,
                old_swapchain.surface_,
                old_swapchain.properties_.extent,
                image_count,
                old_swapchain.properties_.pre_transform,
                old_swapchain.properties_.present_mode,
                old_swapchain.image_usage_flags_} {
    present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
    surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;
    Create();
}

Swapchain::Swapchain(Swapchain &old_swapchain, const std::set<VkImageUsageFlagBits> &image_usage_flags)
    : Swapchain{old_swapchain,
                old_swapchain.device_,
                old_swapchain.surface_,
                old_swapchain.properties_.extent,
                old_swapchain.properties_.image_count,
                old_swapchain.properties_.pre_transform,
                old_swapchain.properties_.present_mode,
                image_usage_flags} {
    present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
    surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;
    Create();
}

Swapchain::Swapchain(Swapchain &old_swapchain, const VkExtent2D &extent, const VkSurfaceTransformFlagBitsKHR transform)
    :

      Swapchain{old_swapchain,
                old_swapchain.device_,
                old_swapchain.surface_,
                extent,
                old_swapchain.properties_.image_count,
                transform,
                old_swapchain.properties_.present_mode,
                old_swapchain.image_usage_flags_} {
    present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
    surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;
    Create();
}

Swapchain::Swapchain(Device &device,
                     VkSurfaceKHR surface,
                     const VkExtent2D &extent,
                     const uint32_t image_count,
                     const VkSurfaceTransformFlagBitsKHR transform,
                     const VkPresentModeKHR present_mode,
                     const std::set<VkImageUsageFlagBits> &image_usage_flags)
    : Swapchain{*this, device, surface, extent, image_count, transform, present_mode, image_usage_flags} {}

Swapchain::Swapchain(Swapchain &old_swapchain,
                     Device &device,
                     VkSurfaceKHR surface,
                     const VkExtent2D &extent,
                     const uint32_t image_count,
                     const VkSurfaceTransformFlagBitsKHR transform,
                     const VkPresentModeKHR present_mode,
                     const std::set<VkImageUsageFlagBits> &image_usage_flags)
    : device_{device}, surface_{surface} {
    present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
    surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;

    VkSurfaceCapabilitiesKHR surface_capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_.GetGpu().GetHandle(), surface, &surface_capabilities);

    uint32_t surface_format_count{0U};
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device_.GetGpu().GetHandle(), surface, &surface_format_count,
                                                  nullptr));
    surface_formats_.resize(surface_format_count);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device_.GetGpu().GetHandle(), surface, &surface_format_count,
                                                  surface_formats_.data()));

    LOGI("Surface supports the following surface formats:")
    for (auto &surface_format : surface_formats_) {
        LOGI("  \t{}", vox::ToString(surface_format))
    }

    uint32_t present_mode_count{0U};
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device_.GetGpu().GetHandle(), surface, &present_mode_count,
                                                       nullptr));
    present_modes_.resize(present_mode_count);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device_.GetGpu().GetHandle(), surface, &present_mode_count,
                                                       present_modes_.data()));

    LOGI("Surface supports the following present modes:")
    for (auto &present_mode : present_modes_) {
        LOGI("  \t{}", vox::ToString(present_mode))
    }

    // Choose the best properties based on surface capabilities
    properties_.image_count =
            ChooseImageCount(image_count, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);
    properties_.extent = ChooseExtent(extent, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent,
                                      surface_capabilities.currentExtent);
    properties_.array_layers = ChooseImageArrayLayers(1U, surface_capabilities.maxImageArrayLayers);
    properties_.surface_format =
            ChooseSurfaceFormat(properties_.surface_format, surface_formats_, surface_format_priority_list_);
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(device_.GetGpu().GetHandle(), properties_.surface_format.format,
                                        &format_properties);
    image_usage_flags_ = ChooseImageUsage(image_usage_flags, surface_capabilities.supportedUsageFlags,
                                          format_properties.optimalTilingFeatures);
    properties_.image_usage = CompositeImageFlags(image_usage_flags_);
    properties_.pre_transform =
            ChooseTransform(transform, surface_capabilities.supportedTransforms, surface_capabilities.currentTransform);
    properties_.composite_alpha =
            ChooseCompositeAlpha(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR, surface_capabilities.supportedCompositeAlpha);

    // Pass through defaults to the create function
    properties_.old_swapchain = old_swapchain.GetHandle();
    properties_.present_mode = present_mode;
}

Swapchain::~Swapchain() {
    if (handle_ != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device_.GetHandle(), handle_, nullptr);
    }
}

Swapchain::Swapchain(Swapchain &&other) noexcept
    : device_{other.device_},
      surface_{other.surface_},
      handle_{other.handle_},
      image_usage_flags_{std::move(other.image_usage_flags_)},
      images_{std::move(other.images_)},
      properties_{other.properties_} {
    other.handle_ = VK_NULL_HANDLE;
    other.surface_ = VK_NULL_HANDLE;
}

void Swapchain::Create() {
    // Revalidate the present mode and surface format
    properties_.present_mode = ChoosePresentMode(properties_.present_mode, present_modes_, present_mode_priority_list_);
    properties_.surface_format =
            ChooseSurfaceFormat(properties_.surface_format, surface_formats_, surface_format_priority_list_);

    VkSwapchainCreateInfoKHR create_info{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    create_info.minImageCount = properties_.image_count;
    create_info.imageExtent = properties_.extent;
    create_info.presentMode = properties_.present_mode;
    create_info.imageFormat = properties_.surface_format.format;
    create_info.imageColorSpace = properties_.surface_format.colorSpace;
    create_info.imageArrayLayers = properties_.array_layers;
    create_info.imageUsage = properties_.image_usage;
    create_info.preTransform = properties_.pre_transform;
    create_info.compositeAlpha = properties_.composite_alpha;
    create_info.oldSwapchain = properties_.old_swapchain;
    create_info.surface = surface_;

    VkResult result = vkCreateSwapchainKHR(device_.GetHandle(), &create_info, nullptr, &handle_);

    if (result != VK_SUCCESS) {
        throw VulkanException{result, "Cannot create Swapchain"};
    }

    uint32_t image_available{0u};
    VK_CHECK(vkGetSwapchainImagesKHR(device_.GetHandle(), handle_, &image_available, nullptr));

    images_.resize(image_available);

    VK_CHECK(vkGetSwapchainImagesKHR(device_.GetHandle(), handle_, &image_available, images_.data()));
}  // namespace vox

bool Swapchain::IsValid() const { return handle_ != VK_NULL_HANDLE; }

Device &Swapchain::GetDevice() { return device_; }

VkSwapchainKHR Swapchain::GetHandle() const { return handle_; }

SwapchainProperties &Swapchain::GetProperties() { return properties_; }

VkResult Swapchain::AcquireNextImage(uint32_t &image_index, VkSemaphore image_acquired_semaphore, VkFence fence) const {
    return vkAcquireNextImageKHR(device_.GetHandle(), handle_, std::numeric_limits<uint64_t>::max(),
                                 image_acquired_semaphore, fence, &image_index);
}

const VkExtent2D &Swapchain::GetExtent() const { return properties_.extent; }

VkFormat Swapchain::GetFormat() const { return properties_.surface_format.format; }

const std::vector<VkImage> &Swapchain::GetImages() const { return images_; }

VkSurfaceTransformFlagBitsKHR Swapchain::GetTransform() const { return properties_.pre_transform; }

VkSurfaceKHR Swapchain::GetSurface() const { return surface_; }

VkImageUsageFlags Swapchain::GetUsage() const { return properties_.image_usage; }

void Swapchain::SetPresentModePriority(const std::vector<VkPresentModeKHR> &present_mode_priority_list) {
    assert(!present_mode_priority_list.empty() && "Priority list must not be empty");
    present_mode_priority_list_ = present_mode_priority_list;
}

void Swapchain::SetSurfaceFormatPriority(const std::vector<VkSurfaceFormatKHR> &surface_format_priority_list) {
    assert(!surface_format_priority_list.empty() && "Priority list must not be empty");
    surface_format_priority_list_ = surface_format_priority_list;
}

VkPresentModeKHR Swapchain::GetPresentMode() const { return properties_.present_mode; }

}  // namespace vox
