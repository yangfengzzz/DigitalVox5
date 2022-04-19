//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "graphics_application.h"
#include "components_manager.h"
#include "physics/physics_manager.h"
//#include "shadow/shadow_manager.h"
#include "lighting/light_manager.h"
//#include "particle/particle_manager.h"
#include "scene_manager.h"

namespace vox {
class ForwardApplication: public GraphicsApplication {
public:
    ForwardApplication() = default;
    
    virtual ~ForwardApplication();
    
    /**
     * @brief Additional sample initialization
     */
    bool prepare(Platform &platform) override;
    
    virtual void loadScene() = 0;
    
protected:
    Camera* _mainCamera{nullptr};
    
    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<ComponentsManager> _componentsManager{nullptr};
    std::unique_ptr<physics::PhysicsManager> _physicsManager{nullptr};
    std::unique_ptr<SceneManager> _sceneManager{nullptr};
//    std::unique_ptr<ShadowManager> _shadowManager{nullptr};
    std::unique_ptr<LightManager> _lightManager{nullptr};
//    std::unique_ptr<ParticleManager> _particleManager{nullptr};
};

}
