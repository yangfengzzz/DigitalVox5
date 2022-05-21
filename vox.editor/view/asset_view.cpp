//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "material/blinn_phong_material.h"
#include "asset_view.h"
#include "camera.h"
#include "entity.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "shader/internal_variant_name.h"

namespace vox::editor::ui {
AssetView::AssetView(const std::string &title, bool opened,
                     const PanelWindowSettings &window_settings,
                     RenderContext &render_context, Scene *scene) :
View(title, opened, window_settings, render_context),
scene_(scene) {
    scene->background_.solid_color = Color(0.2, 0.4, 0.6, 1.0);
    auto editor_root = scene_->FindEntityByName("AssetRoot");
    if (!editor_root) {
        editor_root = scene_->CreateRootEntity("AssetRoot");
    }
    load_scene(editor_root);
    
    // default render pipeline
    std::vector<std::unique_ptr<Subpass>> scene_subpasses{};
    scene_subpasses.emplace_back(std::make_unique<GeometrySubpass>(render_context_, scene, main_camera_));
    subpass_ = static_cast<GeometrySubpass*>(scene_subpasses[0].get());
    render_pipeline_ = std::make_unique<RenderPipeline>(std::move(scene_subpasses));
    auto clear_value = render_pipeline_->GetClearValue();
    clear_value[0].color = {0.2f, 0.4f, 0.6f, 1.f};
    render_pipeline_->SetClearValue(clear_value);

    subpass_->SetRenderMode(GeometrySubpass::RenderMode::MANUAL);
    subpass_->AddRenderElement(elements_[0]);
    subpass_->AddRenderElement(elements_[1]);
}

void AssetView::load_scene(Entity *root_entity) {
    auto camera_entity = root_entity->CreateChild("MainCamera");
    camera_entity->transform->SetPosition(10, 10, 10);
    camera_entity->transform->LookAt(Point3F(0, 0, 0));
    main_camera_ = camera_entity->AddComponent<Camera>();
    camera_control_ = camera_entity->AddComponent<control::OrbitControl>();
    
    auto grid = root_entity->AddComponent<MeshRenderer>();
    grid->SetMesh(create_plane());
    grid->SetMaterial(std::make_shared<GridMaterial>(render_context_.GetDevice()));
    grid->set_enabled(false);
    elements_.emplace_back(grid, grid->mesh(), grid->mesh()->FirstSubMesh(), grid->GetMaterial());
    
    // create box test entity
    float radius = 2.0;
    auto sphere_entity = root_entity->CreateChild("SphereEntity");
    auto sphere_mtl = std::make_shared<BlinnPhongMaterial>(render_context_.GetDevice());
    sphere_mtl->SetBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    auto renderer = sphere_entity->AddComponent<MeshRenderer>();
    renderer->SetMesh(PrimitiveMesh::CreateSphere(radius));
    renderer->SetMaterial(sphere_mtl);
    renderer->shader_data_.AddDefine(HAS_UV);
    renderer->shader_data_.AddDefine(HAS_NORMAL);
    renderer->set_enabled(false);
    elements_.emplace_back(renderer, renderer->mesh(), renderer->mesh()->FirstSubMesh(), renderer->GetMaterial());
}

void AssetView::update(float delta_time) {
    View::update(delta_time);
    
    auto [win_width, win_height] = safe_size();
    if (win_width > 0) {
        main_camera_->SetAspectRatio(float(win_width) / float(win_height));
        main_camera_->resize(win_width, win_height, win_width * 2, win_height * 2);
    }
}

void AssetView::render(CommandBuffer &command_buffer) {
    if (is_focused()) {
        camera_control_->onEnable();
    } else {
        camera_control_->onDisable();
    }
    
    if (render_target_) {
        render_pipeline_->draw(command_buffer, *render_target_);
    }
}

control::OrbitControl *AssetView::camera_control() {
    return camera_control_;
}


}
