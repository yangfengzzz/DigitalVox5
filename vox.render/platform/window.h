//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#pragma once

#include "vk_common.h"
#include "core/instance.h"

namespace vox {
/**
 * @brief An interface class, declaring the behaviour of a Window
 */
class Window {
public:
    struct Extent {
        uint32_t width;
        uint32_t height;
    };
    
    struct OptionalExtent {
        std::optional<uint32_t> width;
        std::optional<uint32_t> height;
    };
    
    enum class Mode {
        HEADLESS,
        FULLSCREEN,
        FULLSCREEN_BORDERLESS,
        DEFAULT
    };
    
    enum class Vsync {
        OFF,
        ON,
        DEFAULT
    };
    
    struct OptionalProperties {
        std::optional<std::string> title;
        std::optional<Mode> mode;
        std::optional<bool> resizable;
        std::optional<Vsync> vsync;
        OptionalExtent extent;
    };
    
    struct Properties {
        std::string title;
        Mode mode = Mode::DEFAULT;
        bool resizable = true;
        Vsync vsync = Vsync::DEFAULT;
        Extent extent = {1280, 720};
    };
    
    /**
     * @brief Constructs a Window
     * @param properties The preferred configuration of the window
     */
    explicit Window(Properties properties);
    
    virtual ~Window() = default;
    
    /**
     * @brief Gets a handle_ from the platform's Vulkan surface
     * @param instance A Vulkan instance
     * @returns A VkSurfaceKHR handle_, for use by the application
     */
    virtual VkSurfaceKHR create_surface(Instance &instance) = 0;
    
    /**
     * @brief Gets a handle_ from the platform's Vulkan surface
     * @param instance A Vulkan instance
     * @param physical_device A Vulkan PhysicalDevice
     * @returns A VkSurfaceKHR handle_, for use by the application
     */
    virtual VkSurfaceKHR create_surface(VkInstance instance, VkPhysicalDevice physical_device) = 0;
    
    /**
     * @brief Checks if the window should be closed
     */
    virtual bool should_close() = 0;
    
    /**
     * @brief Handles the processing of all underlying window events
     */
    virtual void process_events();
    
    /**
     * @brief Requests to close the window
     */
    virtual void close() = 0;
    
    /**
     * @return The dot-per-inch scale factor
     */
    [[nodiscard]] virtual float get_dpi_factor() const = 0;
    
    /**
     * @return The scale factor for systems with heterogeneous window and pixel coordinates
     */
    [[nodiscard]] virtual float get_content_scale_factor() const;
    
    /**
     * @brief Attempt to resize the window - not guaranteed to change
     *
     * @param extent The preferred window extent
     * @return Extent The new window extent
     */
    Extent resize(const Extent &extent);
    
    [[nodiscard]] const Extent &get_extent() const;
    
    [[nodiscard]] Mode get_window_mode() const;
    
protected:
    Properties properties_;
};

}        // namespace vox
