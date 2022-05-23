//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/framebuffer_picker_app.h"

#include "vox.render/camera.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/platform/platform.h"

namespace vox {
void FramebufferPickerApp::LoadScene() {
    u = std::uniform_real_distribution<float>(0, 1);
    auto scene = scene_manager_->CurrentScene();
    auto root_entity = scene->CreateRootEntity();

    auto camera_entity = root_entity->CreateChild("camera");
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();

    // init point light
    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(0, 3, 0);
    auto point_light = light->AddComponent<PointLight>();
    point_light->intensity_ = 0.3;

    // create box test entity
    float cube_size = 2.0;
    auto box_entity = root_entity->CreateChild("BoxEntity");
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    auto box_renderer = box_entity->AddComponent<MeshRenderer>();
    box_mtl->SetBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    box_renderer->SetMesh(PrimitiveMesh::CreateCuboid(cube_size, cube_size, cube_size));
    box_renderer->SetMaterial(box_mtl);

    // create sphere test entity
    float radius = 1.25;
    auto sphere_entity = root_entity->CreateChild("SphereEntity");
    sphere_entity->transform->SetPosition(Point3F(-5, 0, 0));
    auto sphere_renderer = sphere_entity->AddComponent<MeshRenderer>();
    auto sphere_mtl = std::make_shared<BlinnPhongMaterial>(*device_);
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    sphere_mtl->SetBaseColor(Color(u(e), u(e), u(e), 1));
    sphere_renderer->SetMesh(PrimitiveMesh::CreateSphere(radius));
    sphere_renderer->SetMaterial(sphere_mtl);

    scene->Play();
}

void FramebufferPickerApp::PickFunctor(Renderer *renderer, const MeshPtr &mesh) {
    if (renderer && mesh) {
        static_cast<BlinnPhongMaterial *>(renderer->GetMaterial().get())->SetBaseColor(Color(u(e), u(e), u(e), 1));
    }
}

// MARK: - Render Pipeline
std::unique_ptr<RenderTarget> FramebufferPickerApp::CreateRenderTarget(uint32_t width, uint32_t height) {
    VkExtent3D extent{width, height, 1};

    core::Image color_target{*device_, extent, VK_FORMAT_R8G8B8A8_UNORM,
                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                             VMA_MEMORY_USAGE_GPU_ONLY};

    core::Image depth_image{*device_, extent, GetSuitableDepthFormat(device_->GetGpu().GetHandle()),
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            VMA_MEMORY_USAGE_GPU_ONLY};

    std::vector<core::Image> images;
    images.push_back(std::move(color_target));
    images.push_back(std::move(depth_image));
    return std::make_unique<RenderTarget>(std::move(images));
}

bool FramebufferPickerApp::Prepare(Platform &platform) {
    ForwardApplication::Prepare(platform);
    auto extent = platform.GetWindow().GetExtent();
    auto factor = static_cast<uint32_t>(platform.GetWindow().GetContentScaleFactor());
    color_picker_render_target_ = CreateRenderTarget(extent.width * factor, extent.height * factor);

    auto subpass = std::make_unique<ColorPickerSubpass>(*render_context_, scene_manager_->CurrentScene(), main_camera_);
    color_picker_subpass_ = subpass.get();
    color_picker_render_pipeline_ = std::make_unique<RenderPipeline>();
    color_picker_render_pipeline_->AddSubpass(std::move(subpass));
    auto clear_value = color_picker_render_pipeline_->GetClearValue();
    clear_value[0].color = {1.0f, 1.0f, 1.0f, 1.0f};
    color_picker_render_pipeline_->SetClearValue(clear_value);

    stage_buffer_ =
            std::make_unique<core::Buffer>(*device_, 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU);

    regions_.resize(1);
    regions_[0].imageSubresource.layerCount = 1;
    regions_[0].imageExtent.width = 1;
    regions_[0].imageExtent.height = 1;
    regions_[0].imageExtent.depth = 1;

    return true;
}

bool FramebufferPickerApp::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    ForwardApplication::Resize(win_width, win_height, fb_width, fb_height);

    if (color_picker_render_target_->GetExtent().width != fb_width ||
        color_picker_render_target_->GetExtent().height != fb_height) {
        color_picker_render_target_ = CreateRenderTarget(fb_width, fb_height);
    }
    return true;
}

// MARK: - Main Loop
void FramebufferPickerApp::InputEvent(const vox::InputEvent &input_event) {
    ForwardApplication::InputEvent(input_event);
    if (input_event.GetSource() == EventSource::MOUSE) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
        if (mouse_button.GetAction() == MouseAction::DOWN) {
            Pick(mouse_button.GetPosX(), mouse_button.GetPosY());
        }
    }
}

void FramebufferPickerApp::Pick(float offset_x, float offset_y) {
    need_pick_ = true;
    pick_pos_ = Vector2F(offset_x, offset_y);
}

void FramebufferPickerApp::Render(CommandBuffer &command_buffer, RenderTarget &render_target) {
    if (need_pick_) {
        color_picker_render_pipeline_->Draw(command_buffer, *color_picker_render_target_);
        command_buffer.EndRenderPass();
        CopyRenderTargetToBuffer(command_buffer);
    }

    ForwardApplication::Render(command_buffer, render_target);
}

void FramebufferPickerApp::Update(float delta_time) {
    ForwardApplication::Update(delta_time);
    if (need_pick_) {
        ReadColorFromRenderTarget();
        PickFunctor(pick_result_.first, pick_result_.second);
        need_pick_ = false;
    }
}

void FramebufferPickerApp::CopyRenderTargetToBuffer(CommandBuffer &command_buffer) {
    uint32_t client_width = main_camera_->Width();
    uint32_t client_height = main_camera_->Height();
    uint32_t canvas_width = main_camera_->FramebufferWidth();
    uint32_t canvas_height = main_camera_->FramebufferHeight();

    const float kPx = (pick_pos_.x / static_cast<float>(client_width)) * static_cast<float>(canvas_width);
    const float kPy = (pick_pos_.y / static_cast<float>(client_height)) * static_cast<float>(canvas_height);

    const auto kViewport = main_camera_->Viewport();
    const auto kViewWidth = (kViewport.z - kViewport.x) * static_cast<float>(canvas_width);
    const auto kViewHeight = (kViewport.w - kViewport.y) * static_cast<float>(canvas_height);

    const float kNx = (kPx - kViewport.x) / kViewWidth;
    const float kNy = (kPy - kViewport.y) / kViewHeight;
    const uint32_t kLeft = std::floor(kNx * static_cast<float>(canvas_width - 1));
    const uint32_t kBottom = std::floor((1 - kNy) * static_cast<float>(canvas_height - 1));

    regions_[0].imageOffset.x = static_cast<int32_t>(kLeft);
    regions_[0].imageOffset.y = static_cast<int32_t>(canvas_height - kBottom);
    command_buffer.CopyImageToBuffer(color_picker_render_target_->GetViews().at(0).GetImage(),
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, *stage_buffer_, regions_);
}

void FramebufferPickerApp::ReadColorFromRenderTarget() {
    uint8_t *raw = stage_buffer_->Map();
    if (raw) {
        memcpy(pixel_.data(), raw, 4);
        pick_result_ = color_picker_subpass_->GetObjectByColor(pixel_);
    }
    stage_buffer_->Unmap();
}

}  // namespace vox
