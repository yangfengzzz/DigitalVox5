//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/forward_application.h"

#include "vox.render/camera.h"
#include "vox.render/platform/platform.h"
#include "vox.render/rendering/subpasses/geometry_subpass.h"

namespace vox {
ForwardApplication::~ForwardApplication() {
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

bool ForwardApplication::Prepare(Platform &platform) {
    GraphicsApplication::Prepare(platform);

    // resource loader
    texture_manager_ = std::make_unique<TextureManager>(*device_);
    shader_manager_ = std::make_unique<ShaderManager>();
    mesh_manager_ = std::make_unique<MeshManager>(*device_);

    // logic system
    components_manager_ = std::make_unique<ComponentsManager>();
    physics_manager_ = std::make_unique<physics::PhysicsManager>();
    scene_manager_ = std::make_unique<SceneManager>(*device_);
    auto scene = scene_manager_->CurrentScene();

    particle_manager_ = std::make_unique<ParticleManager>(*device_, *render_context_);
    light_manager_ = std::make_unique<LightManager>(scene, *render_context_);
    {
        LoadScene();
        auto extent = platform.GetWindow().GetExtent();
        auto factor = static_cast<uint32_t>(platform.GetWindow().GetContentScaleFactor());
        components_manager_->CallScriptResize(extent.width, extent.height, factor * extent.width,
                                              factor * extent.height);
        main_camera_->Resize(extent.width, extent.height, factor * extent.width, factor * extent.height);
    }
    light_manager_->SetCamera(main_camera_);

    // internal manager
    shadow_manager_ = std::make_unique<ShadowManager>(*device_, *render_context_, scene, main_camera_);

    // default render pipeline
    std::vector<std::unique_ptr<Subpass>> scene_subpasses{};
    scene_subpasses.emplace_back(std::make_unique<GeometrySubpass>(GetRenderContext(), scene, main_camera_));
    SetRenderPipeline(RenderPipeline(std::move(scene_subpasses)));

    return true;
}

void ForwardApplication::Update(float delta_time) {
    {
        components_manager_->CallScriptOnStart();

        physics_manager_->Update(delta_time);

        components_manager_->CallScriptOnUpdate(delta_time);
        //        _componentsManager->callAnimatorUpdate(deltaTime);
        components_manager_->CallSceneAnimatorUpdate(delta_time);
        components_manager_->CallScriptOnLateUpdate(delta_time);

        components_manager_->CallRendererOnUpdate(delta_time);
        scene_manager_->CurrentScene()->UpdateShaderData();
    }

    GraphicsApplication::Update(delta_time);
}

bool ForwardApplication::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    GraphicsApplication::Resize(win_width, win_height, fb_width, fb_height);
    components_manager_->CallScriptResize(win_width, win_height, fb_width, fb_height);
    main_camera_->Resize(win_width, win_height, fb_width, fb_height);
    return true;
}

void ForwardApplication::InputEvent(const vox::InputEvent &input_event) {
    GraphicsApplication::InputEvent(input_event);
    components_manager_->CallScriptInputEvent(input_event);
}

void ForwardApplication::Render(CommandBuffer &command_buffer, RenderTarget &render_target) {
    UpdateGpuTask(command_buffer, render_target);
    GraphicsApplication::Render(command_buffer, render_target);
}

void ForwardApplication::UpdateGpuTask(CommandBuffer &command_buffer, RenderTarget &render_target) {
    shadow_manager_->Draw(command_buffer);
    light_manager_->Draw(command_buffer, render_target);
    particle_manager_->Draw(command_buffer, render_target);
}

}  // namespace vox
