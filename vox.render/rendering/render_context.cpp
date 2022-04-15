//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_context.h"

namespace vox {
VkFormat RenderContext::default_vk_format_ = VK_FORMAT_R8G8B8A8_SRGB;

RenderContext::RenderContext(Device &device, VkSurfaceKHR surface, uint32_t window_width, uint32_t window_height) :
device_{device},
queue_{device.get_suitable_graphics_queue()},
surface_extent_{window_width, window_height} {
    if (surface != VK_NULL_HANDLE) {
        VkSurfaceCapabilitiesKHR surface_properties;
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.get_gpu().get_handle(),
                                                           surface,
                                                           &surface_properties));
        
        if (surface_properties.currentExtent.width == 0xFFFFFFFF) {
            swapchain_ = std::make_unique<Swapchain>(device, surface, surface_extent_);
        } else {
            swapchain_ = std::make_unique<Swapchain>(device, surface);
        }
    }
}

void RenderContext::request_present_mode(const VkPresentModeKHR present_mode) {
    if (swapchain_) {
        auto &properties = swapchain_->get_properties();
        properties.present_mode = present_mode;
    }
}

void RenderContext::request_image_format(const VkFormat format) {
    if (swapchain_) {
        auto &properties = swapchain_->get_properties();
        properties.surface_format.format = format;
    }
}

void RenderContext::prepare(size_t thread_count, const RenderTarget::CreateFunc &create_render_target_func) {
    device_.wait_idle();
    
    if (swapchain_) {
        swapchain_->set_present_mode_priority(present_mode_priority_list_);
        swapchain_->set_surface_format_priority(surface_format_priority_list_);
        swapchain_->create();
        
        surface_extent_ = swapchain_->get_extent();
        
        VkExtent3D extent{surface_extent_.width, surface_extent_.height, 1};
        
        for (auto &image_handle : swapchain_->get_images()) {
            auto swapchain_image = core::Image{
                device_, image_handle,
                extent,
                swapchain_->get_format(),
                swapchain_->get_usage()};
            auto render_target = create_render_target_func(std::move(swapchain_image));
            frames_.emplace_back(std::make_unique<RenderFrame>(device_, std::move(render_target), thread_count));
        }
    } else {
        // Otherwise, create a single RenderFrame
        swapchain_ = nullptr;
        
        auto color_image = core::Image{device_,
            VkExtent3D{surface_extent_.width, surface_extent_.height, 1},
            default_vk_format_,        // We can use any format here that we like
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY};
        
        auto render_target = create_render_target_func(std::move(color_image));
        frames_.emplace_back(std::make_unique<RenderFrame>(device_, std::move(render_target), thread_count));
    }
    
    create_render_target_func_ = create_render_target_func;
    thread_count_ = thread_count;
    prepared_ = true;
}

void
RenderContext::set_present_mode_priority(const std::vector<VkPresentModeKHR> &new_present_mode_priority_list) {
    assert(!new_present_mode_priority_list.empty() && "Priority list must not be empty");
    present_mode_priority_list_ = new_present_mode_priority_list;
}

void RenderContext::set_surface_format_priority(
                                                const std::vector<VkSurfaceFormatKHR> &new_surface_format_priority_list) {
    assert(!new_surface_format_priority_list.empty() && "Priority list must not be empty");
    surface_format_priority_list_ = new_surface_format_priority_list;
}

VkFormat RenderContext::get_format() const {
    VkFormat format = default_vk_format_;
    
    if (swapchain_) {
        format = swapchain_->get_format();
    }
    
    return format;
}

void RenderContext::update_swapchain(const VkExtent2D &extent) {
    if (!swapchain_) {
        LOGW("Can't update the swapchains extent in headless mode, skipping.")
        return;
    }
    
    device_.get_resource_cache().clear_framebuffers();
    
    swapchain_ = std::make_unique<Swapchain>(*swapchain_, extent);
    
    recreate();
}

void RenderContext::update_swapchain(const uint32_t image_count) {
    if (!swapchain_) {
        LOGW("Can't update the swapchains image count in headless mode, skipping.")
        return;
    }
    
    device_.get_resource_cache().clear_framebuffers();
    
    device_.wait_idle();
    
    swapchain_ = std::make_unique<Swapchain>(*swapchain_, image_count);
    
    recreate();
}

void RenderContext::update_swapchain(const std::set<VkImageUsageFlagBits> &image_usage_flags) {
    if (!swapchain_) {
        LOGW("Can't update the swapchains image usage in headless mode, skipping.")
        return;
    }
    
    device_.get_resource_cache().clear_framebuffers();
    
    swapchain_ = std::make_unique<Swapchain>(*swapchain_, image_usage_flags);
    
    recreate();
}

