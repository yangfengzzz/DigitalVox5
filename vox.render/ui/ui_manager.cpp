//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/ui_manager.h"

#include <GLFW/glfw3.h>

#include "vox.math/matrix4x4.h"
#include "vox.render/platform/filesystem.h"
#include "vox.render/ui/imgui_impl_glfw.h"
#include "vox.render/ui/imgui_impl_vulkan.h"
#include "vox.render/vk_initializers.h"

namespace vox {
ui::UiManager *ui::UiManager::GetSingletonPtr() { return ms_singleton; }

ui::UiManager &ui::UiManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

namespace ui {
static ImGui_ImplVulkan_InitInfo info;

UiManager::UiManager(GLFWwindow *glfw_window, RenderContext *render_context, Style style)
    : render_context_(render_context) {
    ImGui::CreateContext();

    /* Disable moving windows by dragging another thing than the title bar */
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    EnableDocking(false);

    ApplyStyle(style);

    SetupRenderPass();
    SetupDescriptorPool();
    info.Instance = render_context->GetDevice().GetGpu().GetInstance().GetHandle();
    info.PhysicalDevice = render_context->GetDevice().GetGpu().GetHandle();
    info.Device = render_context->GetDevice().GetHandle();
    info.Queue = render_context->GetDevice().GetSuitableGraphicsQueue().GetHandle();
    info.QueueFamily = render_context->GetDevice().GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
    info.PipelineCache = VK_NULL_HANDLE;
    info.DescriptorPool = descriptor_pool_;
    info.Subpass = 0;
    info.MinImageCount = 2;
    info.ImageCount = static_cast<uint32_t>(render_context->GetRenderFrames().size());
    info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    info.Allocator = nullptr;
    info.CheckVkResultFn = nullptr;

    ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
    ImGui_ImplVulkan_LoadFunctions(
            [](const char *function_name, void *user_data) {
                auto app = static_cast<UiManager *>(user_data);
                return vkGetInstanceProcAddr(app->render_context_->GetDevice().GetGpu().GetInstance().GetHandle(),
                                             function_name);
            },
            this);
    ImGui_ImplVulkan_Init(&info, render_pass_);
}

UiManager::~UiManager() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UiManager::ApplyStyle(Style style) {
    ImGuiStyle *imgui_style = &ImGui::GetStyle();

    switch (style) {
        case Style::IM_CLASSIC_STYLE:
            ImGui::StyleColorsClassic();
            break;
        case Style::IM_DARK_STYLE:
            ImGui::StyleColorsDark();
            break;
        case Style::IM_LIGHT_STYLE:
            ImGui::StyleColorsLight();
            break;
        default:
            break;
    }

    if (style == Style::DUNE_DARK) {
        imgui_style->WindowPadding = ImVec2(15, 15);
        imgui_style->WindowRounding = 5.0f;
        imgui_style->FramePadding = ImVec2(5, 5);
        imgui_style->FrameRounding = 4.0f;
        imgui_style->ItemSpacing = ImVec2(12, 8);
        imgui_style->ItemInnerSpacing = ImVec2(8, 6);
        imgui_style->IndentSpacing = 25.0f;
        imgui_style->ScrollbarSize = 15.0f;
        imgui_style->ScrollbarRounding = 9.0f;
        imgui_style->GrabMinSize = 5.0f;
        imgui_style->GrabRounding = 3.0f;

        imgui_style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
        imgui_style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        imgui_style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        imgui_style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        imgui_style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        imgui_style->Colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 0.88f);
        imgui_style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
        imgui_style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        imgui_style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        imgui_style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        imgui_style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        imgui_style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.3f, 0.3f, 0.3f, 0.75f);
        imgui_style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        imgui_style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        imgui_style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        imgui_style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        imgui_style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        imgui_style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        imgui_style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        imgui_style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        imgui_style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        imgui_style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        imgui_style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        imgui_style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        imgui_style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        imgui_style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        imgui_style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        imgui_style->Colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        imgui_style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        imgui_style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        imgui_style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        imgui_style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        imgui_style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        imgui_style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        imgui_style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        imgui_style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        imgui_style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        imgui_style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
        imgui_style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

        imgui_style->Colors[ImGuiCol_Tab] = imgui_style->Colors[ImGuiCol_TabUnfocused];
    } else if (style == Style::ALTERNATIVE_DARK) {
        imgui_style->WindowPadding = ImVec2(15, 15);
        imgui_style->WindowRounding = 0.0f;
        imgui_style->FramePadding = ImVec2(5, 5);
        imgui_style->FrameRounding = 0.0f;
        imgui_style->ItemSpacing = ImVec2(12, 8);
        imgui_style->ItemInnerSpacing = ImVec2(8, 6);
        imgui_style->IndentSpacing = 25.0f;
        imgui_style->ScrollbarSize = 15.0f;
        imgui_style->ScrollbarRounding = 0.0f;
        imgui_style->GrabMinSize = 5.0f;
        imgui_style->GrabRounding = 0.0f;
        imgui_style->TabRounding = 0.0f;
        imgui_style->ChildRounding = 0.0f;
        imgui_style->PopupRounding = 0.0f;

        imgui_style->WindowBorderSize = 1.0f;
        imgui_style->FrameBorderSize = 0.0f;
        imgui_style->PopupBorderSize = 1.0f;

        ImVec4 *colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.27f, 0.27f, 0.29f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.32f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.42f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.53f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.44f, 0.44f, 0.47f, 0.59f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.22f, 0.39f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.91f, 0.62f, 0.00f, 0.78f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.91f, 0.62f, 0.00f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }
}

