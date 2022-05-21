//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "graphics_application.h"
#include "image_manager.h"
#include "shader/shader_manager.h"
#include "mesh/mesh_manager.h"

#include "components_manager.h"
#include "physics/physics_manager.h"
#include "shadow/shadow_manager.h"
#include "lighting/light_manager.h"
#include "particle/particle_manager.h"
#include "scene_manager.h"

namespace vox {
class ForwardApplication : public GraphicsApplication {
public:
    ForwardApplication() = default;
    
    virtual ~ForwardApplication();
    
    /**
     * @brief Additional sample initialization
     */
    bool Prepare(Platform &platform) override;
    
    /**
     * @brief Main loop sample events
     */
    void Update(float delta_time) override;
    
    bool Resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;
    
    void InputEvent(const vox::InputEvent &input_event) override;
    
    void render(CommandBuffer &command_buffer, RenderTarget &render_target) override;
    
    virtual void load_scene() = 0;
    
    virtual void update_gpu_task(CommandBuffer &command_buffer, RenderTarget &render_target);
    
protected:
    Camera *main_camera_{nullptr};
    
    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<ImageManager> image_manager_{nullptr};
    std::unique_ptr<ShaderManager> shader_manager_{nullptr};
    std::unique_ptr<MeshManager> mesh_manager_{nullptr};
    
    std::unique_ptr<ComponentsManager> components_manager_{nullptr};
    std::unique_ptr<physics::PhysicsManager> physics_manager_{nullptr};
    std::unique_ptr<SceneManager> scene_manager_{nullptr};
    std::unique_ptr<ShadowManager> shadow_manager_{nullptr};
    std::unique_ptr<LightManager> light_manager_{nullptr};
    std::unique_ptr<ParticleManager> particle_manager_{nullptr};
};

}
