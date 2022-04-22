//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_application.h"
#include "rendering/subpasses/geometry_subpass.h"
#include "platform/platform.h"
#include "camera.h"

namespace vox {
ForwardApplication::~ForwardApplication() {
    // release first
    scene_manager_.reset();
    
    components_manager_.reset();
    physics_manager_.reset();
    light_manager_.reset();
    //    _shadowManager.reset();
    //    _particleManager.reset();
}

bool ForwardApplication::prepare(Platform &platform) {
    GraphicsApplication::prepare(platform);
    
    image_manager_ = std::make_unique<ImageManager>(*device_);
    shader_manager_ = std::make_unique<ShaderManager>();
    mesh_manager_ = std::make_unique<MeshManager>(*device_);
    
    components_manager_ = std::make_unique<ComponentsManager>();
    physics_manager_ = std::make_unique<physics::PhysicsManager>();
    scene_manager_ = std::make_unique<SceneManager>(*device_);
    auto scene = scene_manager_->current_scene();
    
    //    _particleManager = std::make_unique<ParticleManager>(_device);
    light_manager_ = std::make_unique<LightManager>(scene);
    {
        load_scene();
        auto extent = platform.get_window().get_extent();
        auto factor = static_cast<uint32_t>(platform.get_window().get_content_scale_factor());
        components_manager_->call_script_resize(extent.width, extent.height, factor * extent.width, factor * extent.height);
        main_camera_->resize(extent.width, extent.height, factor * extent.width, factor * extent.height);
    }
    light_manager_->set_camera(main_camera_);
    //    _shadowManager = std::make_unique<ShadowManager>(scene, _mainCamera);
    
    std::vector<std::unique_ptr<Subpass>> scene_subpasses{};
    scene_subpasses.emplace_back(std::make_unique<GeometrySubpass>(get_render_context(), scene, main_camera_));
    set_render_pipeline(RenderPipeline(std::move(scene_subpasses)));
    
    return true;
}

void ForwardApplication::update(float delta_time) {
    {
        components_manager_->call_script_on_start();
        
        physics_manager_->update(delta_time);
        
        components_manager_->call_script_on_update(delta_time);
        //        _componentsManager->callAnimatorUpdate(deltaTime);
        components_manager_->call_scene_animator_update(delta_time);
        components_manager_->call_script_on_late_update(delta_time);
        
        components_manager_->call_renderer_on_update(delta_time);
        scene_manager_->current_scene()->update_shader_data();
    }
    
    GraphicsApplication::update(delta_time);
}

bool ForwardApplication::resize(uint32_t win_width, uint32_t win_height,
                                uint32_t fb_width, uint32_t fb_height) {
    GraphicsApplication::resize(win_width, win_height, fb_width, fb_height);
    components_manager_->call_script_resize(win_width, win_height, fb_width, fb_height);
    main_camera_->resize(win_width, win_height, fb_width, fb_height);
    return true;
}

void ForwardApplication::input_event(const InputEvent &input_event) {
    GraphicsApplication::input_event(input_event);
    components_manager_->call_script_input_event(input_event);
}

void ForwardApplication::render(CommandBuffer &command_buffer) {
    light_manager_->draw(command_buffer);
    GraphicsApplication::render(command_buffer);
}

}