bool UiManager::LoadFont(const std::string &id, const std::string &path, float font_size) {
    if (fonts_.find(id) == fonts_.end()) {
        auto &io = ImGui::GetIO();
        ImFont *font_instance =
                io.Fonts->AddFontFromFileTTF((fs::path::Get(fs::path::Type::ASSETS) + path).c_str(), font_size);

        if (font_instance) {
            fonts_[id] = font_instance;
            return true;
        }
    }

    return false;
}

bool UiManager::UnloadFont(const std::string &id) {
    if (fonts_.find(id) != fonts_.end()) {
        fonts_.erase(id);
        return true;
    }

    return false;
}

bool UiManager::UseFont(const std::string &id) {
    auto found_font = fonts_.find(id);

    if (found_font != fonts_.end()) {
        ImGui::GetIO().FontDefault = found_font->second;
        UpdateFontTexture();
        return true;
    }

    return false;
}

void UiManager::UseDefaultFont() {
    ImGui::GetIO().FontDefault = nullptr;
    UpdateFontTexture();
}

void UiManager::EnableEditorLayoutSave(bool value) {
    if (value)
        ImGui::GetIO().IniFilename = layout_save_filename_.c_str();
    else
        ImGui::GetIO().IniFilename = nullptr;
}

bool UiManager::IsEditorLayoutSaveEnabled() { return ImGui::GetIO().IniFilename != nullptr; }

void UiManager::SetEditorLayoutSaveFilename(const std::string &filename) {
    layout_save_filename_ = filename;
    if (IsEditorLayoutSaveEnabled()) ImGui::GetIO().IniFilename = layout_save_filename_.c_str();
}

void UiManager::SetEditorLayoutAutosaveFrequency(float frequency) { ImGui::GetIO().IniSavingRate = frequency; }

float UiManager::EditorLayoutAutosaveFrequency(float frequency) { return ImGui::GetIO().IniSavingRate; }

void UiManager::EnableDocking(bool value) {
    docking_state_ = value;

    if (value)
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    else
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
}

void UiManager::ResetLayout(const std::string &config) { ImGui::LoadIniSettingsFromDisk(config.c_str()); }

bool UiManager::IsDockingEnabled() const { return docking_state_; }

void UiManager::SetCanvas(Canvas &canvas) {
    RemoveCanvas();

    current_canvas_ = &canvas;
}

void UiManager::RemoveCanvas() { current_canvas_ = nullptr; }

void UiManager::PushCurrentFont() {}

void UiManager::PopCurrentFont() {}

// MARK: - Vulkan IMGUI Bindings
void UiManager::SetupRenderPass() {
    std::array<VkAttachmentDescription, 2> attachments = {};
    // Color attachment
    attachments[0].format = render_context_->GetFormat();
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    // Depth attachment
    attachments[1].format = GetSuitableDepthFormat(render_context_->GetDevice().GetGpu().GetHandle());
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description = {};
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &color_reference;
    subpass_description.pDepthStencilAttachment = &depth_reference;
    subpass_description.inputAttachmentCount = 0;
    subpass_description.pInputAttachments = nullptr;
    subpass_description.preserveAttachmentCount = 0;
    subpass_description.pPreserveAttachments = nullptr;
    subpass_description.pResolveAttachments = nullptr;

    // Subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies{};

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                                   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                                   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    render_pass_create_info.pAttachments = attachments.data();
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;
    render_pass_create_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
    render_pass_create_info.pDependencies = dependencies.data();

    VK_CHECK(vkCreateRenderPass(render_context_->GetDevice().GetHandle(), &render_pass_create_info, nullptr,
                                &render_pass_));
}

void UiManager::SetupDescriptorPool() {
    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    VK_CHECK(vkCreateDescriptorPool(render_context_->GetDevice().GetHandle(), &pool_info, nullptr, &descriptor_pool_));
}

void UiManager::Draw(CommandBuffer &command_buffer) {
    if (current_canvas_) {
        current_canvas_->Draw();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer.GetHandle());
    }
}

void UiManager::UpdateFontTexture() {
    auto &command_buffer = render_context_->Begin();
    command_buffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer.GetHandle());

    command_buffer.End();
    render_context_->Submit(command_buffer);
    render_context_->GetDevice().WaitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

}  // namespace ui
}  // namespace vox
