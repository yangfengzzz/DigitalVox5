//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/core/instance.h"
#include "vox.render/error.h"
#include "vox.render/platform/application.h"
#include "vox.render/rendering/render_context.h"
#include "vox.render/rendering/render_pipeline.h"
#include "vox.render/stats/stats.h"
#include "vox.render/ui/ui_manager.h"
#include "vox.render/utils.h"
#include "vox.render/vk_common.h"

namespace vox {
/**
 * @mainpage Overview of the framework
 *
 * @section initialization Initialization
 *
 * @subsection platform_init Platform initialization
 * The lifecycle of a Vulkan sample starts by instantiating the correct Platform
 * (e.g. WindowsPlatform) and then calling initialize() on it, which sets up
 * the windowing system and logging. Then it calls the parent Platform::initialize(),
 * which takes ownership of the active application. It's the platforms responsibility
 * to then call GraphicsApplication::prepare() to prepare the vulkan sample when it is ready.
 *
 * @subsection sample_init Sample initialization
 * The preparation step is divided in two steps, one in GraphicsApplication and the other in the
 * specific sample, such as SurfaceRotation.
 * GraphicsApplication::prepare() contains functions that do not require customization,
 * including creating a Vulkan instance, the surface and getting physical devices.
 * The prepare() function for the specific sample completes the initialization, including:
 * - setting enabled Stats
 * - creating the Device
 * - creating the Swapchain
 * - creating the RenderContext (or child class)
 * - preparing the RenderContext
 * - loading the sg::Scene
 * - creating the RenderPipeline with ShaderModule (s)
 * - creating the sg::Camera
 * - creating the Gui
 *
 * @section frame_rendering Frame rendering
 *
 * @subsection update Update function
 * Rendering happens in the update() function. Each sample can override it, e.g.
 * to recreate the Swapchain in SwapchainImages when required by user input.
 * Typically a sample will then call GraphicsApplication::update().
 *
 * @subsection rendering Rendering
 * A series of steps are performed, some of which can be customized (it will be
 * highlighted when that's the case):
 *
 * - calling sg::Script::update() for all sg::Script (s)
 * - beginning a frame in RenderContext (does the necessary waiting on fences and
 *   acquires an core::Image)
 * - requesting a CommandBuffer
 * - updating Stats and Gui
 * - getting an active RenderTarget constructed by the factory function of the RenderFrame
 * - setting up barriers for color and depth, note that these are only for the default RenderTarget
 * - calling GraphicsApplication::draw_swapchain_renderpass (see below)
 * - setting up a barrier for the Swapchain transition to present
 * - submitting the CommandBuffer and end the Frame (present)
 *
 * @subsection draw_swapchain Draw swapchain renderpass
 * The function starts and ends a RenderPass which includes setting up viewport, scissors,
 * blend state (etc.) and calling draw_scene.
 * Note that RenderPipeline::draw is not virtual in RenderPipeline, but internally it calls
 * Subpass::draw for each Subpass, which is virtual and can be customized.
 *
 * @section framework_classes Main framework classes
 *
 * - RenderContext
 * - RenderFrame
 * - RenderTarget
 * - RenderPipeline
 * - ShaderModule
 * - ResourceCache
 * - BufferPool
 * - Core classes: Classes in vox::core wrap Vulkan objects for indexing and hashing.
 */

class GraphicsApplication : public Application {
public:
    GraphicsApplication() = default;

    ~GraphicsApplication() override;

    /**
     * @brief Additional sample initialization
     */
    bool Prepare(Platform &platform) override;

    /**
     * @brief Create the Vulkan device used by this sample
     * @note Can be overridden to implement custom device creation
     */
    virtual void CreateDevice();

    /**
     * @brief Create the Vulkan instance used by this sample
     * @note Can be overridden to implement custom instance creation
     */
    virtual void CreateInstance();

    /**
     * @brief Main loop sample events
     */
    void Update(float delta_time) override;

    bool Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) override;

    void InputEvent(const vox::InputEvent &input_event) override;

    void Finish() override;

    VkSurfaceKHR GetSurface();

    Device &GetDevice();

    RenderContext &GetRenderContext();

    void SetRenderPipeline(RenderPipeline &&render_pipeline);

    RenderPipeline &GetRenderPipeline();

    Configuration &GetConfiguration();

protected:
    /**
     * @brief The Vulkan instance
     */
    std::unique_ptr<Instance> instance_{nullptr};

    /**
     * @brief The Vulkan device
     */
    std::unique_ptr<Device> device_{nullptr};

    /**
     * @brief Context used for rendering, it is responsible for managing the frames and their underlying images
     */
    std::unique_ptr<RenderContext> render_context_{nullptr};

