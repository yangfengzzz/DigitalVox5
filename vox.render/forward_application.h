//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/components_manager.h"
#include "vox.render/graphics_application.h"
#include "vox.render/lighting/light_manager.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/particle/particle_manager.h"
#include "vox.render/physics/physics_manager.h"
#include "vox.render/scene_manager.h"
#include "vox.render/shader/shader_manager.h"
#include "vox.render/shadow/shadow_manager.h"
#include "vox.render/texture_manager.h"

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

    bool Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) override;

    void InputEvent(const vox::InputEvent &input_event) override;

    void Render(CommandBuffer &command_buffer, RenderTarget &render_target) override;

    virtual void LoadScene() = 0;

    virtual void UpdateGpuTask(CommandBuffer &command_buffer, RenderTarget &render_target);

protected:
    Camera *main_camera_{nullptr};

    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<TextureManager> texture_manager_{nullptr};
    std::unique_ptr<ShaderManager> shader_manager_{nullptr};
    std::unique_ptr<MeshManager> mesh_manager_{nullptr};

    std::unique_ptr<ComponentsManager> components_manager_{nullptr};
    std::unique_ptr<physics::PhysicsManager> physics_manager_{nullptr};
    std::unique_ptr<SceneManager> scene_manager_{nullptr};
    std::unique_ptr<ShadowManager> shadow_manager_{nullptr};
    std::unique_ptr<LightManager> light_manager_{nullptr};
    std::unique_ptr<ParticleManager> particle_manager_{nullptr};
};

}  // namespace vox