void RenderContext::update_swapchain(const VkExtent2D &extent, const VkSurfaceTransformFlagBitsKHR transform) {
    if (!swapchain_) {
        LOGW("Can't update the swapchains extent and surface transform in headless mode, skipping.")
        return;
    }
    
    device_.get_resource_cache().clear_framebuffers();
    
    auto width = extent.width;
    auto height = extent.height;
    if (transform == VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR ||
        transform == VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
        // Pre-rotation: always use native orientation i.e. if rotated, use width and height of identity transform
        std::swap(width, height);
    }
    
    swapchain_ = std::make_unique<Swapchain>(*swapchain_, VkExtent2D{width, height}, transform);
    
    // Save the preTransform attribute for future rotations
    pre_transform_ = transform;
    
    recreate();
}

void RenderContext::recreate() {
    LOGI("Recreated swapchain")
    
    VkExtent2D swapchain_extent = swapchain_->get_extent();
    VkExtent3D extent{swapchain_extent.width, swapchain_extent.height, 1};
    
    auto frame_it = frames_.begin();
    
    for (auto &image_handle : swapchain_->get_images()) {
        core::Image swapchain_image{device_, image_handle,
            extent,
            swapchain_->get_format(),
            swapchain_->get_usage()};
        
        auto render_target = create_render_target_func_(std::move(swapchain_image));
        
        if (frame_it != frames_.end()) {
            (*frame_it)->update_render_target(std::move(render_target));
        } else {
            // Create a new frame if the new swapchain has more images than current frames
            frames_.emplace_back(std::make_unique<RenderFrame>(device_, std::move(render_target), thread_count_));
        }
        
        ++frame_it;
    }
    
    device_.get_resource_cache().clear_framebuffers();
}

bool RenderContext::handle_surface_changes(bool force_update) {
    if (!swapchain_) {
        LOGW("Can't handle_ surface changes in headless mode, skipping.")
        return false;
    }
    
    VkSurfaceCapabilitiesKHR surface_properties;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_.get_gpu().get_handle(),
                                                       swapchain_->get_surface(),
                                                       &surface_properties));
    
    if (surface_properties.currentExtent.width == 0xFFFFFFFF) {
        return false;
    }
    
    // Only recreate the swapchain if the dimensions have changed;
    // handle_surface_changes() is called on VK_SUBOPTIMAL_KHR,
    // which might not be due to a surface resize
    if (surface_properties.currentExtent.width != surface_extent_.width ||
        surface_properties.currentExtent.height != surface_extent_.height ||
        force_update) {
        // Recreate swapchain
        device_.wait_idle();
        
        update_swapchain(surface_properties.currentExtent, pre_transform_);
        
        surface_extent_ = surface_properties.currentExtent;
        
        return true;
    }
    
    return false;
}

CommandBuffer &RenderContext::begin(CommandBuffer::ResetMode reset_mode) {
    assert(prepared_ && "RenderContext not prepared for rendering, call prepare()");
    
    if (!frame_active_) {
        begin_frame();
    }
    
    if (acquired_semaphore_ == VK_NULL_HANDLE) {
        throw std::runtime_error("Couldn't begin frame");
    }
    
    const auto &queue = device_.get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);
    return get_active_frame().request_command_buffer(queue, reset_mode);
}

void RenderContext::submit(CommandBuffer &command_buffer) {
    submit({&command_buffer});
}

void RenderContext::submit(const std::vector<CommandBuffer *> &command_buffers) {
    assert(frame_active_ && "RenderContext is inactive, cannot submit command buffer. Please call begin()");
    
    VkSemaphore render_semaphore = VK_NULL_HANDLE;
    
    if (swapchain_) {
        assert(acquired_semaphore_ && "We do not have acquired_semaphore, it was probably consumed?\n");
        render_semaphore = submit(queue_, command_buffers, acquired_semaphore_,
                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    } else {
        submit(queue_, command_buffers);
    }
    
    end_frame(render_semaphore);
}

void RenderContext::begin_frame() {
    // Only handle_ surface changes if a swapchain exists
    if (swapchain_) {
        handle_surface_changes();
    }
    
    assert(!frame_active_ && "Frame is still active, please call end_frame");
    
    auto &prev_frame = *frames_.at(active_frame_index_);
    
    // We will use the acquired semaphore in a different frame context,
    // so we need to hold ownership.
    acquired_semaphore_ = prev_frame.request_semaphore_with_ownership();
    
    if (swapchain_) {
        auto result = swapchain_->acquire_next_image(active_frame_index_, acquired_semaphore_, VK_NULL_HANDLE);
        
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
            bool swapchain_updated = handle_surface_changes(result == VK_ERROR_OUT_OF_DATE_KHR);
            
            if (swapchain_updated) {
                result = swapchain_->acquire_next_image(active_frame_index_, acquired_semaphore_, VK_NULL_HANDLE);
            }
        }
        
        if (result != VK_SUCCESS) {
            prev_frame.reset();
            return;
        }
    }
    
    // Now the frame is active again
    frame_active_ = true;
    
    // Wait on all resource to be freed from the previous render to this frame
    wait_frame();
}