    /**
     * @brief Pipeline used for rendering, it should be set up by the concrete sample
     */
    std::unique_ptr<RenderPipeline> render_pipeline_{nullptr};

    std::unique_ptr<ui::UiManager> gui_{nullptr};

    std::unique_ptr<Stats> stats_{nullptr};

    /**
     * @brief Update counter values
     * @param delta_time delta_time
     */
    void UpdateStats(float delta_time);

    /**
     * @brief Prepares the render target and draws to it, calling DrawRenderpass
     * @param command_buffer The command buffer to record the commands to
     * @param render_target The render target that is being drawn to
     */
    virtual void Draw(CommandBuffer &command_buffer, RenderTarget &render_target);

    /**
     * @brief Starts the render pass, executes the render pipeline, and then ends the render pass
     * @param command_buffer The command buffer to record the commands to
     * @param render_target The render target that is being drawn to
     */
    virtual void DrawRenderpass(CommandBuffer &command_buffer, RenderTarget &render_target);

    /**
     * @brief Triggers the render pipeline, it can be overridden by samples to specialize their rendering logic
     * @param command_buffer The command buffer to record the commands to
     */
    virtual void Render(CommandBuffer &command_buffer, RenderTarget &render_target);

    /**
     * @brief Get additional sample-specific instance layers.
     *
     * @return Vector of additional instance layers. Default is empty vector.
     */
    virtual std::vector<const char *> GetValidationLayers();

    /**
     * @brief Get sample-specific instance extensions.
     *
     * @return Map of instance extensions and whether or not they are optional. Default is empty map.
     */
    std::unordered_map<const char *, bool> GetInstanceExtensions();

    /**
     * @brief Get sample-specific device extensions.
     *
     * @return Map of device extensions and whether or not they are optional. Default is empty map.
     */
    std::unordered_map<const char *, bool> GetDeviceExtensions();

    /**
     * @brief Add a sample-specific device extension
     * @param extension The extension name
     * @param optional (Optional) Whether the extension is optional
     */
    void AddDeviceExtension(const char *extension, bool optional = false);

    /**
     * @brief Add a sample-specific instance extension
     * @param extension The extension name
     * @param optional (Optional) Whether the extension is optional
     */
    void AddInstanceExtension(const char *extension, bool optional = false);

    /**
     * @brief Set the Vulkan API version to request at instance creation time
     */
    void SetApiVersion(uint32_t requested_api_version);

    /**
     * @brief Request features from the gpu based on what is supported
     */
    virtual void RequestGpuFeatures(PhysicalDevice &gpu);

    /**
     * @brief Override this to customise the creation of the render_context
     */
    virtual void CreateRenderContext(Platform &platform);

    /**
     * @brief Override this to customise the creation of the swapchain and render_context
     */
    virtual void PrepareRenderContext();

    /**
     * @brief Resets the stats view max values for high demanding configs
     *        Should be overridden by the samples since they
     *        know which configuration is resource demanding
     */
    virtual void ResetStatsView(){};

    /**
     * @brief Samples should override this function to draw their interface
     */
    virtual void DrawGui();

    /**
     * @brief Set viewport and scissor state in command buffer for a given extent
     */
    static void SetViewportAndScissor(vox::CommandBuffer &command_buffer, const VkExtent2D &extent);

    static constexpr float stats_view_reset_time_{10.0f};  // 10 seconds

    /**
     * @brief The Vulkan surface
     */
    VkSurfaceKHR surface_{VK_NULL_HANDLE};

    /**
     * @brief The configuration of the sample
     */
    Configuration configuration_{};

    /**
     * @brief Sets whether or not the first graphics queue should have higher priority than other queues.
     * Very specific feature which is used by async compute samples.
     * Needs to be called before prepare().
     * @param enable If true, present queue will have prio 1.0 and other queues have prio 0.5.
     * Default state is false, where all queues have 0.5 priority.
     */
    void SetHighPriorityGraphicsQueueEnable(bool enable) { high_priority_graphics_queue_ = enable; }

private:
    /** @brief Set of device extensions to be enabled for this example and whether they are optional (must be set in the
     * derived constructor) */
    std::unordered_map<const char *, bool> device_extensions_;

    /** @brief Set of instance extensions to be enabled for this example and whether they are optional (must be set in
     * the derived constructor) */
    std::unordered_map<const char *, bool> instance_extensions_;

    /** @brief The Vulkan API version to request for this sample at instance creation time */
    uint32_t api_version_ = VK_API_VERSION_1_0;

    /** @brief Whether or not we want a high priority graphics queue. */
    bool high_priority_graphics_queue_{false};
};

}  // namespace vox
