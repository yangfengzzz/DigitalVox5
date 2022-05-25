//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/graphics_application.h"

#include "vox.render/error.h"

VKBP_DISABLE_WARNINGS()

#include <imgui.h>

VKBP_ENABLE_WARNINGS()

#include "vox.base/helper.h"
#include "vox.base/logging.h"
#include "vox.render/platform/glfw_window.h"
#include "vox.render/platform/platform.h"
#include "vox.render/platform/window.h"
#include "vox.render/rendering/render_context.h"
#include "vox.render/strings.h"
#include "vox.render/utils.h"
#include "vox.render/vk_common.h"

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#include "vox.render/platform/android/android_platform.h"
#endif

namespace vox {
GraphicsApplication::~GraphicsApplication() {
    if (device_) {
        device_->WaitIdle();
    }

    if (render_pipeline_) {
        render_pipeline_.reset();
    }
    stats_.reset();
    gui_.reset();
    render_context_.reset();
    device_.reset();

    if (surface_ != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance_->GetHandle(), surface_, nullptr);
    }

    instance_.reset();
}

void GraphicsApplication::SetRenderPipeline(RenderPipeline &&render_pipeline) {
    render_pipeline_ = std::make_unique<RenderPipeline>(std::move(render_pipeline));
}

RenderPipeline &GraphicsApplication::GetRenderPipeline() {
    assert(render_pipeline_ && "Render pipeline was not created");
    return *render_pipeline_;
}

bool GraphicsApplication::Prepare(Platform &platform) {
    if (!Application::Prepare(platform)) {
        return false;
    }

    LOGI("Initializing DigitalVox")

    bool headless = platform.GetWindow().GetWindowMode() == Window::Mode::HEADLESS;

    VkResult result = volkInitialize();
    if (result) {
        throw VulkanException(result, "Failed to initialize volk.");
    }

    std::unique_ptr<DebugUtils> debug_utils{};

    // Creating the vulkan instance
    AddInstanceExtension(platform.GetSurfaceExtension());

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
                AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                break;
            }
        }
    }