VkSemaphore RenderContext::submit(const Queue &queue, const std::vector<CommandBuffer *> &command_buffers,
                                  VkSemaphore wait_semaphore, VkPipelineStageFlags wait_pipeline_stage) {
    std::vector<VkCommandBuffer> cmd_buf_handles(command_buffers.size(), VK_NULL_HANDLE);
    std::transform(command_buffers.begin(), command_buffers.end(), cmd_buf_handles.begin(),
                   [](const CommandBuffer *cmd_buf) { return cmd_buf->get_handle(); });
    
    RenderFrame &frame = get_active_frame();
    
    VkSemaphore signal_semaphore = frame.request_semaphore();
    
    VkSubmitInfo submit_info{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    
    submit_info.commandBufferCount = to_u32(cmd_buf_handles.size());
    submit_info.pCommandBuffers = cmd_buf_handles.data();
    
    if (wait_semaphore != VK_NULL_HANDLE) {
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &wait_semaphore;
        submit_info.pWaitDstStageMask = &wait_pipeline_stage;
    }
    
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &signal_semaphore;
    
    VkFence fence = frame.request_fence();
    
    queue.submit({submit_info}, fence);
    
    return signal_semaphore;
}

void RenderContext::submit(const Queue &queue, const std::vector<CommandBuffer *> &command_buffers) {
    std::vector<VkCommandBuffer> cmd_buf_handles(command_buffers.size(), VK_NULL_HANDLE);
    std::transform(command_buffers.begin(), command_buffers.end(), cmd_buf_handles.begin(),
                   [](const CommandBuffer *cmd_buf) { return cmd_buf->get_handle(); });
    
    RenderFrame &frame = get_active_frame();
    
    VkSubmitInfo submit_info{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    
    submit_info.commandBufferCount = to_u32(cmd_buf_handles.size());
    submit_info.pCommandBuffers = cmd_buf_handles.data();
    
    VkFence fence = frame.request_fence();
    
    queue.submit({submit_info}, fence);
}

void RenderContext::wait_frame() {
    RenderFrame &frame = get_active_frame();
    frame.reset();
}

void RenderContext::end_frame(VkSemaphore semaphore) {
    assert(frame_active_ && "Frame is not active, please call begin_frame");
    
    if (swapchain_) {
        VkSwapchainKHR vk_swapchain = swapchain_->get_handle();
        
        VkPresentInfoKHR present_info{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &semaphore;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &vk_swapchain;
        present_info.pImageIndices = &active_frame_index_;
        
        VkResult result = queue_.present(present_info);
        
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
            handle_surface_changes();
        }
    }
    
    // Frame is not active anymore
    if (acquired_semaphore_) {
        release_owned_semaphore(acquired_semaphore_);
        acquired_semaphore_ = VK_NULL_HANDLE;
    }
    frame_active_ = false;
}

VkSemaphore RenderContext::consume_acquired_semaphore() {
    assert(frame_active_ && "Frame is not active, please call begin_frame");
    auto sem = acquired_semaphore_;
    acquired_semaphore_ = VK_NULL_HANDLE;
    return sem;
}

RenderFrame &RenderContext::get_active_frame() {
    assert(frame_active_ && "Frame is not active, please call begin_frame");
    return *frames_.at(active_frame_index_);
}

uint32_t RenderContext::get_active_frame_index() {
    assert(frame_active_ && "Frame is not active, please call begin_frame");
    return active_frame_index_;
}

RenderFrame &RenderContext::get_last_rendered_frame() {
    assert(!frame_active_ && "Frame is still active, please call end_frame");
    return *frames_.at(active_frame_index_);
}

VkSemaphore RenderContext::request_semaphore() {
    RenderFrame &frame = get_active_frame();
    return frame.request_semaphore();
}

VkSemaphore RenderContext::request_semaphore_with_ownership() {
    RenderFrame &frame = get_active_frame();
    return frame.request_semaphore_with_ownership();
}

void RenderContext::release_owned_semaphore(VkSemaphore semaphore) {
    RenderFrame &frame = get_active_frame();
    frame.release_owned_semaphore(semaphore);
}

Device &RenderContext::get_device() {
    return device_;
}

void RenderContext::recreate_swapchain() {
    device_.wait_idle();
    device_.get_resource_cache().clear_framebuffers();
    
    VkExtent2D swapchain_extent = swapchain_->get_extent();
    VkExtent3D extent{swapchain_extent.width, swapchain_extent.height, 1};
    
    auto frame_it = frames_.begin();
    
    for (auto &image_handle : swapchain_->get_images()) {
        core::Image swapchain_image{device_, image_handle,
            extent,
            swapchain_->get_format(),
            swapchain_->get_usage()};
        
        auto render_target = create_render_target_func_(std::move(swapchain_image));
        (*frame_it)->update_render_target(std::move(render_target));
        
        ++frame_it;
    }
}

bool RenderContext::has_swapchain() {
    return swapchain_ != nullptr;
}

Swapchain const &RenderContext::get_swapchain() const {
    assert(swapchain_ && "Swapchain is not valid");
    return *swapchain_;
}

VkExtent2D const &RenderContext::get_surface_extent() const {
    return surface_extent_;
}

uint32_t RenderContext::get_active_frame_index() const {
    return active_frame_index_;
}

std::vector<std::unique_ptr<RenderFrame>> &RenderContext::get_render_frames() {
    return frames_;
}

}        // namespace vox
