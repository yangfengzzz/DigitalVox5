//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/view/view.h"

#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/shader/shader_manager.h"
#include "vox.render/ui/imgui_impl_vulkan.h"

namespace vox::editor::ui {
View::View(const std::string &p_title,
           bool p_opened,
           const PanelWindowSettings &p_window_settings,
           RenderContext &render_context)
    : PanelWindow(p_title, p_opened, p_window_settings),
      render_context_(render_context),
      sampler_create_info_{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO} {
    auto &device = render_context.GetDevice();

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
    // In a real-world scenario the level of anisotropy should be a user setting or e.g. lowered for mobile devices by
    // default
    sampler_create_info_.maxAnisotropy = device.GetGpu().GetFeatures().samplerAnisotropy
                                                 ? (device.GetGpu().GetProperties().limits.maxSamplerAnisotropy)
                                                 : 1.0f;
    sampler_create_info_.anisotropyEnable = device.GetGpu().GetFeatures().samplerAnisotropy;
    sampler_create_info_.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    sampler_ = std::make_unique<core::Sampler>(device, sampler_create_info_);

    scrollable_ = false;
}

void View::Update(float p_delta_time) {
    auto [win_width, win_height] = SafeSize();

    if (win_width > 0) {
        if (!image_) {
            image_ = &CreateWidget<::vox::ui::Image>(nullptr, Vector2F{0.f, 0.f});
        }

        image_->size_ = Vector2F(static_cast<float>(win_width), static_cast<float>(win_height));
        if (!render_target_ || win_width * 2 != render_target_->GetExtent().width ||
            win_height * 2 != render_target_->GetExtent().height) {
            render_target_ = CreateRenderTarget(win_width * 2, win_height * 2);
            image_->SetTextureView(ImGui_ImplVulkan_AddTexture(sampler_->GetHandle(),
                                                               render_target_->GetViews().at(0).GetHandle(),
                                                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        }
    }
}

void View::DrawImpl() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PanelWindow::DrawImpl();
    ImGui::PopStyleVar();
}

std::pair<uint16_t, uint16_t> View::SafeSize() const {
    auto result = Size() - Vector2F{0.f, 25.f};  // 25 == title bar height
    return {static_cast<uint16_t>(result.x), static_cast<uint16_t>(result.y)};
}

std::unique_ptr<RenderTarget> View::CreateRenderTarget(uint32_t width, uint32_t height, VkFormat format) {
    VkExtent3D extent{width, height, 1};

    core::Image color_target{render_context_.GetDevice(), extent,
                             format == VK_FORMAT_UNDEFINED ? render_context_.GetSwapchain().GetFormat() : format,
                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                             VMA_MEMORY_USAGE_GPU_ONLY};

    core::Image depth_image{render_context_.GetDevice(), extent,
                            GetSuitableDepthFormat(render_context_.GetDevice().GetGpu().GetHandle()),
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            VMA_MEMORY_USAGE_GPU_ONLY};

    std::vector<core::Image> images;
    images.push_back(std::move(color_target));
    images.push_back(std::move(depth_image));
    return std::make_unique<RenderTarget>(std::move(images));
}

// MARK: - Grid
GridMaterial::GridMaterial(Device &device) : BaseMaterial(device, "editor-grid") {
    SetIsTransparent(true);
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/editor/grid.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/editor/grid.frag");
}

const Vector3F &View::GridColor() const { return grid_color_; }

void View::SetGridColor(const Vector3F &p_color) { grid_color_ = p_color; }

ModelMeshPtr View::CreatePlane() {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();
    mesh->AddSubMesh(0, 6);
    return mesh;
}

}  // namespace vox::editor::ui
