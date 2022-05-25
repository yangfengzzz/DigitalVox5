//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/helper.h"
#include "vox.render/core/command_buffer.h"
#include "vox.render/core/command_pool.h"
#include "vox.render/core/descriptor_set.h"
#include "vox.render/core/descriptor_set_layout.h"
#include "vox.render/core/framebuffer.h"
#include "vox.render/core/pipeline.h"
#include "vox.render/core/pipeline_layout.h"
#include "vox.render/core/queue.h"
#include "vox.render/core/render_pass.h"
#include "vox.render/core/swapchain.h"
#include "vox.render/rendering/pipeline_state.h"
#include "vox.render/rendering/render_frame.h"
#include "vox.render/rendering/render_target.h"
#include "vox.render/resource_cache.h"
#include "vox.render/shader/shader_module.h"
#include "vox.render/vk_common.h"

namespace vox {
/**
 * @brief RenderContext acts as a frame manager for the sample, with a lifetime that is the
 * same as that of the Application itself. It acts as a container for RenderFrame objects,
 * swapping between them (BeginFrame, EndFrame) and forwarding requests for Vulkan resources
 * to the active frame. Note that it's guaranteed that there is always an active frame.
 * More than one frame can be in-flight in the GPU, thus the need for per-frame resources.
 *
 * It requires a Device to be valid on creation, and will take control of a given Swapchain.
 *
 * For normal rendering (using a swapchain), the RenderContext can be created by passing in a
 * swapchain. A RenderFrame will then be created for each Swapchain image.
 *
 * For headless rendering (no swapchain), the RenderContext can be given a valid Device, and
 * a width and height. A single RenderFrame will then be created.
 */
class RenderContext {
public:
    // The format to use for the RenderTargets if a swapchain isn't created
    static VkFormat default_vk_format_;

    /**
     * @brief Constructor
     * @param device A valid device_
     * @param surface A surface, VK_NULL_HANDLE if in headless mode
     * @param window_width The width of the window where the surface was created
     * @param window_height The height of the window where the surface was created
     */
    RenderContext(Device &device, VkSurfaceKHR surface, uint32_t window_width, uint32_t window_height);

    RenderContext(const RenderContext &) = delete;

    RenderContext(RenderContext &&) = delete;

    virtual ~RenderContext() = default;

    RenderContext &operator=(const RenderContext &) = delete;

    RenderContext &operator=(RenderContext &&) = delete;

    /**
     * @brief Requests to set the present mode of the swapchain, must be called before prepare
     */
    void RequestPresentMode(VkPresentModeKHR present_mode);

    /**
     * @brief Requests to set a specific image format for the swapchain
     */
    void RequestImageFormat(VkFormat format);

    /**
     * @brief Sets the order in which the swapchain prioritizes selecting its present mode
     */
    void SetPresentModePriority(const std::vector<VkPresentModeKHR> &present_mode_priority_list);

    /**
     * @brief Sets the order in which the swapchain prioritizes selecting its surface format
     */
    void SetSurfaceFormatPriority(const std::vector<VkSurfaceFormatKHR> &surface_format_priority_list);

    /**
     * @brief Prepares the RenderFrames for rendering
     * @param thread_count The number of threads in the application, necessary to allocate this many resource pools for
     * each RenderFrame
     * @param create_render_target_func A function delegate, used to create a RenderTarget
     */
    void Prepare(size_t thread_count = 1,
                 const RenderTarget::CreateFunc &create_render_target_func = RenderTarget::default_create_func_);

    /**
     * @brief Updates the swapchains extent, if a swapchain exists
     * @param extent The width and height of the new swapchain images
     */
    void UpdateSwapchain(const VkExtent2D &extent);

    /**
     * @brief Updates the swapchains image count, if a swapchain exists
     * @param image_count The amount of images in the new swapchain
     */
    void UpdateSwapchain(uint32_t image_count);

    /**
     * @brief Updates the swapchains image usage, if a swapchain exists
     * @param image_usage_flags The usage flags the new swapchain images will have
     */
    void UpdateSwapchain(const std::set<VkImageUsageFlagBits> &image_usage_flags);

    /**
     * @brief Updates the swapchains extent and surface transform, if a swapchain exists
     * @param extent The width and height of the new swapchain images
     * @param transform The surface transform flags
     */
    void UpdateSwapchain(const VkExtent2D &extent, VkSurfaceTransformFlagBitsKHR transform);

    /**
     * @returns True if a valid swapchain exists in the RenderContext
     */
    bool HasSwapchain();

