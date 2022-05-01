//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "view.h"
#include "mesh/mesh_renderer.h"
#include "mesh/mesh_manager.h"
#include "shader/shader_manager.h"
#include "ui/imgui_impl_vulkan.h"

namespace vox::editor::ui {
View::View(const std::string &p_title, bool p_opened,
           const PanelWindowSettings &p_window_settings,
           RenderContext &render_context) :
PanelWindow(p_title, p_opened, p_window_settings),
render_context_(render_context),
sampler_create_info_{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO} {
    auto &device = render_context.get_device();
    
    // Create a default sampler
    sampler_create_info_.magFilter = VK_FILTER_LINEAR;
    sampler_create_info_.minFilter = VK_FILTER_LINEAR;
    sampler_create_info_.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info_.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info_.mipLodBias = 0.0f;
    sampler_create_info_.compareOp = VK_COMPARE_OP_NEVER;
    sampler_create_info_.minLod = 0.0f;
    // Max level-of-detail should match mip level count
    sampler_create_info_.maxLod = 0.0f;
    // Only enable anisotropic filtering if enabled on the device
    // Note that for simplicity, we will always be using max. available anisotropy level for the current device
    // This may have an impact on performance, esp. on lower-specced devices
    // In a real-world scenario the level of anisotropy should be a user setting or e.g. lowered for mobile devices by default
    sampler_create_info_.maxAnisotropy = device.get_gpu().get_features().samplerAnisotropy
    ? (device.get_gpu().get_properties().limits.maxSamplerAnisotropy)
    : 1.0f;
    sampler_create_info_.anisotropyEnable = device.get_gpu().get_features().samplerAnisotropy;
    sampler_create_info_.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    
    sampler_ = std::make_unique<core::Sampler>(device, sampler_create_info_);
    
    scrollable_ = false;
}

void View::update(float p_delta_time) {
    auto [win_width, win_height] = safe_size();
    
    if (win_width > 0) {
        if (!image_) {
            image_ = &create_widget<::vox::ui::Image>(nullptr, Vector2F{0.f, 0.f});
        }
        
        image_->size_ = Vector2F(static_cast<float>(win_width), static_cast<float>(win_height));
        if (!render_target_ ||
            win_width * 2 != render_target_->get_extent().width ||
            win_height * 2 != render_target_->get_extent().height) {
            render_target_ = create_render_target(win_width * 2, win_height * 2);
            image_->set_texture_view(ImGui_ImplVulkan_AddTexture(sampler_->get_handle(), render_target_->get_views().at(0).get_handle(),
                                                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        }
    }
}

void View::draw_impl() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PanelWindow::draw_impl();
    ImGui::PopStyleVar();
}

std::pair<uint16_t, uint16_t> View::safe_size() const {
    auto result = size() - Vector2F{0.f, 25.f}; // 25 == title bar height
    return {static_cast<uint16_t>(result.x), static_cast<uint16_t>(result.y)};
}

std::unique_ptr<RenderTarget> View::create_render_target(uint32_t width, uint32_t height) {
    VkExtent3D extent{width, height, 1};
    
    core::Image color_target{
        render_context_.get_device(), extent,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY
    };
    
    core::Image depth_image{
        render_context_.get_device(), extent,
        get_suitable_depth_format(render_context_.get_device().get_gpu().get_handle()),
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY
    };
    
    std::vector<core::Image> images;
    images.push_back(std::move(color_target));
    images.push_back(std::move(depth_image));
    return std::make_unique<RenderTarget>(std::move(images));
}

//MARK: - Grid
GridMaterial::GridMaterial(Device &device) :
BaseMaterial(device, "editor-grid") {
    set_is_transparent(true);
    vertex_source_ = ShaderManager::get_singleton().load_shader("");
    fragment_source_ = ShaderManager::get_singleton().load_shader("");
}

const Vector3F &View::grid_color() const {
    return grid_color_;
}

void View::set_grid_color(const Vector3F &p_color) {
    grid_color_ = p_color;
}

ModelMeshPtr View::create_plane() {
    auto mesh = MeshManager::get_singleton().load_model_mesh();
    
    auto positions = std::vector<Vector3F>(4);
    positions[0] = Vector3F(-1, -1, 0);
    positions[1] = Vector3F(1, -1, 0);
    positions[2] = Vector3F(-1, 1, 0);
    positions[3] = Vector3F(1, 1, 0);
    
    auto indices = std::vector<uint32_t>(6);
    indices[0] = 1;
    indices[1] = 2;
    indices[2] = 0;
    indices[3] = 1;
    indices[4] = 3;
    indices[5] = 2;
    
    mesh->set_positions(positions);
    mesh->set_indices(indices);
    mesh->upload_data(true);
    mesh->add_sub_mesh(0, 6);
    return mesh;
}

}
