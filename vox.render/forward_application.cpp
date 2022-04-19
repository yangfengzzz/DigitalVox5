//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_application.h"
#include "rendering/subpasses/geometry_subpass.h"
#include "platform.h"
#include "camera.h"

namespace vox {
ForwardApplication::~ForwardApplication() {
    // release first
    _sceneManager.reset();

    _componentsManager.reset();
    _physicsManager.reset();
    _lightManager.reset();
//    _shadowManager.reset();
//    _particleManager.reset();
}

bool ForwardApplication::prepare(Platform &platform) {
    GraphicsApplication::prepare(platform);
    
    _componentsManager = std::make_unique<ComponentsManager>();
    _physicsManager = std::make_unique<physics::PhysicsManager>();
    _sceneManager = std::make_unique<SceneManager>(*device_);
    auto scene = _sceneManager->current_scene();
    
//    _particleManager = std::make_unique<ParticleManager>(_device);
    _lightManager = std::make_unique<LightManager>(scene);
    {
        loadScene();
        auto extent = platform.get_window().get_extent();
        auto factor = platform.get_window().get_content_scale_factor();
        _componentsManager->call_script_resize(extent.width, extent.height, factor * extent.width, factor * extent.height);
        _mainCamera->resize(extent.width, extent.height, factor * extent.width, factor * extent.height);
    }
    
    return true;
}

}
