//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/platform.h"

#include "vox.editor/editor_application.h"

#include "vox.render/camera.h"
#include "vox.editor/profiling/profiler_spy.h"
#include "vox.render/rendering/subpasses/geometry_subpass.h"
#include "vox.editor/ui/console.h"
#include "vox.editor/ui/hierarchy.h"
#include "vox.editor/ui/inspector.h"
#include "vox.editor/ui/menu_bar.h"
#include "vox.editor/ui/profiler_window.h"
#include "vox.editor/ui/project_settings.h"
#include "vox.editor/ui/tool_bar.h"
#include "vox.editor/view/asset_view.h"
#include "vox.editor/view/game_view.h"
#include "vox.editor/view/scene_view.h"

namespace vox::editor {
EditorApplication::EditorApplication(const std::string &project_path, const std::string &project_name)
    : GraphicsApplication(),
      project_path_(project_path),
      project_name_(project_name),
      project_file_path_(project_path + project_name + ".project"),
      engine_assets_path_(std::filesystem::canonical("./assets").string() + "/"),
      project_assets_path_(project_path + "./assets/"),
      project_scripts_path_(project_path + "./assets/Scripts/"),
      editor_assets_path_("./assets/Editor/"),
      panels_manager_(canvas_) {}

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

bool EditorApplication::Prepare(Platform &platform) {
    GraphicsApplication::Prepare(platform);

    gui_->LoadFont("Ruda_Big", "Fonts/Ruda-Bold.ttf", 16);
    gui_->LoadFont("Ruda_Small", "Fonts/Ruda-Bold.ttf", 12);
    gui_->LoadFont("Ruda_Medium", "Fonts/Ruda-Bold.ttf", 14);
    gui_->UseFont("Ruda_Medium");
    gui_->SetEditorLayoutAutosaveFrequency(60.0f);
    gui_->EnableEditorLayoutSave(true);
    gui_->EnableDocking(true);

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
        auto extent = platform.GetWindow().GetExtent();
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
    SetupUi();

    return true;
}

void EditorApplication::SetupUi() {
    PanelWindowSettings settings;
    settings.closable = true;
    settings.collapsable = true;
    settings.dockable = true;

    panels_manager_.CreatePanel<ui::MenuBar>("Menu Bar");
    panels_manager_.CreatePanel<ui::ProfilerWindow>("Profiler", true, settings, 0.25f);
    panels_manager_.CreatePanel<ui::Console>("Console", true, settings);
    panels_manager_.CreatePanel<ui::Hierarchy>("Hierarchy", true, settings);
    panels_manager_.CreatePanel<ui::Inspector>("Inspector", true, settings);
    panels_manager_.CreatePanel<ui::SceneView>("Scene View", true, settings, *render_context_,
                                               scene_manager_->CurrentScene());
    panels_manager_.CreatePanel<ui::GameView>("Game View", true, settings, *render_context_,
                                              scene_manager_->CurrentScene());

    panels_manager_.CreatePanel<ui::AssetView>("Asset View", true, settings, *render_context_,
                                               scene_manager_->CurrentScene());
    panels_manager_.CreatePanel<ui::Toolbar>("Toolbar", true, settings, editor_resources_.get());
    panels_manager_.CreatePanel<ui::ProjectSettings>("Project Settings", false, settings, project_path_, project_name_);

    canvas_.MakeDockSpace(true);
    gui_->SetCanvas(canvas_);
    scene_manager_->CurrentScene()->Play();
}

// MARK: - Update
void EditorApplication::Update(float delta_time) {
    //    if (auto editorMode = editor_actions_->current_editor_mode();
    //        editorMode == EditorActions::EditorMode::PLAY ||
    //        editorMode == EditorActions::EditorMode::FRAME_BY_FRAME) {
    components_manager_->CallScriptOnStart();

    physics_manager_->Update(delta_time);

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
    GraphicsApplication::Update(delta_time);
    editor_actions_->ExecuteDelayedActions();
}

void EditorApplication::Render(CommandBuffer &command_buffer, RenderTarget &render_target) {
    UpdateGpuTask(command_buffer, render_target);
    UpdateEditorPanels(delta_time_);
    RenderViews(delta_time_, command_buffer);

    GraphicsApplication::Render(command_buffer, render_target);
}

void EditorApplication::UpdateGpuTask(CommandBuffer &command_buffer, RenderTarget &render_target) {
    shadow_manager_->Draw(command_buffer);
    light_manager_->Draw(command_buffer, render_target);
    particle_manager_->Draw(command_buffer, render_target);
}

void EditorApplication::UpdateEditorPanels(float delta_time) {
    auto &menu_bar = panels_manager_.GetPanelAs<ui::MenuBar>("Menu Bar");
    auto &profiler = panels_manager_.GetPanelAs<ui::ProfilerWindow>("Profiler");

    menu_bar.HandleShortcuts(delta_time);

    if (profiler.IsOpened()) {
        PROFILER_SPY("Profiler Update");
        profiler.Update(delta_time);
    }
}

void EditorApplication::RenderViews(float delta_time, CommandBuffer &command_buffer) {
    auto &game_view = panels_manager_.GetPanelAs<ui::GameView>("Game View");
    auto &scene_view = panels_manager_.GetPanelAs<ui::SceneView>("Scene View");
    auto &asset_view = panels_manager_.GetPanelAs<ui::AssetView>("Asset View");

    {
        PROFILER_SPY("Editor Views Update");
        asset_view.Update(delta_time);
        game_view.Update(delta_time);
        scene_view.Update(delta_time);
    }

    if (asset_view.IsOpened()) {
        PROFILER_SPY("Game View Rendering");
        asset_view.Render(command_buffer);
    }

    if (game_view.IsOpened()) {
        PROFILER_SPY("Game View Rendering");
        game_view.Render(command_buffer);
    }

    if (scene_view.IsOpened()) {
        PROFILER_SPY("Scene View Rendering");
        scene_view.Render(command_buffer);
    }
}

bool EditorApplication::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    GraphicsApplication::Resize(win_width, win_height, fb_width, fb_height);
    components_manager_->CallScriptResize(win_width, win_height, fb_width, fb_height);
    return true;
}

void EditorApplication::InputEvent(const vox::InputEvent &input_event) {
    GraphicsApplication::InputEvent(input_event);
    components_manager_->CallScriptInputEvent(input_event);

    auto &scene_view = panels_manager_.GetPanelAs<ui::SceneView>("Scene View");
    scene_view.InputEvent(input_event);

    if (input_event.GetSource() == EventSource::KEYBOARD) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
        if (key_event.GetCode() == KeyCode::ESCAPE) {
            editor_actions_->StopPlaying();
        }
        if (key_event.GetCode() == KeyCode::F5) {
            editor_actions_->StartPlaying();
        }
    }
}

}  // namespace vox::editor
