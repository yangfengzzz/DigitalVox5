//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ui_manager.h"
#include "matrix4x4.h"
#include "matrix_utils.h"
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"
#include "platform/filesystem.h"

namespace vox::ui {
namespace {
void upload_draw_data(ImDrawData *draw_data, const uint8_t *vertex_data, const uint8_t *index_data) {
    auto *vtx_dst = (ImDrawVert *)vertex_data;
    auto *idx_dst = (ImDrawIdx *)index_data;
    
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList *cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
    }
}

} // namespace

UiManager::UiManager(GLFWwindow *p_glfw_window,
                     RenderContext *context, Style p_style):
context_(context) {
    ImGui::CreateContext();
    
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly =
    true; /* Disable moving windows by dragging another thing than the title bar */
    enable_docking(false);
    
    apply_style(p_style);
    
    ImGui_ImplGlfw_InitForOpenGL(p_glfw_window, true);
    
    // Create texture sampler
    VkSamplerCreateInfo sampler_info{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    sampler_info.maxAnisotropy = 1.0f;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    
    vox::ShaderSource vert_shader("imgui.vert");
    vox::ShaderSource frag_shader("imgui.frag");
    
    std::vector<vox::ShaderModule *> shader_modules;
    shader_modules.push_back(&context_->get_device().get_resource_cache().request_shader_module(VK_SHADER_STAGE_VERTEX_BIT, vert_shader, {}));
    shader_modules.push_back(&context_->get_device().get_resource_cache().request_shader_module(VK_SHADER_STAGE_FRAGMENT_BIT, frag_shader, {}));
    
    pipeline_layout_ = &context_->get_device().get_resource_cache().request_pipeline_layout(shader_modules);
    
    sampler_ = std::make_unique<core::Sampler>(context_->get_device(), sampler_info);
    sampler_->set_debug_name("GUI sampler");
}

UiManager::~UiManager() {
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UiManager::apply_style(Style p_style) {
    ImGuiStyle *style = &ImGui::GetStyle();
    
    switch (p_style) {
        case Style::IM_CLASSIC_STYLE:ImGui::StyleColorsClassic();
            break;
        case Style::IM_DARK_STYLE:ImGui::StyleColorsDark();
            break;
        case Style::IM_LIGHT_STYLE:ImGui::StyleColorsLight();
            break;
        default:break;
    }
    
    if (p_style == Style::DUNE_DARK) {
        style->WindowPadding = ImVec2(15, 15);
        style->WindowRounding = 5.0f;
        style->FramePadding = ImVec2(5, 5);
        style->FrameRounding = 4.0f;
        style->ItemSpacing = ImVec2(12, 8);
        style->ItemInnerSpacing = ImVec2(8, 6);
        style->IndentSpacing = 25.0f;
        style->ScrollbarSize = 15.0f;
        style->ScrollbarRounding = 9.0f;
        style->GrabMinSize = 5.0f;
        style->GrabRounding = 3.0f;
        
        style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
        style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 0.88f);
        style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
        style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.3f, 0.3f, 0.3f, 0.75f);
        style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
        style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
        
        style->Colors[ImGuiCol_Tab] = style->Colors[ImGuiCol_TabUnfocused];
    } else if (p_style == Style::ALTERNATIVE_DARK) {
        style->WindowPadding = ImVec2(15, 15);
        style->WindowRounding = 0.0f;
        style->FramePadding = ImVec2(5, 5);
        style->FrameRounding = 0.0f;
        style->ItemSpacing = ImVec2(12, 8);
        style->ItemInnerSpacing = ImVec2(8, 6);
        style->IndentSpacing = 25.0f;
        style->ScrollbarSize = 15.0f;
        style->ScrollbarRounding = 0.0f;
        style->GrabMinSize = 5.0f;
        style->GrabRounding = 0.0f;
        style->TabRounding = 0.0f;
        style->ChildRounding = 0.0f;
        style->PopupRounding = 0.0f;
        
        style->WindowBorderSize = 1.0f;
        style->FrameBorderSize = 0.0f;
        style->PopupBorderSize = 1.0f;
        
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

bool UiManager::load_font(const std::string &p_id, const std::string &p_path, float p_font_size) {
    if (fonts_.find(p_id) == fonts_.end()) {
        auto &io = ImGui::GetIO();
        ImFont *font_instance = io.Fonts->AddFontFromFileTTF((fs::path::get(fs::path::Type::ASSETS) + p_path).c_str(), p_font_size);
        
        if (font_instance) {
            fonts_[p_id] = font_instance;
            return true;
        }
    }
    
    return false;
}

bool UiManager::unload_font(const std::string &p_id) {
    if (fonts_.find(p_id) != fonts_.end()) {
        fonts_.erase(p_id);
        return true;
    }
    
    return false;
}

bool UiManager::use_font(const std::string &p_id) {
    auto found_font = fonts_.find(p_id);
    
    if (found_font != fonts_.end()) {
        ImGui::GetIO().FontDefault = found_font->second;
		update_font_texture();
        return true;
    }
    
    return false;
}

void UiManager::use_default_font() {
    ImGui::GetIO().FontDefault = nullptr;
	update_font_texture();
}

void UiManager::enable_editor_layout_save(bool p_value) {
    if (p_value)
        ImGui::GetIO().IniFilename = layout_save_filename_.c_str();
    else
        ImGui::GetIO().IniFilename = nullptr;
}

bool UiManager::is_editor_layout_save_enabled() {
    return ImGui::GetIO().IniFilename != nullptr;
}

void UiManager::set_editor_layout_save_filename(const std::string &p_filename) {
    layout_save_filename_ = p_filename;
    if (is_editor_layout_save_enabled())
        ImGui::GetIO().IniFilename = layout_save_filename_.c_str();
}

void UiManager::set_editor_layout_autosave_frequency(float p_frequency) {
    ImGui::GetIO().IniSavingRate = p_frequency;
}

float UiManager::editor_layout_autosave_frequency(float p_frequeny) {
    return ImGui::GetIO().IniSavingRate;
}

void UiManager::enable_docking(bool p_value) {
    docking_state_ = p_value;
    
    if (p_value)
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    else
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
}

void UiManager::reset_layout(const std::string &p_config) {
    ImGui::LoadIniSettingsFromDisk(p_config.c_str());
}

bool UiManager::is_docking_enabled() const {
    return docking_state_;
}

void UiManager::set_canvas(Canvas &p_canvas) {
    remove_canvas();
    
    current_canvas_ = &p_canvas;
}

void UiManager::remove_canvas() {
    current_canvas_ = nullptr;
}

void UiManager::push_current_font() {
    
}

void UiManager::pop_current_font() {
    
}

//MARK: - Vulkan IMGUI Bindings
void UiManager::update_font_texture() {
    // Create font texture
    unsigned char *font_data;
    int tex_width, tex_height;
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->GetTexDataAsRGBA32(&font_data, &tex_width, &tex_height);
    size_t upload_size = tex_width * tex_height * 4 * sizeof(char);
    
    auto &device = context_->get_device();
    
    // Create target image for copy
    VkExtent3D font_extent{to_u32(tex_width), to_u32(tex_height), 1u};
    
    font_image_ = std::make_unique<core::Image>(device, font_extent, VK_FORMAT_R8G8B8A8_UNORM,
                                                VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                                VMA_MEMORY_USAGE_GPU_ONLY);
    font_image_->set_debug_name("GUI font image");
    
    font_image_view_ = std::make_unique<core::ImageView>(*font_image_, VK_IMAGE_VIEW_TYPE_2D);
    font_image_view_->set_debug_name("View on GUI font image");
    
    // Upload font data into the vulkan image memory
    {
        core::Buffer stage_buffer{device, upload_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, 0};
        stage_buffer.update({font_data, font_data + upload_size});
        
        auto &command_buffer = device.request_command_buffer();
        
        FencePool fence_pool{device};
        
        // Begin recording
        command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);
        
        {
            // Prepare for transfer
            ImageMemoryBarrier memory_barrier{};
            memory_barrier.old_layout = VK_IMAGE_LAYOUT_UNDEFINED;
            memory_barrier.new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            memory_barrier.src_access_mask = 0;
            memory_barrier.dst_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
            memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_HOST_BIT;
            memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            
            command_buffer.image_memory_barrier(*font_image_view_, memory_barrier);
        }
        
        // Copy
        VkBufferImageCopy buffer_copy_region{};
        buffer_copy_region.imageSubresource.layerCount = font_image_view_->get_subresource_range().layerCount;
        buffer_copy_region.imageSubresource.aspectMask = font_image_view_->get_subresource_range().aspectMask;
        buffer_copy_region.imageExtent = font_image_->get_extent();
        
        command_buffer.copy_buffer_to_image(stage_buffer, *font_image_, {buffer_copy_region});
        
        {
            // Prepare for fragmen shader
            ImageMemoryBarrier memory_barrier{};
            memory_barrier.old_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            memory_barrier.new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            memory_barrier.src_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
            memory_barrier.dst_access_mask = VK_ACCESS_SHADER_READ_BIT;
            memory_barrier.src_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            memory_barrier.dst_stage_mask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            
            command_buffer.image_memory_barrier(*font_image_view_, memory_barrier);
        }
        
        // End recording
        command_buffer.end();
        
        auto &queue = device.get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);
        
        queue.submit(command_buffer, device.request_fence());
        
        // Wait for the command buffer to finish its work before destroying the staging buffer
        device.get_fence_pool().wait();
        device.get_fence_pool().reset();
        device.get_command_pool().reset_pool();
    }
}

