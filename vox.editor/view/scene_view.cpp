//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/view/scene_view.h"

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/lighting/point_light.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/rendering/subpasses/color_picker_subpass.h"
#include "vox.render/rendering/subpasses/geometry_subpass.h"

namespace vox::editor::ui {
SceneView::SceneView(const std::string &title,
                     bool opened,
                     const PanelWindowSettings &window_settings,
                     RenderContext &render_context,
                     Scene *scene)
    : View(title, opened, window_settings, render_context), scene_(scene) {
    scene->background.solid_color = Color(0.2, 0.4, 0.6, 1.0);
    auto editor_root = scene_->FindEntityByName("SceneRoot");
    if (!editor_root) {
        editor_root = scene_->CreateRootEntity("SceneRoot");
    }
    LoadScene(editor_root);

    // scene render target
    {
        std::vector<std::unique_ptr<Subpass>> scene_subpasses{};
        scene_subpasses.emplace_back(std::make_unique<GeometrySubpass>(render_context_, scene, main_camera_));
        render_pipeline_ = std::make_unique<RenderPipeline>(std::move(scene_subpasses));
        auto clear_value = render_pipeline_->GetClearValue();
        clear_value[0].color = {0.2f, 0.4f, 0.6f, 1.f};
        render_pipeline_->SetClearValue(clear_value);
    }

    // color picker render target
    {
        auto subpass = std::make_unique<ColorPickerSubpass>(render_context, scene, main_camera_);
        color_picker_subpass_ = subpass.get();
        color_picker_render_pipeline_ = std::make_unique<RenderPipeline>();
        color_picker_render_pipeline_->AddSubpass(std::move(subpass));
        auto clear_value = color_picker_render_pipeline_->GetClearValue();
        clear_value[0].color = {1.0f, 1.0f, 1.0f, 1.0f};
        color_picker_render_pipeline_->SetClearValue(clear_value);

        // dont't render grid when pick
        color_picker_subpass_->AddExclusiveRenderer(editor_root->GetComponent<MeshRenderer>());
    }

    stage_buffer_ = std::make_unique<core::Buffer>(render_context.GetDevice(), 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                   VMA_MEMORY_USAGE_GPU_TO_CPU);

    regions_.resize(1);
    regions_[0].imageSubresource.layerCount = 1;
    regions_[0].imageExtent.width = 1;
    regions_[0].imageExtent.height = 1;
    regions_[0].imageExtent.depth = 1;
}

void SceneView::LoadScene(Entity *root_entity) {
    auto camera_entity = root_entity->CreateChild("MainCamera");
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_control_ = camera_entity->AddComponent<control::OrbitControl>();

    auto grid = root_entity->AddComponent<MeshRenderer>();
    grid->SetMesh(CreatePlane());
    grid->SetMaterial(std::make_shared<GridMaterial>(render_context_.GetDevice()));

    // init point light
    auto light = root_entity->CreateChild("light");
    light->transform->SetPosition(0, 3, 0);
    auto point_light = light->AddComponent<PointLight>();
    point_light->intensity_ = 0.3;

    // create box test entity
    float cube_size = 2.0;
    auto box_entity = root_entity->CreateChild("BoxEntity");
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(render_context_.GetDevice());
    auto box_renderer = box_entity->AddComponent<MeshRenderer>();
    box_mtl->SetBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    box_renderer->SetMesh(PrimitiveMesh::CreateCuboid(cube_size, cube_size, cube_size));
    box_renderer->SetMaterial(box_mtl);
}

void SceneView::Update(float delta_time) {
    View::Update(delta_time);

    auto [win_width, win_height] = SafeSize();
    if (win_width > 0 &&
        (!color_picker_render_target_ || color_picker_render_target_->GetExtent().width != win_width * 2)) {
        main_camera_->SetAspectRatio(float(win_width) / float(win_height));
        main_camera_->Resize(win_width, win_height, win_width * 2, win_height * 2);
        color_picker_render_target_ = CreateRenderTarget(win_width * 2, win_height * 2, VK_FORMAT_R8G8B8A8_UNORM);
    }
}

void SceneView::Render(CommandBuffer &command_buffer) {
    if (IsFocused()) {
        camera_control_->OnScriptEnable();
    } else {
        camera_control_->OnScriptDisable();
    }

    // Let the first frame happen and then make the scene view the first seen view
    if (elapsed_frames_) {
        Focus();
    }

    if (render_target_ && IsFocused()) {
        elapsed_frames_ = false;

        if (need_pick_) {
            color_picker_render_pipeline_->Draw(command_buffer, *color_picker_render_target_);
            command_buffer.EndRenderPass();
            CopyRenderTargetToBuffer(command_buffer);
        }

        render_pipeline_->Draw(command_buffer, *render_target_);

        if (need_pick_) {
            ReadColorFromRenderTarget();
            need_pick_ = false;
        }
    }
}

void SceneView::DrawImpl() {
    View::DrawImpl();
    int window_flags = ImGuiWindowFlags_None;

    if (!resizable_) window_flags |= ImGuiWindowFlags_NoResize;
    if (!movable_) window_flags |= ImGuiWindowFlags_NoMove;
    if (!dockable_) window_flags |= ImGuiWindowFlags_NoDocking;
    if (hide_background_) window_flags |= ImGuiWindowFlags_NoBackground;
    if (force_horizontal_scrollbar_) window_flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
    if (force_vertical_scrollbar_) window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
    if (allow_horizontal_scrollbar_) window_flags |= ImGuiWindowFlags_HorizontalScrollbar;
    if (!bring_to_front_on_focus_) window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (!collapsable_) window_flags |= ImGuiWindowFlags_NoCollapse;
    if (!allow_inputs_) window_flags |= ImGuiWindowFlags_NoInputs;
    if (!scrollable_) window_flags |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
    if (!title_bar_) window_flags |= ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin((name_ + PanelId()).c_str(), nullptr, window_flags)) {
        if (pick_result_.first != nullptr) {
            if (ImGuizmo::IsOver()) {
                camera_control_->SetEnabled(false);
            }
            // camera transform
            auto camera_projection = main_camera_->ProjectionMatrix();
            camera_projection(1, 1) *= -1;  // vulkan flip
            auto camera_view = main_camera_->ViewMatrix();
            auto &camera_transform = main_camera_->GetEntity()->transform;

            // renderer transform
            auto renderer = pick_result_.first;
            auto &renderer_transform = renderer->GetEntity()->transform;
            auto model_mat = renderer_transform->LocalMatrix();

            // open gizmo
            cam_distance_ = camera_transform->WorldPosition().distanceTo(renderer_transform->WorldPosition());
            EditTransform(camera_view.data(), camera_projection.data(), model_mat.data());

            // data store
            renderer_transform->SetLocalMatrix(model_mat);
            camera_view.invert();
            camera_transform->SetWorldMatrix(camera_view);
        }
    }
    ImGui::End();
}

