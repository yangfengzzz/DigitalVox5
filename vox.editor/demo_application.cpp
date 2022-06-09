//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/platform.h"

#include "vox.editor/demo_application.h"

#include "vox.editor/profiling/profiler_spy.h"
#include "vox.editor/ui/console.h"
#include "vox.editor/ui/menu_bar.h"
#include "vox.editor/view/demo_view.h"
#include "vox.render/camera.h"
#include "vox.render/rendering/subpasses/geometry_subpass.h"

namespace vox::editor {
DemoApplication::DemoApplication() : GraphicsApplication(), panels_manager_(canvas_) {}

DemoApplication::~DemoApplication() {
    // release first
    scene_manager_.reset();

    components_manager_.reset();
    physics_manager_.reset();
    light_manager_.reset();
    shadow_manager_.reset();
    particle_manager_.reset();

    texture_manager_->CollectGarbage();
    texture_manager_.reset();
    shader_manager_->CollectGarbage();
    shader_manager_.reset();
    mesh_manager_->CollectGarbage();
    mesh_manager_.reset();
}

bool DemoApplication::Prepare(Platform &platform) {
    GraphicsApplication::Prepare(platform);

    gui_->LoadFont("Ruda_Big", "Fonts/Ruda-Bold.ttf", 16);
    gui_->LoadFont("Ruda_Small", "Fonts/Ruda-Bold.ttf", 12);
    gui_->LoadFont("Ruda_Medium", "Fonts/Ruda-Bold.ttf", 14);
    gui_->UseFont("Ruda_Medium");
    gui_->SetEditorLayoutAutosaveFrequency(60.0f);
    gui_->EnableEditorLayoutSave(true);
    gui_->EnableDocking(true);

    // resource loader
    texture_manager_ = std::make_unique<TextureManager>(*device_);
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

    SetupUi();

    return true;
}

void DemoApplication::SetupUi() {
    PanelWindowSettings settings;
    settings.closable = true;
    settings.collapsable = true;
    settings.dockable = true;

    panels_manager_.CreatePanel<ui::MenuBar>("Menu Bar");
    panels_manager_.CreatePanel<ui::Console>("Console", true, settings);
    panels_manager_.CreatePanel<ui::DemoView>("Scene View", true, settings, *render_context_,
                                              scene_manager_->CurrentScene(), this);

    canvas_.MakeDockSpace(true);
    gui_->SetCanvas(canvas_);
    scene_manager_->CurrentScene()->Play();
}

// MARK: - Update
void DemoApplication::Update(float delta_time) {
    components_manager_->CallScriptOnStart();

    physics_manager_->Update(delta_time);

    components_manager_->CallScriptOnUpdate(delta_time);
    //        _componentsManager->callAnimatorUpdate(deltaTime);
    components_manager_->CallSceneAnimatorUpdate(delta_time);
    components_manager_->CallScriptOnLateUpdate(delta_time);

    components_manager_->CallRendererOnUpdate(delta_time);
    scene_manager_->CurrentScene()->UpdateShaderData();

    PROFILER_SPY("Scene garbage collection");
    texture_manager_->CollectGarbage();
    mesh_manager_->CollectGarbage();
    shader_manager_->CollectGarbage();

    delta_time_ = delta_time;
    GraphicsApplication::Update(delta_time);
}

void DemoApplication::Render(CommandBuffer &command_buffer, RenderTarget &render_target) {
    UpdateGpuTask(command_buffer, render_target);
    UpdateEditorPanels(delta_time_);
    RenderViews(delta_time_, command_buffer);

    GraphicsApplication::Render(command_buffer, render_target);
}

void DemoApplication::UpdateGpuTask(CommandBuffer &command_buffer, RenderTarget &render_target) {
    shadow_manager_->Draw(command_buffer);
    light_manager_->Draw(command_buffer, render_target);
    particle_manager_->Draw(command_buffer, render_target);
}

void DemoApplication::UpdateEditorPanels(float delta_time) {
    panels_manager_.GetPanelAs<ui::MenuBar>("Menu Bar").HandleShortcuts(delta_time);
}

void DemoApplication::RenderViews(float delta_time, CommandBuffer &command_buffer) {
    auto &scene_view = panels_manager_.GetPanelAs<ui::DemoView>("Scene View");

    {
        PROFILER_SPY("Editor Views Update");
        scene_view.Update(delta_time);
    }

    if (scene_view.IsOpened()) {
        PROFILER_SPY("Scene View Rendering");
        scene_view.Render(command_buffer);
    }
}

bool DemoApplication::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    GraphicsApplication::Resize(win_width, win_height, fb_width, fb_height);
    components_manager_->CallScriptResize(win_width, win_height, fb_width, fb_height);
    return true;
}

void DemoApplication::InputEvent(const vox::InputEvent &input_event) {
    GraphicsApplication::InputEvent(input_event);
    components_manager_->CallScriptInputEvent(input_event);

    auto &scene_view = panels_manager_.GetPanelAs<ui::DemoView>("Scene View");
    scene_view.InputEvent(input_event);
}

}  // namespace vox::editor