void UiManager::update(const float delta_time) {
    // Update imGui
    ImGuiIO &io = ImGui::GetIO();
    auto extent = context_->get_surface_extent();
    resize(extent.width, extent.height);
    io.DeltaTime = delta_time;
}

void UiManager::resize(const uint32_t width, const uint32_t height) {
    auto &io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<float>(width);
    io.DisplaySize.y = static_cast<float>(height);
}

void UiManager::draw(CommandBuffer &command_buffer) {
    if (current_canvas_) {
        current_canvas_->draw();
        ScopedDebugLabel debug_label{command_buffer, "GUI"};
        
        // Vertex input state
        VkVertexInputBindingDescription vertex_input_binding{};
        vertex_input_binding.stride = to_u32(sizeof(ImDrawVert));
        
        // Location 0: Position
        VkVertexInputAttributeDescription pos_attr{};
        pos_attr.format = VK_FORMAT_R32G32_SFLOAT;
        pos_attr.offset = to_u32(offsetof(ImDrawVert, pos));
        
        // Location 1: UV
        VkVertexInputAttributeDescription uv_attr{};
        uv_attr.location = 1;
        uv_attr.format = VK_FORMAT_R32G32_SFLOAT;
        uv_attr.offset = to_u32(offsetof(ImDrawVert, uv));
        
        // Location 2: Color
        VkVertexInputAttributeDescription col_attr{};
        col_attr.location = 2;
        col_attr.format = VK_FORMAT_R8G8B8A8_UNORM;
        col_attr.offset = to_u32(offsetof(ImDrawVert, col));
        
        VertexInputState vertex_input_state{};
        vertex_input_state.bindings = {vertex_input_binding};
        vertex_input_state.attributes = {pos_attr, uv_attr, col_attr};
        
        command_buffer.set_vertex_input_state(vertex_input_state);
        
        // Blend state
        vox::ColorBlendAttachmentState color_attachment{};
        color_attachment.blend_enable = VK_TRUE;
        color_attachment.color_write_mask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
        color_attachment.src_color_blend_factor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_attachment.dst_color_blend_factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_attachment.src_alpha_blend_factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        
        vox::ColorBlendState blend_state{};
        blend_state.attachments = {color_attachment};
        
        command_buffer.set_color_blend_state(blend_state);
        
        vox::RasterizationState rasterization_state{};
        rasterization_state.cull_mode = VK_CULL_MODE_NONE;
        command_buffer.set_rasterization_state(rasterization_state);
        
        vox::DepthStencilState depth_state{};
        depth_state.depth_test_enable = VK_FALSE;
        depth_state.depth_write_enable = VK_FALSE;
        command_buffer.set_depth_stencil_state(depth_state);
        
        // Bind pipeline layout
        command_buffer.bind_pipeline_layout(*pipeline_layout_);
        
        command_buffer.bind_image(*font_image_view_, *sampler_, 0, 0, 0);
        
        // Pre-rotation
        auto &io = ImGui::GetIO();
        auto push_transform = Matrix4x4F();
        
        if (context_->has_swapchain()) {
            auto transform = context_->get_swapchain().get_transform();
            
            Vector3F rotation_axis = Vector3F(0.0f, 0.0f, 1.0f);
            if (transform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
                push_transform *= makeRotationMatrix(rotation_axis, degreesToRadians(90.0f));
            } else if (transform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
                push_transform *= makeRotationMatrix(rotation_axis, degreesToRadians(270.0f));
            } else if (transform & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
                push_transform *= makeRotationMatrix(rotation_axis, degreesToRadians(180.0f));
            }
        }
        
        // GUI coordinate space to screen space
        push_transform *= makeTranslationMatrix(Point3F(-1.0f, -1.0f, 0.0f));
        push_transform *= makeScaleMatrix(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y, 0.0f);
        
        // Push constants
        command_buffer.push_constants(push_transform);
        
        // If a render context is used, then use the frames buffer pools to allocate GUI vertex/index data from
        update_buffers(command_buffer, context_->get_active_frame());
        
        // Render commands
        ImDrawData *draw_data = ImGui::GetDrawData();
        int32_t vertex_offset = 0;
        uint32_t index_offset = 0;
        
        if (!draw_data || draw_data->CmdListsCount == 0) {
            return;
        }
        
        for (int32_t i = 0; i < draw_data->CmdListsCount; i++) {
            const ImDrawList *cmd_list = draw_data->CmdLists[i];
            for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                const ImDrawCmd *cmd = &cmd_list->CmdBuffer[j];
                VkRect2D scissor_rect;
                scissor_rect.offset.x = std::max(static_cast<int32_t>(cmd->ClipRect.x), 0);
                scissor_rect.offset.y = std::max(static_cast<int32_t>(cmd->ClipRect.y), 0);
                scissor_rect.extent.width = static_cast<uint32_t>(cmd->ClipRect.z - cmd->ClipRect.x);
                scissor_rect.extent.height = static_cast<uint32_t>(cmd->ClipRect.w - cmd->ClipRect.y);
                
                // Adjust for pre-rotation if necessary
                if (context_->has_swapchain()) {
                    auto transform = context_->get_swapchain().get_transform();
                    if (transform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
                        scissor_rect.offset.x = static_cast<uint32_t>(io.DisplaySize.y - cmd->ClipRect.w);
                        scissor_rect.offset.y = static_cast<uint32_t>(cmd->ClipRect.x);
                        scissor_rect.extent.width = static_cast<uint32_t>(cmd->ClipRect.w - cmd->ClipRect.y);
                        scissor_rect.extent.height = static_cast<uint32_t>(cmd->ClipRect.z - cmd->ClipRect.x);
                    } else if (transform & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
                        scissor_rect.offset.x = static_cast<uint32_t>(io.DisplaySize.x - cmd->ClipRect.z);
                        scissor_rect.offset.y = static_cast<uint32_t>(io.DisplaySize.y - cmd->ClipRect.w);
                        scissor_rect.extent.width = static_cast<uint32_t>(cmd->ClipRect.z - cmd->ClipRect.x);
                        scissor_rect.extent.height = static_cast<uint32_t>(cmd->ClipRect.w - cmd->ClipRect.y);
                    } else if (transform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
                        scissor_rect.offset.x = static_cast<uint32_t>(cmd->ClipRect.y);
                        scissor_rect.offset.y = static_cast<uint32_t>(io.DisplaySize.x - cmd->ClipRect.z);
                        scissor_rect.extent.width = static_cast<uint32_t>(cmd->ClipRect.w - cmd->ClipRect.y);
                        scissor_rect.extent.height = static_cast<uint32_t>(cmd->ClipRect.z - cmd->ClipRect.x);
                    }
                }
                
                command_buffer.set_scissor(0, {scissor_rect});
                command_buffer.draw_indexed(cmd->ElemCount, 1, index_offset, vertex_offset, 0);
                index_offset += cmd->ElemCount;
            }
            vertex_offset += cmd_list->VtxBuffer.Size;
        }
    }
}