void SceneView::Pick(float offset_x, float offset_y) {
    need_pick_ = true;
    pick_pos_ = Vector2F(offset_x, offset_y);
}

void SceneView::CopyRenderTargetToBuffer(CommandBuffer &command_buffer) {
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

void SceneView::ReadColorFromRenderTarget() {
    uint8_t *raw = stage_buffer_->Map();
    if (raw) {
        memcpy(pixel_.data(), raw, 4);
        auto result = color_picker_subpass_->GetObjectByColor(pixel_);
        if (result.first) {
            pick_result_ = result;
        }
    }
    stage_buffer_->Unmap();
}

void SceneView::InputEvent(const vox::InputEvent &input_event) {
    if (input_event.GetSource() == EventSource::MOUSE) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
        if (mouse_button.GetAction() == MouseAction::DOWN) {
            auto width = main_camera_->Width();
            auto height = main_camera_->Height();

            auto picker_pos_x = mouse_button.GetPosX() - Position().x;
            auto picker_pos_y = mouse_button.GetPosY() - Position().y;

            if (picker_pos_x <= width && picker_pos_x > 0 && picker_pos_y <= height && picker_pos_y > 0) {
                Pick(picker_pos_x, picker_pos_y);
            }
        }
    }
}

void SceneView::EditTransform(float *camera_view, float *camera_projection, float *matrix) {
    static ImGuizmo::MODE m_current_gizmo_mode(ImGuizmo::LOCAL);
    static bool use_snap = false;
    static float snap[3] = {1.f, 1.f, 1.f};
    static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
    static float bounds_snap[] = {0.1f, 0.1f, 0.1f};
    static bool bound_sizing = false;
    static bool bound_sizing_snap = false;

    auto [win_width, win_height] = SafeSize();
    auto viewport_pos = Position();
    float view_manipulate_right = win_width + viewport_pos.x;
    float view_manipulate_top = viewport_pos.y + 25;
    ImGuizmo::SetRect(viewport_pos.x, viewport_pos.y, win_width, win_height);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

    ImGuizmo::Manipulate(camera_view, camera_projection, current_gizmo_operation_, m_current_gizmo_mode, matrix,
                         nullptr, use_snap ? &snap[0] : nullptr, bound_sizing ? bounds : nullptr,
                         bound_sizing_snap ? bounds_snap : nullptr);

    ImGuizmo::ViewManipulate(camera_view, cam_distance_, ImVec2(view_manipulate_right - 128, view_manipulate_top),
                             ImVec2(128, 128), 0x10101010);
}

control::OrbitControl *SceneView::CameraControl() { return camera_control_; }

}  // namespace vox::editor::ui
