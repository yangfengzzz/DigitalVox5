//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/view/game_view.h"

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/rendering/subpasses/geometry_subpass.h"

namespace vox::editor::ui {
GameView::GameView(const std::string &title,
                   bool opened,
                   const PanelWindowSettings &window_settings,
                   RenderContext &render_context,
                   Scene *scene)
    : View(title, opened, window_settings, render_context), scene_(scene) {
    scene->background.solid_color = Color(0.2, 0.4, 0.6, 1.0);
    auto editor_root = scene_->FindEntityByName("GameRoot");
    if (!editor_root) {
        editor_root = scene_->CreateRootEntity("GameRoot");
    }
    LoadScene(editor_root);

    // default render pipeline
    std::vector<std::unique_ptr<Subpass>> scene_subpasses{};
    scene_subpasses.emplace_back(std::make_unique<GeometrySubpass>(render_context_, scene, main_camera_));
    render_pipeline_ = std::make_unique<RenderPipeline>(std::move(scene_subpasses));
    auto clear_value = render_pipeline_->GetClearValue();
    clear_value[0].color = {0.2f, 0.4f, 0.6f, 1.f};
    render_pipeline_->SetClearValue(clear_value);
}

void GameView::Render(CommandBuffer &command_buffer) {
    if (render_target_ && IsFocused()) {
        render_pipeline_->Draw(command_buffer, *render_target_);
    }
}

void GameView::Update(float delta_time) {
    View::Update(delta_time);

    auto [win_width, win_height] = SafeSize();
    if (win_width > 0) {
        main_camera_->SetAspectRatio(float(win_width) / float(win_height));
        main_camera_->Resize(win_width, win_height, win_width * 2, win_height * 2);
    }
}

void GameView::LoadScene(Entity *root_entity) {
    auto camera_entity = root_entity->CreateChild("MainCamera");
    camera_entity->transform->SetPosition(10, 0, 0);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
}

}  // namespace vox::editor::ui