void UiManager::update_buffers(CommandBuffer &command_buffer, RenderFrame &render_frame) {
    ImDrawData *draw_data = ImGui::GetDrawData();
    
    if (!draw_data) {
        return;
    }
    
    size_t vertex_buffer_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
    size_t index_buffer_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
    
    if ((vertex_buffer_size == 0) || (index_buffer_size == 0)) {
        return;
    }
    
    std::vector<uint8_t> vertex_data(vertex_buffer_size);
    std::vector<uint8_t> index_data(index_buffer_size);
    
    upload_draw_data(draw_data, vertex_data.data(), index_data.data());
    
    auto vertex_allocation = context_->get_active_frame().allocate_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                          vertex_buffer_size);
    
    vertex_allocation.update(vertex_data);
    
    std::vector<std::reference_wrapper<const core::Buffer>> buffers;
    buffers.emplace_back(std::ref(vertex_allocation.get_buffer()));
    
    std::vector<VkDeviceSize> offsets{vertex_allocation.get_offset()};
    
    command_buffer.bind_vertex_buffers(0, buffers, offsets);
    
    auto index_allocation = context_->get_active_frame().allocate_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                                         index_buffer_size);
    
    index_allocation.update(index_data);
    
    command_buffer.bind_index_buffer(index_allocation.get_buffer(), index_allocation.get_offset(),
                                     VK_INDEX_TYPE_UINT16);
}

}
