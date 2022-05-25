//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/render_context.h"

namespace vox {
VkFormat RenderContext::default_vk_format_ = VK_FORMAT_R8G8B8A8_SRGB;

RenderContext::RenderContext(Device &device, VkSurfaceKHR surface, uint32_t window_width, uint32_t window_height)
    : device_{device}, queue_{device.GetSuitableGraphicsQueue()}, surface_extent_{window_width, window_height} {
    if (surface != VK_NULL_HANDLE) {
        VkSurfaceCapabilitiesKHR surface_properties;
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetGpu().GetHandle(), surface, &surface_properties));

        if (surface_properties.currentExtent.width == 0xFFFFFFFF) {
            swapchain_ = std::make_unique<Swapchain>(device, surface, surface_extent_);
        } else {
            swapchain_ = std::make_unique<Swapchain>(device, surface);
        }
    }
}

void RenderContext::RequestPresentMode(VkPresentModeKHR present_mode) {
    if (swapchain_) {
        auto &properties = swapchain_->GetProperties();
        properties.present_mode = present_mode;
    }
}

void RenderContext::RequestImageFormat(VkFormat format) {
    if (swapchain_) {
        auto &properties = swapchain_->GetProperties();
        properties.surface_format.format = format;
    }
}

void RenderContext::Prepare(size_t thread_count, const RenderTarget::CreateFunc &create_render_target_func) {
    device_.WaitIdle();

    if (swapchain_) {
        swapchain_->SetPresentModePriority(present_mode_priority_list_);
        swapchain_->SetSurfaceFormatPriority(surface_format_priority_list_);
        swapchain_->Create();

        surface_extent_ = swapchain_->GetExtent();

        VkExtent3D extent{surface_extent_.width, surface_extent_.height, 1};

        for (auto &image_handle : swapchain_->GetImages()) {
            auto swapchain_image =
                    core::Image{device_, image_handle, extent, swapchain_->GetFormat(), swapchain_->GetUsage()};
            auto render_target = create_render_target_func(std::move(swapchain_image));
            frames_.emplace_back(std::make_unique<RenderFrame>(device_, std::move(render_target), thread_count));
        }
    } else {
        // Otherwise, create a single RenderFrame
        swapchain_ = nullptr;

        auto color_image = core::Image{device_, VkExtent3D{surface_extent_.width, surface_extent_.height, 1},
                                       default_vk_format_,  // We can use any format here that we like
                                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                       VMA_MEMORY_USAGE_GPU_ONLY};

        auto render_target = create_render_target_func(std::move(color_image));
        frames_.emplace_back(std::make_unique<RenderFrame>(device_, std::move(render_target), thread_count));
    }

    create_render_target_func_ = create_render_target_func;
    thread_count_ = thread_count;
    prepared_ = true;
}

void RenderContext::SetPresentModePriority(const std::vector<VkPresentModeKHR> &present_mode_priority_list) {
    assert(!present_mode_priority_list.empty() && "Priority list must not be empty");
    present_mode_priority_list_ = present_mode_priority_list;
}

void RenderContext::SetSurfaceFormatPriority(const std::vector<VkSurfaceFormatKHR> &surface_format_priority_list) {
    assert(!surface_format_priority_list.empty() && "Priority list must not be empty");
    surface_format_priority_list_ = surface_format_priority_list;
}

VkFormat RenderContext::GetFormat() const {
    VkFormat format = default_vk_format_;

    if (swapchain_) {
        format = swapchain_->GetFormat();
    }

    return format;
}

void RenderContext::UpdateSwapchain(const VkExtent2D &extent) {
    if (!swapchain_) {
        LOGW("Can't update the swapchains extent in headless mode, skipping.")
        return;
    }

    device_.GetResourceCache().ClearFramebuffers();

    swapchain_ = std::make_unique<Swapchain>(*swapchain_, extent);

    Recreate();
}

