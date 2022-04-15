//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "helpers.h"
#include "vk_common.h"

namespace vox {
class Device;

enum ImageFormat {
    S_RGB,
    UNORM
};

struct SwapchainProperties {
    VkSwapchainKHR old_swapchain{};
    uint32_t image_count{3};
    VkExtent2D extent{};
    VkSurfaceFormatKHR surface_format{};
    uint32_t array_layers{};
    VkImageUsageFlags image_usage{};
    VkSurfaceTransformFlagBitsKHR pre_transform{};
    VkCompositeAlphaFlagBitsKHR composite_alpha{};
    VkPresentModeKHR present_mode{};
};

class Swapchain {
public:
    /**
     * @brief Constructor to create a swapchain by changing the extent
     *        only and preserving the configuration from the old swapchain.
     */
    Swapchain(Swapchain &old_swapchain, const VkExtent2D &extent);
    
    /**
     * @brief Constructor to create a swapchain by changing the image count
     *        only and preserving the configuration from the old swapchain.
     */
    Swapchain(Swapchain &old_swapchain, uint32_t image_count);
    
    /**
     * @brief Constructor to create a swapchain by changing the image usage
     * only and preserving the configuration from the old swapchain.
     */
    Swapchain(Swapchain &old_swapchain, const std::set<VkImageUsageFlagBits> &image_usage_flags);
    
    /**
     * @brief Constructor to create a swapchain by changing the extent
     *        and transform only and preserving the configuration from the old swapchain.
     */
    Swapchain(Swapchain &swapchain, const VkExtent2D &extent, VkSurfaceTransformFlagBitsKHR transform);
    
    /**
     * @brief Constructor to create a swapchain.
     */
    Swapchain(Device &device,
              VkSurfaceKHR surface,
              const VkExtent2D &extent = {},
              uint32_t image_count = 3,
              VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
              VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR,
              const std::set<VkImageUsageFlagBits> &image_usage_flags = {VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT});
    
    /**
     * @brief Constructor to create a swapchain from the old swapchain
     *        by configuring all parameters.
     */
    Swapchain(Swapchain &old_swapchain,
              Device &device,
              VkSurfaceKHR surface,
              const VkExtent2D &extent = {},
              uint32_t image_count = 3,
              VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
              VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR,
              const std::set<VkImageUsageFlagBits> &image_usage_flags = {VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT});
    
    Swapchain(const Swapchain &) = delete;
    
    Swapchain(Swapchain &&other) noexcept;
    
    ~Swapchain();
    
    Swapchain &operator=(const Swapchain &) = delete;
    
    Swapchain &operator=(Swapchain &&) = delete;
    
    void create();
    
    [[nodiscard]] bool is_valid() const;
    
    Device &get_device();
    
    [[nodiscard]] VkSwapchainKHR get_handle() const;
    
    SwapchainProperties &get_properties();
    
    VkResult acquire_next_image(uint32_t &image_index, VkSemaphore image_acquired_semaphore,
                                VkFence fence = VK_NULL_HANDLE) const;
    
    [[nodiscard]] const VkExtent2D &get_extent() const;
    
    [[nodiscard]] VkFormat get_format() const;
    
    [[nodiscard]] const std::vector<VkImage> &get_images() const;
    
    [[nodiscard]] VkSurfaceTransformFlagBitsKHR get_transform() const;
    
    [[nodiscard]] VkSurfaceKHR get_surface() const;
    
    [[nodiscard]] VkImageUsageFlags get_usage() const;
    
    [[nodiscard]] VkPresentModeKHR get_present_mode() const;
    
    /**
     * @brief Sets the order in which the swapchain prioritizes selecting its present mode
     */
    void set_present_mode_priority(const std::vector<VkPresentModeKHR> &present_mode_priority_list);
    
    /**
     * @brief Sets the order in which the swapchain prioritizes selecting its surface format
     */
    void set_surface_format_priority(const std::vector<VkSurfaceFormatKHR> &surface_format_priority_list);
    
private:
    Device &device_;
    
    VkSurfaceKHR surface_{VK_NULL_HANDLE};
    
    VkSwapchainKHR handle_{VK_NULL_HANDLE};
    
    std::vector<VkImage> images_;
    
    std::vector<VkSurfaceFormatKHR> surface_formats_{};
    
    std::vector<VkPresentModeKHR> present_modes_{};
    
    SwapchainProperties properties_;
    
    // A list of present modes in order of priority (vector[0] has high priority, vector[size-1] has low priority)
    std::vector<VkPresentModeKHR> present_mode_priority_list_ = {
        VK_PRESENT_MODE_FIFO_KHR,
        VK_PRESENT_MODE_MAILBOX_KHR};
    
    // A list of surface formats in order of priority (vector[0] has high priority, vector[size-1] has low priority)
    std::vector<VkSurfaceFormatKHR> surface_format_priority_list_ = {
        {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};
    
    std::set<VkImageUsageFlagBits> image_usage_flags_;
};

}        // namespace vox
