//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "graphics_application.h"

#include "error.h"

VKBP_DISABLE_WARNINGS()

#include <imgui.h>

VKBP_ENABLE_WARNINGS()

#include "helpers.h"
#include "logging.h"
#include "strings.h"
#include "utils.h"
#include "vk_common.h"
#include "platform/platform.h"
#include "platform/window.h"
#include "rendering/render_context.h"
#include "platform/glfw_window.h"

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#	include "platform/android/android_platform.h"
#endif

namespace vox {
GraphicsApplication::~GraphicsApplication() {
    if (device_) {
        device_->wait_idle();
    }
        
    if (render_pipeline_) {
        render_pipeline_.reset();
    }
    stats_.reset();
    gui_.reset();
    render_context_.reset();
    device_.reset();
    
    if (surface_ != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance_->get_handle(), surface_, nullptr);
    }
    
    instance_.reset();
}

void GraphicsApplication::set_render_pipeline(RenderPipeline &&rp) {
    render_pipeline_ = std::make_unique<RenderPipeline>(std::move(rp));
}

RenderPipeline &GraphicsApplication::get_render_pipeline() {
    assert(render_pipeline_ && "Render pipeline was not created");
    return *render_pipeline_;
}

bool GraphicsApplication::prepare(Platform &platform) {
    if (!Application::prepare(platform)) {
        return false;
    }
    
    LOGI("Initializing DigitalVox")
    
    bool headless = platform.get_window().get_window_mode() == Window::Mode::HEADLESS;
    
    VkResult result = volkInitialize();
    if (result) {
        throw VulkanException(result, "Failed to initialize volk.");
    }
    
    std::unique_ptr<DebugUtils> debug_utils{};
    
    // Creating the vulkan instance
    add_instance_extension(platform.get_surface_extension());
    
#ifdef VKB_VULKAN_DEBUG
    {
        uint32_t instance_extension_count;
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr));
        
        std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count,
                                                        available_instance_extensions.data()));
        
        for (const auto &it : available_instance_extensions) {
            if (strcmp(it.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
                LOGI("Vulkan debug utils enabled ({})", VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
                
                debug_utils = std::make_unique<DebugUtilsExtDebugUtils>();
                add_instance_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                break;
            }
        }
    }
#endif
    
    create_instance();
    
    if (!instance_) {
        instance_ = std::make_unique<Instance>(get_name(), get_instance_extensions(), get_validation_layers(),
                                               headless, api_version_);
    }
    
    // Getting a valid vulkan surface from the platform
    surface_ = platform.get_window().create_surface(*instance_);
    
    auto &gpu = instance_->get_suitable_gpu(surface_);
    gpu.set_high_priority_graphics_queue_enable(high_priority_graphics_queue_);
    
    // Request to enable ASTC
    if (gpu.get_features().textureCompressionASTC_LDR) {
        gpu.get_mutable_requested_features().textureCompressionASTC_LDR = VK_TRUE;
    }
    
    // Request sample required GPU features
    request_gpu_features(gpu);
    
    // Creating vulkan device, specifying the swapchain extension always
    if (!headless || instance_->is_enabled(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME)) {
        add_device_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    
#ifdef VKB_VULKAN_DEBUG
    if (!debug_utils) {
        uint32_t device_extension_count;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.get_handle(), nullptr, &device_extension_count, nullptr));
        
        std::vector<VkExtensionProperties> available_device_extensions(device_extension_count);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.get_handle(), nullptr, &device_extension_count,
                                                      available_device_extensions.data()));
        
        for (const auto &it : available_device_extensions) {
            if (strcmp(it.extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME) == 0) {
                LOGI("Vulkan debug utils enabled ({})", VK_EXT_DEBUG_MARKER_EXTENSION_NAME)
                
                debug_utils = std::make_unique<DebugMarkerExtDebugUtils>();
                add_device_extension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
                break;
            }
        }
    }
    
    if (!debug_utils) {
        LOGW("Vulkan debug utils were requested, but no extension that provides them was found")
    }
#endif
    
    if (!debug_utils) {
        debug_utils = std::make_unique<DummyDebugUtils>();
    }
    
    create_device();        // create_custom_device? better way than override?
    
    if (!device_) {
        device_ = std::make_unique<vox::Device>(gpu, surface_, std::move(debug_utils), get_device_extensions());
    }
    
    create_render_context(platform);
    prepare_render_context();
    
    gui_ = std::make_unique<ui::UiManager>(static_cast<GlfwWindow*>(&platform.get_window())->handle(), render_context_.get());
    stats_ = std::make_unique<vox::Stats>(*render_context_);
    
    // Start the sample in the first GUI configuration
    configuration_.reset();
    
    return true;
}

void GraphicsApplication::create_device() {
}

void GraphicsApplication::create_instance() {
}