void RenderContext::UpdateSwapchain(uint32_t image_count) {
    if (!swapchain_) {
        LOGW("Can't update the swapchains image count in headless mode, skipping.")
        return;
    }

    device_.GetResourceCache().ClearFramebuffers();

    device_.WaitIdle();

    swapchain_ = std::make_unique<Swapchain>(*swapchain_, image_count);

    Recreate();
}

void RenderContext::UpdateSwapchain(const std::set<VkImageUsageFlagBits> &image_usage_flags) {
    if (!swapchain_) {
        LOGW("Can't update the swapchains image usage in headless mode, skipping.")
        return;
    }

    device_.GetResourceCache().ClearFramebuffers();

    swapchain_ = std::make_unique<Swapchain>(*swapchain_, image_usage_flags);

    Recreate();
}

void RenderContext::UpdateSwapchain(const VkExtent2D &extent, VkSurfaceTransformFlagBitsKHR transform) {
    if (!swapchain_) {
        LOGW("Can't update the swapchains extent and surface transform in headless mode, skipping.")
        return;
    }

    device_.GetResourceCache().ClearFramebuffers();

    auto width = extent.width;
    auto height = extent.height;
    if (transform == VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR || transform == VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
        // Pre-rotation: always use native orientation i.e. if rotated, use width and height of identity transform
        std::swap(width, height);
    }

    swapchain_ = std::make_unique<Swapchain>(*swapchain_, VkExtent2D{width, height}, transform);

    // Save the preTransform attribute for future rotations
    pre_transform_ = transform;

    Recreate();
}

void RenderContext::Recreate() {
    LOGI("Recreated swapchain")

    VkExtent2D swapchain_extent = swapchain_->GetExtent();
    VkExtent3D extent{swapchain_extent.width, swapchain_extent.height, 1};

    auto frame_it = frames_.begin();

    for (auto &image_handle : swapchain_->GetImages()) {
        core::Image swapchain_image{device_, image_handle, extent, swapchain_->GetFormat(), swapchain_->GetUsage()};

        auto render_target = create_render_target_func_(std::move(swapchain_image));

        if (frame_it != frames_.end()) {
            (*frame_it)->UpdateRenderTarget(std::move(render_target));
        } else {
            // Create a new frame if the new swapchain has more images than current frames
            frames_.emplace_back(std::make_unique<RenderFrame>(device_, std::move(render_target), thread_count_));
        }

        ++frame_it;
    }

    device_.GetResourceCache().ClearFramebuffers();
}

bool RenderContext::HandleSurfaceChanges(bool force_update) {
    if (!swapchain_) {
        LOGW("Can't handle_ surface changes in headless mode, skipping.")
        return false;
    }

    VkSurfaceCapabilitiesKHR surface_properties;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_.GetGpu().GetHandle(), swapchain_->GetSurface(),
                                                       &surface_properties));

    if (surface_properties.currentExtent.width == 0xFFFFFFFF) {
        return false;
    }

    // Only recreate the swapchain if the dimensions have changed;
    // handle_surface_changes() is called on VK_SUBOPTIMAL_KHR,
    // which might not be due to a surface resize
    if (surface_properties.currentExtent.width != surface_extent_.width ||
        surface_properties.currentExtent.height != surface_extent_.height || force_update) {
        // Recreate swapchain
        device_.WaitIdle();

        UpdateSwapchain(surface_properties.currentExtent, pre_transform_);

        surface_extent_ = surface_properties.currentExtent;

        return true;
    }

    return false;
}

CommandBuffer &RenderContext::Begin(CommandBuffer::ResetMode reset_mode) {
    assert(prepared_ && "RenderContext not prepared for rendering, call prepare()");

    if (!frame_active_) {
        BeginFrame();
    }

    if (acquired_semaphore_ == VK_NULL_HANDLE) {
        throw std::runtime_error("Couldn't begin frame");
    }

    const auto &queue = device_.GetQueueByFlags(VK_QUEUE_GRAPHICS_BIT, 0);
    return GetActiveFrame().RequestCommandBuffer(queue, reset_mode);
}

