//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui.h"

#include <map>
#include <numeric>
#include "matrix4x4.h"
#include "matrix_utils.h"
#include "error.h"

#include "buffer_pool.h"
#include "logging.h"
#include "vk_common.h"
#include "vk_initializers.h"
#include "core/descriptor_set.h"
#include "core/pipeline.h"
#include "shader/shader_module.h"
#include "imgui_internal.h"
#include "platform/filesystem.h"
#include "platform/window.h"
#include "rendering/render_context.h"
#include "timer.h"
#include "graphics_application.h"

namespace vox {
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

}        // namespace

const double Gui::press_time_ms_ = 200.0f;

const std::string Gui::default_font_ = "Roboto-Regular";

Gui::Gui(GraphicsApplication &sample, const Window &window, const Stats *stats,
		 const float font_size, bool explicit_update) :
sample_{sample},
content_scale_factor_{window.get_content_scale_factor()},
dpi_factor_{window.get_dpi_factor() * content_scale_factor_},
explicit_update_{explicit_update} {
    ImGui::CreateContext();
    
    ImGuiStyle &style = ImGui::GetStyle();
    
    // Color scheme
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    
    // Borderless window
    style.WindowBorderSize = 0.0f;
    
    // Global scale
    style.ScaleAllSizes(dpi_factor_);
    
    // Dimensions
    ImGuiIO &io = ImGui::GetIO();
    auto extent = sample.get_render_context().get_surface_extent();
    io.DisplaySize.x = static_cast<float>(extent.width);
    io.DisplaySize.y = static_cast<float>(extent.height);
    io.FontGlobalScale = 1.0f;
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    
    // Enable keyboard navigation
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.KeyMap[ImGuiKey_Space] = static_cast<int>(KeyCode::SPACE);
    io.KeyMap[ImGuiKey_Enter] = static_cast<int>(KeyCode::ENTER);
    io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int>(KeyCode::LEFT);
    io.KeyMap[ImGuiKey_RightArrow] = static_cast<int>(KeyCode::RIGHT);
    io.KeyMap[ImGuiKey_UpArrow] = static_cast<int>(KeyCode::UP);
    io.KeyMap[ImGuiKey_DownArrow] = static_cast<int>(KeyCode::DOWN);
    io.KeyMap[ImGuiKey_Tab] = static_cast<int>(KeyCode::TAB);
    
    // Default font
    fonts_.emplace_back(default_font_, font_size * dpi_factor_);
    
    // Debug window font
    fonts_.emplace_back("RobotoMono-Regular", (font_size / 2) * dpi_factor_);
    
    // Create font texture
    unsigned char *font_data;
    int tex_width, tex_height;
    io.Fonts->GetTexDataAsRGBA32(&font_data, &tex_width, &tex_height);
    size_t upload_size = tex_width * tex_height * 4 * sizeof(char);
    
    auto &device = sample.get_render_context().get_device();
    
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
        core::Buffer stage_buffer{device, upload_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY,
            0};
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
    shader_modules.push_back(
                             &device.get_resource_cache().request_shader_module(VK_SHADER_STAGE_VERTEX_BIT, vert_shader, {}));
    shader_modules.push_back(
                             &device.get_resource_cache().request_shader_module(VK_SHADER_STAGE_FRAGMENT_BIT, frag_shader, {}));
    
    pipeline_layout_ = &device.get_resource_cache().request_pipeline_layout(shader_modules);
    
    sampler_ = std::make_unique<core::Sampler>(device, sampler_info);
    sampler_->set_debug_name("GUI sampler");
    
    if (explicit_update) {
        vertex_buffer_ = std::make_unique<core::Buffer>(sample.get_render_context().get_device(), 1,
                                                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                        VMA_MEMORY_USAGE_GPU_TO_CPU);
        vertex_buffer_->set_debug_name("GUI vertex buffer");
        
        index_buffer_ = std::make_unique<core::Buffer>(sample.get_render_context().get_device(), 1,
                                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                       VMA_MEMORY_USAGE_GPU_TO_CPU);
        index_buffer_->set_debug_name("GUI index buffer");
    }
}

void Gui::update(const float delta_time) {
    // Update imGui
    ImGuiIO &io = ImGui::GetIO();
    auto extent = sample_.get_render_context().get_surface_extent();
    resize(extent.width, extent.height);
    io.DeltaTime = delta_time;
    
    // Render to generate draw buffers
    ImGui::Render();
}