    /**
     * @brief Recreates the RenderFrames, called after every update
     */
    void Recreate();

    /**
     * @brief Recreates the swapchain
     */
    void RecreateSwapchain();

    /**
     * @brief Prepares the next available frame for rendering
     * @param reset_mode How to reset the command buffer
     * @returns A valid command buffer to record commands to be submitted
     * Also ensures that there is an active frame if there is no existing active frame already
     */
    CommandBuffer &Begin(CommandBuffer::ResetMode reset_mode = CommandBuffer::ResetMode::RESET_POOL);

    /**
     * @brief Submits the command buffer to the right queue
     * @param command_buffer A command buffer containing recorded commands
     */
    void Submit(CommandBuffer &command_buffer);

    /**
     * @brief Submits multiple command buffers to the right queue
     * @param command_buffers Command buffers containing recorded commands
     */
    void Submit(const std::vector<CommandBuffer *> &command_buffers);

    /**
     * @brief BeginFrame
     */
    void BeginFrame();

    VkSemaphore Submit(const Queue &queue,
                       const std::vector<CommandBuffer *> &command_buffers,
                       VkSemaphore wait_semaphore,
                       VkPipelineStageFlags wait_pipeline_stage);

    /**
     * @brief Submits a command buffer related to a frame to a queue
     */
    void Submit(const Queue &queue, const std::vector<CommandBuffer *> &command_buffers);

    /**
     * @brief Waits a frame to finish its rendering
     */
    virtual void WaitFrame();

    void EndFrame(VkSemaphore semaphore);

    /**
     * @brief An error should be raised if the frame is not active.
     *        A frame is active after @ref BeginFrame has been called.
     * @return The current active frame
     */
    RenderFrame &GetActiveFrame();

    /**
     * @brief An error should be raised if the frame is not active.
     *        A frame is active after @ref BeginFrame has been called.
     * @return The current active frame index
     */
    [[nodiscard]] uint32_t GetActiveFrameIndex();

    /**
     * @brief An error should be raised if a frame is active.
     *        A frame is active after @ref BeginFrame has been called.
     * @return The previous frame
     */
    RenderFrame &GetLastRenderedFrame();

    VkSemaphore RequestSemaphore();

    VkSemaphore RequestSemaphoreWithOwnership();

    void ReleaseOwnedSemaphore(VkSemaphore semaphore);

    Device &GetDevice();

    /**
     * @brief Returns the format that the RenderTargets are created with within the RenderContext
     */
    [[nodiscard]] VkFormat GetFormat() const;

    [[nodiscard]] Swapchain const &GetSwapchain() const;

    [[nodiscard]] VkExtent2D const &GetSurfaceExtent() const;

    [[nodiscard]] uint32_t GetActiveFrameIndex() const;

    std::vector<std::unique_ptr<RenderFrame>> &GetRenderFrames();

    /**
     * @brief Handles surface changes, only applicable if the render_context makes use of a swapchain
     */
    virtual bool HandleSurfaceChanges(bool force_update = false);

    /**
     * @brief Returns the WSI acquire semaphore. Only to be used in very special circumstances.
     * @return The WSI acquire semaphore.
     */
    VkSemaphore ConsumeAcquiredSemaphore();

protected:
    VkExtent2D surface_extent_;

private:
    Device &device_;

    /// If swapchain exists, then this will be a present supported queue, else a graphics queue
    const Queue &queue_;

    std::unique_ptr<Swapchain> swapchain_;

    SwapchainProperties swapchain_properties_;

    // A list of present modes in order of priority (vector[0] has high priority, vector[size-1] has low priority)
    std::vector<VkPresentModeKHR> present_mode_priority_list_ = {VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_MAILBOX_KHR};

    // A list of surface formats in order of priority (vector[0] has high priority, vector[size-1] has low priority)
    std::vector<VkSurfaceFormatKHR> surface_format_priority_list_ = {
            {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};

    std::vector<std::unique_ptr<RenderFrame>> frames_;

    VkSemaphore acquired_semaphore_{};

    bool prepared_{false};

    /// Current active frame index
    uint32_t active_frame_index_{0};

    /// Whether a frame is active or not
    bool frame_active_{false};

    RenderTarget::CreateFunc create_render_target_func_ = RenderTarget::default_create_func_;

    VkSurfaceTransformFlagBitsKHR pre_transform_{VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR};

    size_t thread_count_{1};
};

}  // namespace vox