#endif

    CreateInstance();

    if (!instance_) {
        instance_ = std::make_unique<Instance>(GetName(), GetInstanceExtensions(), GetValidationLayers(), headless,
                                               api_version_);
    }

    // Getting a valid vulkan surface from the platform
    surface_ = platform.GetWindow().CreateSurface(*instance_);

    auto &gpu = instance_->GetSuitableGpu(surface_);
    gpu.SetHighPriorityGraphicsQueueEnable(high_priority_graphics_queue_);

    // Request to enable ASTC
    if (gpu.GetFeatures().textureCompressionASTC_LDR) {
        gpu.GetMutableRequestedFeatures().textureCompressionASTC_LDR = VK_TRUE;
    }

    // Request sample required GPU features
    RequestGpuFeatures(gpu);

    // Creating vulkan device, specifying the swapchain extension always
    if (!headless || instance_->IsEnabled(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME)) {
        AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

#ifdef VKB_VULKAN_DEBUG
    if (!debug_utils) {
        uint32_t device_extension_count;
        VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.GetHandle(), nullptr, &device_extension_count, nullptr));

        std::vector<VkExtensionProperties> available_device_extensions(device_extension_count);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.GetHandle(), nullptr, &device_extension_count,
                                                      available_device_extensions.data()));

        for (const auto &it : available_device_extensions) {
            if (strcmp(it.extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME) == 0) {
                LOGI("Vulkan debug utils enabled ({})", VK_EXT_DEBUG_MARKER_EXTENSION_NAME)

                debug_utils = std::make_unique<DebugMarkerExtDebugUtils>();
                AddDeviceExtension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
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

    CreateDevice();  // create_custom_device? better way than override?

    if (!device_) {
        device_ = std::make_unique<vox::Device>(gpu, surface_, std::move(debug_utils), GetDeviceExtensions());
    }

    CreateRenderContext(platform);
    PrepareRenderContext();

    gui_ = std::make_unique<ui::UiManager>(static_cast<GlfwWindow *>(&platform.GetWindow())->Handle(),
                                           render_context_.get());
    stats_ = std::make_unique<vox::Stats>(*render_context_);

    // Start the sample in the first GUI configuration
    configuration_.Reset();

    return true;
}

void GraphicsApplication::CreateDevice() {}

void GraphicsApplication::CreateInstance() {}

void GraphicsApplication::CreateRenderContext(Platform &platform) {
    auto surface_priority_list =
            std::vector<VkSurfaceFormatKHR>{{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
                                            {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
                                            {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
                                            {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};

    render_context_ = platform.CreateRenderContext(*device_, surface_, surface_priority_list);
}

void GraphicsApplication::PrepareRenderContext() { render_context_->Prepare(); }

void GraphicsApplication::UpdateStats(float delta_time) {
    if (stats_) {
        stats_->Update(delta_time);

        static float stats_view_count = 0.0f;
        stats_view_count += delta_time;

        // Reset every STATS_VIEW_RESET_TIME seconds
        if (stats_view_count > stats_view_reset_time_) {
            ResetStatsView();
            stats_view_count = 0.0f;
        }
    }
}

void GraphicsApplication::Update(float delta_time) {
    auto &command_buffer = render_context_->Begin();

    // Collect the performance data for the sample graphs
    UpdateStats(delta_time);

    command_buffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    stats_->BeginSampling(command_buffer);

    Draw(command_buffer, render_context_->GetActiveFrame().GetRenderTarget());

    stats_->EndSampling(command_buffer);
    command_buffer.End();

    render_context_->Submit(command_buffer);

    platform_->OnPostDraw(GetRenderContext());
    device_->WaitIdle();  // sync cpu and gpu
}

void GraphicsApplication::Draw(CommandBuffer &command_buffer, RenderTarget &render_target) {
    auto &views = render_target.GetViews();

    {
        // Image 0 is the swapchain
        ImageMemoryBarrier memory_barrier{};
        memory_barrier.old_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        memory_barrier.new_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        memory_barrier.src_access_mask = 0;
        memory_barrier.dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        command_buffer.ImageMemoryBarrier(views.at(0), memory_barrier);

        // Skip 1 as it is handled later as a depth-stencil attachment
        for (size_t i = 2; i < views.size(); ++i) {
            command_buffer.ImageMemoryBarrier(views.at(i), memory_barrier);
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

        command_buffer.ImageMemoryBarrier(views.at(1), memory_barrier);
    }

    DrawRenderpass(command_buffer, render_target);

    {
        ImageMemoryBarrier memory_barrier{};
        memory_barrier.old_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        memory_barrier.new_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        memory_barrier.src_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        command_buffer.ImageMemoryBarrier(views.at(0), memory_barrier);
    }
}

void GraphicsApplication::DrawRenderpass(CommandBuffer &command_buffer, RenderTarget &render_target) {
    Render(command_buffer, render_target);

    if (gui_) {
        gui_->Draw(command_buffer);
    }

    command_buffer.EndRenderPass();
}

void GraphicsApplication::Render(CommandBuffer &command_buffer, RenderTarget &render_target) {
    if (render_pipeline_) {
        SetViewportAndScissor(command_buffer, render_target.GetExtent());
        render_pipeline_->Draw(command_buffer, render_target);
    }
}

bool GraphicsApplication::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    Application::Resize(win_width, win_height, fb_width, fb_height);

    if (stats_) {
        stats_->Resize(win_width);
    }
    return true;
}

void GraphicsApplication::InputEvent(const vox::InputEvent &input_event) {
    Application::InputEvent(input_event);

    if (input_event.GetSource() == EventSource::KEYBOARD) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
        if (key_event.GetAction() == KeyAction::DOWN &&
            (key_event.GetCode() == KeyCode::PRINT_SCREEN || key_event.GetCode() == KeyCode::F12)) {
            Screenshot(*render_context_, "screenshot-" + GetName());
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

void GraphicsApplication::Finish() {
    Application::Finish();

    if (device_) {
        device_->WaitIdle();
    }
}

Device &GraphicsApplication::GetDevice() { return *device_; }

Configuration &GraphicsApplication::GetConfiguration() { return configuration_; }

void GraphicsApplication::DrawGui() {}

void GraphicsApplication::SetViewportAndScissor(vox::CommandBuffer &command_buffer, const VkExtent2D &extent) {
    VkViewport viewport{};
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    command_buffer.SetViewport(0, {viewport});

    VkRect2D scissor{};
    scissor.extent = extent;
    command_buffer.SetScissor(0, {scissor});
}

VkSurfaceKHR GraphicsApplication::GetSurface() { return surface_; }

RenderContext &GraphicsApplication::GetRenderContext() {
    assert(render_context_ && "Render context is not valid");
    return *render_context_;
}

std::vector<const char *> GraphicsApplication::GetValidationLayers() { return {}; }

std::unordered_map<const char *, bool> GraphicsApplication::GetInstanceExtensions() { return instance_extensions_; }

std::unordered_map<const char *, bool> GraphicsApplication::GetDeviceExtensions() { return device_extensions_; }

void GraphicsApplication::AddDeviceExtension(const char *extension, bool optional) {
    device_extensions_[extension] = optional;
}

void GraphicsApplication::AddInstanceExtension(const char *extension, bool optional) {
    instance_extensions_[extension] = optional;
}

void GraphicsApplication::SetApiVersion(uint32_t requested_api_version) { api_version_ = requested_api_version; }

void GraphicsApplication::RequestGpuFeatures(PhysicalDevice &gpu) {
    // To be overridden by sample
}

}  // namespace vox