void Gui::update_buffers(CommandBuffer &command_buffer, RenderFrame &render_frame) {
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
    
    auto vertex_allocation = sample_.get_render_context().get_active_frame().allocate_buffer(
                                                                                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                                             vertex_buffer_size);
    
    vertex_allocation.update(vertex_data);
    
    std::vector<std::reference_wrapper<const core::Buffer>> buffers;
    buffers.emplace_back(std::ref(vertex_allocation.get_buffer()));
    
    std::vector<VkDeviceSize> offsets{vertex_allocation.get_offset()};
    
    command_buffer.bind_vertex_buffers(0, buffers, offsets);
    
    auto index_allocation = sample_.get_render_context().get_active_frame().allocate_buffer(
                                                                                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                                                            index_buffer_size);
    
    index_allocation.update(index_data);
    
    command_buffer.bind_index_buffer(index_allocation.get_buffer(), index_allocation.get_offset(),
                                     VK_INDEX_TYPE_UINT16);
}

void Gui::resize(const uint32_t width, const uint32_t height) {
    auto &io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<float>(width);
    io.DisplaySize.y = static_cast<float>(height);
}

void Gui::draw(CommandBuffer &command_buffer) {
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
    
    if (sample_.get_render_context().has_swapchain()) {
        auto transform = sample_.get_render_context().get_swapchain().get_transform();
        
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
    if (!explicit_update_) {
        update_buffers(command_buffer, sample_.get_render_context().get_active_frame());
    } else {
        std::vector<std::reference_wrapper<const vox::core::Buffer>> buffers;
        buffers.emplace_back(*vertex_buffer_);
        command_buffer.bind_vertex_buffers(0, buffers, {0});
        
        command_buffer.bind_index_buffer(*index_buffer_, 0, VK_INDEX_TYPE_UINT16);
    }
    
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
            if (sample_.get_render_context().has_swapchain()) {
                auto transform = sample_.get_render_context().get_swapchain().get_transform();
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

Gui::~Gui() {
    vkDestroyDescriptorPool(sample_.get_render_context().get_device().get_handle(), descriptor_pool_, nullptr);
    vkDestroyDescriptorSetLayout(sample_.get_render_context().get_device().get_handle(), descriptor_set_layout_,
                                 nullptr);
    vkDestroyPipeline(sample_.get_render_context().get_device().get_handle(), pipeline_, nullptr);
    
    ImGui::DestroyContext();
}

Font &Gui::get_font(const std::string &font_name) {
    assert(!fonts_.empty() && "No fonts exist");
    
    auto it = std::find_if(fonts_.begin(), fonts_.end(), [&font_name](Font &font) { return font.name == font_name; });
    
    if (it != fonts_.end()) {
        return *it;
    } else {
        LOGW("Couldn't find font with name {}", font_name)
        return *fonts_.begin();
    }
}

bool Gui::input_event(const InputEvent &input_event) {
    auto &io = ImGui::GetIO();
    auto capture_move_event = false;
    
    if (input_event.get_source() == EventSource::KEYBOARD) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
        
        if (key_event.get_action() == KeyAction::DOWN) {
            io.KeysDown[static_cast<int>(key_event.get_code())] = true;
        } else if (key_event.get_action() == KeyAction::UP) {
            io.KeysDown[static_cast<int>(key_event.get_code())] = false;
        }
    } else if (input_event.get_source() == EventSource::MOUSE) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
        
        io.MousePos = ImVec2{mouse_button.get_pos_x() * content_scale_factor_,
            mouse_button.get_pos_y() * content_scale_factor_};
        
        auto button_id = static_cast<int>(mouse_button.get_button());
        
        if (mouse_button.get_action() == MouseAction::DOWN) {
            io.MouseDown[button_id] = true;
        } else if (mouse_button.get_action() == MouseAction::UP) {
            io.MouseDown[button_id] = false;
        } else if (mouse_button.get_action() == MouseAction::MOVE) {
            capture_move_event = io.WantCaptureMouse;
        }
    } else if (input_event.get_source() == EventSource::TOUCHSCREEN) {
        const auto &touch_event = static_cast<const TouchInputEvent &>(input_event);
        
        io.MousePos = ImVec2{touch_event.get_pos_x(), touch_event.get_pos_y()};
        
        if (touch_event.get_action() == TouchAction::DOWN) {
            io.MouseDown[touch_event.get_pointer_id()] = true;
        } else if (touch_event.get_action() == TouchAction::UP) {
            io.MouseDown[touch_event.get_pointer_id()] = false;
        } else if (touch_event.get_action() == TouchAction::MOVE) {
            capture_move_event = io.WantCaptureMouse;
        }
    }
    
    return capture_move_event;
}

}        // namespace vox
