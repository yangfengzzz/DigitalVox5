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

#include "panels_manager.h"
#include "editor_actions.h"
#include "editor_resources.h"

#include "lua/script_interpreter.h"

namespace vox::editor {
class EditorApplication : public GraphicsApplication {
public:
    EditorApplication(const std::string &project_path, const std::string &project_name);
    
    ~EditorApplication() override;
    
    /**
     * @brief Additional sample initialization
     */
    bool prepare(Platform &platform) override;
    
    /**
     * @brief Main loop sample events
     */
    void update(float delta_time) override;
    
    bool resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;
    
    void input_event(const InputEvent &input_event) override;
    
    void render(CommandBuffer &command_buffer, RenderTarget &render_target) override;
    
    void update_gpu_task(CommandBuffer &command_buffer, RenderTarget &render_target);
    
    /**
     * Handle panels creation and canvas binding
     */
    void setup_ui();
    
    /**
     * Render every views (Scene View, Game View, Asset View)
     */
    void render_views(float delta_time, CommandBuffer &command_buffer);
    
    /**
     * Update editor panels
     * @param delta_time deltaTime
     */
    void update_editor_panels(float delta_time);
    
private:
    friend class EditorActions;
    const std::string project_path_;
    const std::string project_name_;
    const std::string project_file_path_;
    const std::string engine_assets_path_;
    const std::string project_assets_path_;
    const std::string project_scripts_path_;
    const std::string editor_assets_path_;
    
    ::vox::ui::Canvas canvas_;
    ui::PanelsManager panels_manager_;
    std::unique_ptr<EditorActions> editor_actions_{nullptr};
    std::unique_ptr<EditorResources> editor_resources_{nullptr};
    
    float delta_time_{};
    Camera *main_camera_{nullptr};
    
    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<ImageManager> image_manager_{nullptr};
    std::unique_ptr<ShaderManager> shader_manager_{nullptr};
    std::unique_ptr<MeshManager> mesh_manager_{nullptr};
    std::unique_ptr<ScriptInterpreter> script_interpreter_{nullptr};

    std::unique_ptr<ComponentsManager> components_manager_{nullptr};
    std::unique_ptr<physics::PhysicsManager> physics_manager_{nullptr};
    std::unique_ptr<SceneManager> scene_manager_{nullptr};
    std::unique_ptr<ShadowManager> shadow_manager_{nullptr};
    std::unique_ptr<LightManager> light_manager_{nullptr};
    std::unique_ptr<ParticleManager> particle_manager_{nullptr};
};

}
