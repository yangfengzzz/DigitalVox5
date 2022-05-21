//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "platform/platform.h"

#include "editor_application.h"
#include "rendering/subpasses/geometry_subpass.h"
#include "camera.h"

#include "ui/menu_bar.h"
#include "ui/hierarchy.h"
#include "ui/inspector.h"
#include "ui/profiler_window.h"
#include "ui/tool_bar.h"
#include "ui/project_settings.h"
#include "ui/console.h"
#include "view/game_view.h"
#include "view/scene_view.h"
#include "view/asset_view.h"
#include "profiling/profiler_spy.h"

namespace vox::editor {
EditorApplication::EditorApplication(const std::string &project_path, const std::string &project_name) :
GraphicsApplication(),
project_path_(project_path),
project_name_(project_name),
project_file_path_(project_path + project_name + ".project"),
engine_assets_path_(std::filesystem::canonical("./assets").string() + "/"),
project_assets_path_(project_path + "./assets/"),
project_scripts_path_(project_path + "./assets/Scripts/"),
editor_assets_path_("./assets/Editor/"),
panels_manager_(canvas_) {
}

EditorApplication::~EditorApplication() {
    // release first
    scene_manager_.reset();
    
    components_manager_.reset();
    physics_manager_.reset();
    light_manager_.reset();
    shadow_manager_.reset();
    particle_manager_.reset();

    image_manager_->CollectGarbage();
    image_manager_.reset();
    shader_manager_->CollectGarbage();
    shader_manager_.reset();
    mesh_manager_->CollectGarbage();
    mesh_manager_.reset();
}

bool EditorApplication::prepare(Platform &platform) {
    GraphicsApplication::prepare(platform);
    
    gui_->load_font("Ruda_Big", "Fonts/Ruda-Bold.ttf", 16);
    gui_->load_font("Ruda_Small", "Fonts/Ruda-Bold.ttf", 12);
    gui_->load_font("Ruda_Medium", "Fonts/Ruda-Bold.ttf", 14);
    gui_->use_font("Ruda_Medium");
    gui_->set_editor_layout_autosave_frequency(60.0f);
    gui_->enable_editor_layout_save(true);
    gui_->enable_docking(true);
    
    // resource loader
    image_manager_ = std::make_unique<ImageManager>(*device_);
    shader_manager_ = std::make_unique<ShaderManager>();
    mesh_manager_ = std::make_unique<MeshManager>(*device_);
    script_interpreter_ = std::make_unique<ScriptInterpreter>("");
    
    // logic system
    components_manager_ = std::make_unique<ComponentsManager>();
    physics_manager_ = std::make_unique<physics::PhysicsManager>();
    scene_manager_ = std::make_unique<SceneManager>(*device_);
    auto scene = scene_manager_->CurrentScene();
    
    particle_manager_ = std::make_unique<ParticleManager>(*device_, *render_context_);
    light_manager_ = std::make_unique<LightManager>(scene, *render_context_);
    {
        auto extent = platform.GetWindow().get_extent();
        auto factor = static_cast<uint32_t>(platform.GetWindow().GetContentScaleFactor());
        components_manager_->CallScriptResize(extent.width, extent.height, factor * extent.width,
                                              factor * extent.height);
    }
    light_manager_->SetCamera(main_camera_);
    
    // internal manager
    shadow_manager_ = std::make_unique<ShadowManager>(*device_, *render_context_, scene, main_camera_);
    
    // default render pipeline
    auto subpass = std::make_unique<GeometrySubpass>(GetRenderContext(), scene, nullptr);
    subpass->SetRenderMode(GeometrySubpass::RenderMode::MANUAL);
    std::vector<std::unique_ptr<Subpass>> scene_subpasses{};
    scene_subpasses.emplace_back(std::move(subpass));
    SetRenderPipeline(RenderPipeline(std::move(scene_subpasses)));
    
    editor_actions_ = std::make_unique<EditorActions>(*this);
    editor_resources_ = std::make_unique<EditorResources>(*device_, editor_assets_path_);
    setup_ui();
    
    return true;
}

void EditorApplication::setup_ui() {
    PanelWindowSettings settings;
    settings.closable = true;
    settings.collapsable = true;
    settings.dockable = true;
    
    panels_manager_.create_panel<ui::MenuBar>("Menu Bar");
    panels_manager_.create_panel<ui::ProfilerWindow>("Profiler", true, settings, 0.25f);
    panels_manager_.create_panel<ui::Console>("Console", true, settings);
    panels_manager_.create_panel<ui::Hierarchy>("Hierarchy", true, settings);
    panels_manager_.create_panel<ui::Inspector>("Inspector", true, settings);
    panels_manager_.create_panel<ui::SceneView>("Scene View", true, settings,
                                                *render_context_,
                                                scene_manager_->CurrentScene());
    panels_manager_.create_panel<ui::GameView>("Game View", true, settings,
                                               *render_context_,
                                               scene_manager_->CurrentScene());
    
    panels_manager_.create_panel<ui::AssetView>("Asset View", true, settings,
                                                *render_context_,
                                                scene_manager_->CurrentScene());
    panels_manager_.create_panel<ui::Toolbar>("Toolbar", true, settings, editor_resources_.get());
    panels_manager_.create_panel<ui::ProjectSettings>("Project Settings", false, settings, project_path_, project_name_);
    
    canvas_.make_dock_space(true);
    gui_->set_canvas(canvas_);
    scene_manager_->CurrentScene()->play();
}

//MARK: - Update
void EditorApplication::update(float delta_time) {
//    if (auto editorMode = editor_actions_->current_editor_mode();
//        editorMode == EditorActions::EditorMode::PLAY ||
//        editorMode == EditorActions::EditorMode::FRAME_BY_FRAME) {
components_manager_->CallScriptOnStart();
        
        physics_manager_->update(delta_time);

        components_manager_->CallScriptOnUpdate(delta_time);
        //        _componentsManager->callAnimatorUpdate(deltaTime);
        components_manager_->CallSceneAnimatorUpdate(delta_time);
        components_manager_->CallScriptOnLateUpdate(delta_time);

        components_manager_->CallRendererOnUpdate(delta_time);
        scene_manager_->CurrentScene()->UpdateShaderData();
        
//        if (editorMode == EditorActions::EditorMode::FRAME_BY_FRAME)
//            editor_actions_->pause_game();
//    }
    
    PROFILER_SPY("Scene garbage collection");
    image_manager_->CollectGarbage();
    mesh_manager_->CollectGarbage();
    shader_manager_->CollectGarbage();
    
    delta_time_ = delta_time;
    GraphicsApplication::update(delta_time);
    editor_actions_->execute_delayed_actions();
}

void EditorApplication::render(CommandBuffer &command_buffer, RenderTarget &render_target) {
    update_gpu_task(command_buffer, render_target);
    update_editor_panels(delta_time_);
    render_views(delta_time_, command_buffer);
    
    GraphicsApplication::render(command_buffer, render_target);
}

void EditorApplication::update_gpu_task(CommandBuffer &command_buffer, RenderTarget &render_target) {
    shadow_manager_->draw(command_buffer);
    light_manager_->draw(command_buffer, render_target);
    particle_manager_->draw(command_buffer, render_target);
}

void EditorApplication::update_editor_panels(float delta_time) {
    auto &menu_bar = panels_manager_.get_panel_as<ui::MenuBar>("Menu Bar");
    auto& profiler = panels_manager_.get_panel_as<ui::ProfilerWindow>("Profiler");
    
    menu_bar.handle_shortcuts(delta_time);
    
    if (profiler.is_opened()) {
        PROFILER_SPY("Profiler Update");
        profiler.update(delta_time);
    }
}

void EditorApplication::render_views(float delta_time, CommandBuffer &command_buffer) {
    auto &game_view = panels_manager_.get_panel_as<ui::GameView>("Game View");
    auto &scene_view = panels_manager_.get_panel_as<ui::SceneView>("Scene View");
    auto &asset_view = panels_manager_.get_panel_as<ui::AssetView>("Asset View");
    
    {
        PROFILER_SPY("Editor Views Update");
        asset_view.update(delta_time);
        game_view.update(delta_time);
        scene_view.update(delta_time);
    }
    
    if (asset_view.is_opened()) {
        PROFILER_SPY("Game View Rendering");
        asset_view.render(command_buffer);
    }
    
    if (game_view.is_opened()) {
        PROFILER_SPY("Game View Rendering");
        game_view.render(command_buffer);
    }
    
    if (scene_view.is_opened()) {
        PROFILER_SPY("Scene View Rendering");
        scene_view.render(command_buffer);
    }
}

bool EditorApplication::resize(uint32_t win_width, uint32_t win_height,
                               uint32_t fb_width, uint32_t fb_height) {
    GraphicsApplication::resize(win_width, win_height, fb_width, fb_height);
    components_manager_->CallScriptResize(win_width, win_height, fb_width, fb_height);
    return true;
}

void EditorApplication::input_event(const InputEvent &input_event) {
    GraphicsApplication::input_event(input_event);
    components_manager_->CallScriptInputEvent(input_event);
    
    auto &scene_view = panels_manager_.get_panel_as<ui::SceneView>("Scene View");
    scene_view.input_event(input_event);
    
    if (input_event.GetSource() == EventSource::KEYBOARD) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
        if (key_event.GetCode() == KeyCode::ESCAPE) {
            editor_actions_->stop_playing();
        }
        if (key_event.GetCode() == KeyCode::F5) {
            editor_actions_->start_playing();
        }
    }
}

}
