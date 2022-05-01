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
    scene->background_.solid_color_ = Color(0.2, 0.4, 0.6, 1.0);
    auto editor_root = scene_->find_entity_by_name("AssetRoot");
    if (!editor_root) {
        editor_root = scene_->create_root_entity("AssetRoot");
    }
    load_scene(editor_root);
    
    // default render pipeline
    std::vector<std::unique_ptr<Subpass>> scene_subpasses{};
    scene_subpasses.emplace_back(std::make_unique<GeometrySubpass>(render_context_, scene, main_camera_));
    subpass_ = static_cast<GeometrySubpass*>(scene_subpasses[0].get());
    render_pipeline_ = std::make_unique<RenderPipeline>(std::move(scene_subpasses));
    auto clear_value = render_pipeline_->get_clear_value();
    clear_value[0].color = {0.2f, 0.4f, 0.6f, 1.f};
    render_pipeline_->set_clear_value(clear_value);
    
    subpass_->set_render_mode(GeometrySubpass::RenderMode::MANUAL);
    subpass_->add_render_element(elements_[0]);
    subpass_->add_render_element(elements_[1]);
}

void AssetView::load_scene(Entity *root_entity) {
    auto camera_entity = root_entity->create_child("MainCamera");
    camera_entity->transform_->set_position(10, 10, 10);
    camera_entity->transform_->look_at(Point3F(0, 0, 0));
    main_camera_ = camera_entity->add_component<Camera>();
    camera_control_ = camera_entity->add_component<control::OrbitControl>();
    
    auto grid = root_entity->add_component<MeshRenderer>();
    grid->set_mesh(create_plane());
    grid->set_material(std::make_shared<GridMaterial>(render_context_.get_device()));
    grid->set_enabled(false);
    elements_.emplace_back(grid, grid->mesh(),
                           grid->mesh()->sub_mesh(), grid->get_material());
    
    // create box test entity
    float radius = 2.0;
    auto sphere_entity = root_entity->create_child("SphereEntity");
    auto sphere_mtl = std::make_shared<BlinnPhongMaterial>(render_context_.get_device());
    sphere_mtl->set_base_color(Color(0.8, 0.3, 0.3, 1.0));
    auto renderer = sphere_entity->add_component<MeshRenderer>();
    renderer->set_mesh(PrimitiveMesh::create_sphere(radius));
    renderer->set_material(sphere_mtl);
    renderer->shader_data_.add_define(HAS_UV);
    renderer->shader_data_.add_define(HAS_NORMAL);
    renderer->set_enabled(false);
    elements_.emplace_back(renderer, renderer->mesh(),
                           renderer->mesh()->sub_mesh(), renderer->get_material());
}

void AssetView::update(float delta_time) {
    View::update(delta_time);
    
    auto [win_width, win_height] = safe_size();
    if (win_width > 0) {
        main_camera_->set_aspect_ratio(float(win_width) / float(win_height));
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

}