void RenderContext::Submit(CommandBuffer &command_buffer) { Submit({&command_buffer}); }

void RenderContext::Submit(const std::vector<CommandBuffer *> &command_buffers) {
    assert(frame_active_ && "RenderContext is inactive, cannot submit command buffer. Please call begin()");

    VkSemaphore render_semaphore = VK_NULL_HANDLE;

    if (swapchain_) {
        assert(acquired_semaphore_ && "We do not have acquired_semaphore, it was probably consumed?\n");
        render_semaphore =
                Submit(queue_, command_buffers, acquired_semaphore_, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    } else {
        Submit(queue_, command_buffers);
    }

    EndFrame(render_semaphore);
}

void RenderContext::BeginFrame() {
    // Only handle_ surface changes if a swapchain exists
    if (swapchain_) {
        HandleSurfaceChanges();
    }

    assert(!frame_active_ && "Frame is still active, please call EndFrame");

    auto &prev_frame = *frames_.at(active_frame_index_);

    // We will use the acquired semaphore in a different frame context,
    // so we need to hold ownership.
    acquired_semaphore_ = prev_frame.RequestSemaphoreWithOwnership();

    if (swapchain_) {
        auto result = swapchain_->AcquireNextImage(active_frame_index_, acquired_semaphore_, VK_NULL_HANDLE);

        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
            bool swapchain_updated = HandleSurfaceChanges(result == VK_ERROR_OUT_OF_DATE_KHR);

            if (swapchain_updated) {
                result = swapchain_->AcquireNextImage(active_frame_index_, acquired_semaphore_, VK_NULL_HANDLE);
            }
        }

        if (result != VK_SUCCESS) {
            prev_frame.Reset();
            return;
        }
    }

    // Now the frame is active again
    frame_active_ = true;

    // Wait on all resource to be freed from the previous render to this frame
    WaitFrame();
}

VkSemaphore RenderContext::Submit(const Queue &queue,
                                  const std::vector<CommandBuffer *> &command_buffers,
                                  VkSemaphore wait_semaphore,
                                  VkPipelineStageFlags wait_pipeline_stage) {
    std::vector<VkCommandBuffer> cmd_buf_handles(command_buffers.size(), VK_NULL_HANDLE);
    std::transform(command_buffers.begin(), command_buffers.end(), cmd_buf_handles.begin(),
                   [](const CommandBuffer *cmd_buf) { return cmd_buf->GetHandle(); });

    RenderFrame &frame = GetActiveFrame();

    VkSemaphore signal_semaphore = frame.RequestSemaphore();

    VkSubmitInfo submit_info{VK_STRUCTURE_TYPE_SUBMIT_INFO};

    submit_info.commandBufferCount = utility::ToU32(cmd_buf_handles.size());
    submit_info.pCommandBuffers = cmd_buf_handles.data();

    if (wait_semaphore != VK_NULL_HANDLE) {
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &wait_semaphore;
        submit_info.pWaitDstStageMask = &wait_pipeline_stage;
    }

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &signal_semaphore;

    VkFence fence = frame.RequestFence();

    queue.Submit({submit_info}, fence);

    return signal_semaphore;
}

void RenderContext::Submit(const Queue &queue, const std::vector<CommandBuffer *> &command_buffers) {
    std::vector<VkCommandBuffer> cmd_buf_handles(command_buffers.size(), VK_NULL_HANDLE);
    std::transform(command_buffers.begin(), command_buffers.end(), cmd_buf_handles.begin(),
                   [](const CommandBuffer *cmd_buf) { return cmd_buf->GetHandle(); });

    RenderFrame &frame = GetActiveFrame();

    VkSubmitInfo submit_info{VK_STRUCTURE_TYPE_SUBMIT_INFO};

    submit_info.commandBufferCount = utility::ToU32(cmd_buf_handles.size());
    submit_info.pCommandBuffers = cmd_buf_handles.data();

    VkFence fence = frame.RequestFence();

    queue.Submit({submit_info}, fence);
}