void GraphicsApplication::create_render_context(Platform &platform) {
    auto surface_priority_list =
    std::vector<VkSurfaceFormatKHR>{{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
        {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};
    
    render_context_ = platform.create_render_context(*device_, surface_, surface_priority_list);
}

void GraphicsApplication::prepare_render_context() {
    render_context_->prepare();
}

void GraphicsApplication::update_stats(float delta_time) {
    if (stats_) {
        stats_->update(delta_time);
        
        static float stats_view_count = 0.0f;
        stats_view_count += delta_time;
        
        // Reset every STATS_VIEW_RESET_TIME seconds
        if (stats_view_count > stats_view_reset_time_) {
            reset_stats_view();
            stats_view_count = 0.0f;
        }
    }
}

void GraphicsApplication::update_gui(float delta_time) {
    if (gui_) {
        gui_->update(delta_time);
    }
}

void GraphicsApplication::update(float delta_time) {
    update_gui(delta_time);
    
    auto &command_buffer = render_context_->begin();
    
    // Collect the performance data for the sample graphs
    update_stats(delta_time);
    
    command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    stats_->begin_sampling(command_buffer);
    
    draw(command_buffer, render_context_->get_active_frame().get_render_target());
    
    stats_->end_sampling(command_buffer);
    command_buffer.end();
    
    render_context_->submit(command_buffer);
    
    platform_->on_post_draw(get_render_context());
    device_->wait_idle(); // sync cpu and gpu
}

void GraphicsApplication::draw(CommandBuffer &command_buffer, RenderTarget &render_target) {
    auto &views = render_target.get_views();
    
    {
        // Image 0 is the swapchain
        ImageMemoryBarrier memory_barrier{};
        memory_barrier.old_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        memory_barrier.new_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        memory_barrier.src_access_mask = 0;
        memory_barrier.dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        
        command_buffer.image_memory_barrier(views.at(0), memory_barrier);
        
        // Skip 1 as it is handled later as a depth-stencil attachment
        for (size_t i = 2; i < views.size(); ++i) {
            command_buffer.image_memory_barrier(views.at(i), memory_barrier);
        }
    }
    
    {
        ImageMemoryBarrier memory_barrier{};
        memory_barrier.old_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        memory_barrier.new_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        memory_barrier.src_access_mask = 0;
        memory_barrier.dst_access_mask =
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        memory_barrier.dst_stage_mask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        
        command_buffer.image_memory_barrier(views.at(1), memory_barrier);
    }
    
    draw_renderpass(command_buffer, render_target);
    
    {
        ImageMemoryBarrier memory_barrier{};
        memory_barrier.old_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        memory_barrier.new_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        memory_barrier.src_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        
        command_buffer.image_memory_barrier(views.at(0), memory_barrier);
    }
}

void GraphicsApplication::draw_renderpass(CommandBuffer &command_buffer, RenderTarget &render_target) {
    render(command_buffer, render_target);
    
    if (gui_) {
        gui_->draw(command_buffer);
    }
    
    command_buffer.end_render_pass();
}

void GraphicsApplication::render(CommandBuffer &command_buffer, RenderTarget &render_target) {
    if (render_pipeline_) {
        set_viewport_and_scissor(command_buffer, render_target.get_extent());
        render_pipeline_->draw(command_buffer, render_target);
    }
}

bool GraphicsApplication::resize(uint32_t win_width, uint32_t win_height,
                                 uint32_t fb_width, uint32_t fb_height) {
    Application::resize(win_width, win_height, fb_width, fb_height);
    
    if (gui_) {
        gui_->resize(win_width, win_height);
    }
    
    if (stats_) {
        stats_->resize(win_width);
    }
    return true;
}

void GraphicsApplication::input_event(const InputEvent &input_event) {
    Application::input_event(input_event);
    
    if (input_event.get_source() == EventSource::KEYBOARD) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
        if (key_event.get_action() == KeyAction::DOWN &&
            (key_event.get_code() == KeyCode::PRINT_SCREEN || key_event.get_code() == KeyCode::F12)) {
            screenshot(*render_context_, "screenshot-" + get_name());
        }
        
        //		if (key_event.get_code() == KeyCode::F6 && key_event.get_action() == KeyAction::Down)
        //		{
        //			if (!graphs::generate_all(get_render_context(), *scene))
        //			{
        //				LOGE("Failed to save Graphs");
        //			}
        //		}
    }
}

void GraphicsApplication::finish() {
    Application::finish();
    
    if (device_) {
        device_->wait_idle();
    }
}

Device &GraphicsApplication::get_device() {
    return *device_;
}

Configuration &GraphicsApplication::get_configuration() {
    return configuration_;
}

void GraphicsApplication::draw_gui() {
}

void GraphicsApplication::set_viewport_and_scissor(vox::CommandBuffer &command_buffer, const VkExtent2D &extent) {
    VkViewport viewport{};
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    command_buffer.set_viewport(0, {viewport});
    
    VkRect2D scissor{};
    scissor.extent = extent;
    command_buffer.set_scissor(0, {scissor});
}

VkSurfaceKHR GraphicsApplication::get_surface() {
    return surface_;
}

RenderContext &GraphicsApplication::get_render_context() {
    assert(render_context_ && "Render context is not valid");
    return *render_context_;
}

std::vector<const char *> GraphicsApplication::get_validation_layers() {
    return {};
}

std::unordered_map<const char *, bool> GraphicsApplication::get_instance_extensions() {
    return instance_extensions_;
}

std::unordered_map<const char *, bool> GraphicsApplication::get_device_extensions() {
    return device_extensions_;
}

void GraphicsApplication::add_device_extension(const char *extension, bool optional) {
    device_extensions_[extension] = optional;
}

void GraphicsApplication::add_instance_extension(const char *extension, bool optional) {
    instance_extensions_[extension] = optional;
}

void GraphicsApplication::set_api_version(uint32_t requested_api_version) {
    api_version_ = requested_api_version;
}

void GraphicsApplication::request_gpu_features(PhysicalDevice &gpu) {
    // To be overridden by sample
}

}        // namespace vox