void RenderContext::WaitFrame() {
    RenderFrame &frame = GetActiveFrame();
    frame.Reset();
}

void RenderContext::EndFrame(VkSemaphore semaphore) {
    assert(frame_active_ && "Frame is not active, please call BeginFrame");

    if (swapchain_) {
        VkSwapchainKHR vk_swapchain = swapchain_->GetHandle();

        VkPresentInfoKHR present_info{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &semaphore;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &vk_swapchain;
        present_info.pImageIndices = &active_frame_index_;

        VkResult result = queue_.Present(present_info);

        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
            HandleSurfaceChanges();
        }
    }

    // Frame is not active anymore
    if (acquired_semaphore_) {
        ReleaseOwnedSemaphore(acquired_semaphore_);
        acquired_semaphore_ = VK_NULL_HANDLE;
    }
    frame_active_ = false;
}

VkSemaphore RenderContext::ConsumeAcquiredSemaphore() {
    assert(frame_active_ && "Frame is not active, please call BeginFrame");
    auto sem = acquired_semaphore_;
    acquired_semaphore_ = VK_NULL_HANDLE;
    return sem;
}

RenderFrame &RenderContext::GetActiveFrame() {
    assert(frame_active_ && "Frame is not active, please call BeginFrame");
    return *frames_.at(active_frame_index_);
}

uint32_t RenderContext::GetActiveFrameIndex() {
    assert(frame_active_ && "Frame is not active, please call BeginFrame");
    return active_frame_index_;
}

RenderFrame &RenderContext::GetLastRenderedFrame() {
    assert(!frame_active_ && "Frame is still active, please call EndFrame");
    return *frames_.at(active_frame_index_);
}

VkSemaphore RenderContext::RequestSemaphore() {
    RenderFrame &frame = GetActiveFrame();
    return frame.RequestSemaphore();
}

VkSemaphore RenderContext::RequestSemaphoreWithOwnership() {
    RenderFrame &frame = GetActiveFrame();
    return frame.RequestSemaphoreWithOwnership();
}

void RenderContext::ReleaseOwnedSemaphore(VkSemaphore semaphore) {
    RenderFrame &frame = GetActiveFrame();
    frame.ReleaseOwnedSemaphore(semaphore);
}

Device &RenderContext::GetDevice() { return device_; }

void RenderContext::RecreateSwapchain() {
    device_.WaitIdle();
    device_.GetResourceCache().ClearFramebuffers();

    VkExtent2D swapchain_extent = swapchain_->GetExtent();
    VkExtent3D extent{swapchain_extent.width, swapchain_extent.height, 1};

    auto frame_it = frames_.begin();

    for (auto &image_handle : swapchain_->GetImages()) {
        core::Image swapchain_image{device_, image_handle, extent, swapchain_->GetFormat(), swapchain_->GetUsage()};

        auto render_target = create_render_target_func_(std::move(swapchain_image));
        (*frame_it)->UpdateRenderTarget(std::move(render_target));

        ++frame_it;
    }
}

bool RenderContext::HasSwapchain() { return swapchain_ != nullptr; }

Swapchain const &RenderContext::GetSwapchain() const {
    assert(swapchain_ && "Swapchain is not valid");
    return *swapchain_;
}

VkExtent2D const &RenderContext::GetSurfaceExtent() const { return surface_extent_; }

uint32_t RenderContext::GetActiveFrameIndex() const { return active_frame_index_; }

std::vector<std::unique_ptr<RenderFrame>> &RenderContext::GetRenderFrames() { return frames_; }

}  // namespace vox
