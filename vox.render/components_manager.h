//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "matrix4x4.h"
#include "bounding_frustum.h"
#include "platform/input_events.h"
#include "scene_forward.h"
#include "rendering/render_element.h"
#include "singleton.h"
#include <unordered_map>
#include <vector>

namespace vox {
/**
 * The manager of the components.
 */
class ComponentsManager : public Singleton<ComponentsManager> {
public:
    static ComponentsManager &GetSingleton();
    
    static ComponentsManager *GetSingletonPtr();
    
    void add_on_start_script(Script *script);
    
    void remove_on_start_script(Script *script);
    
    void add_on_update_script(Script *script);
    
    void remove_on_update_script(Script *script);
    
    void add_destroy_component(Script *component);
    
    void call_script_on_start();
    
    void call_script_on_update(float delta_time);
    
    void call_script_on_late_update(float delta_time);
    
    void call_script_input_event(const InputEvent &input_event);
    
    void call_script_resize(uint32_t win_width, uint32_t win_height,
                            uint32_t fb_width, uint32_t fb_height);
    
    void call_component_destroy();
    
public:
    void add_renderer(Renderer *renderer);
    
    void remove_renderer(Renderer *renderer);
    
    void call_renderer_on_update(float delta_time);
    
    void call_render(Camera *camera,
                     std::vector<RenderElement> &opaque_queue,
                     std::vector<RenderElement> &alpha_test_queue,
                     std::vector<RenderElement> &transparent_queue);
    
    void call_render(const BoundingFrustum &frustum,
                     std::vector<RenderElement> &opaque_queue,
                     std::vector<RenderElement> &alpha_test_queue,
                     std::vector<RenderElement> &transparent_queue);
    
public:
    //    void addOnUpdateAnimators(Animator *animator);
    //
    //    void removeOnUpdateAnimators(Animator *animator);
    //
    //    void callAnimatorUpdate(float deltaTime);
    //
    void add_on_update_scene_animators(SceneAnimator *animator);
    
    void remove_on_update_scene_animators(SceneAnimator *animator);
    
    void call_scene_animator_update(float delta_time);
    
public:
    static void call_camera_on_begin_render(Camera *camera);
    
    static void call_camera_on_end_render(Camera *camera);
    
    std::vector<Component *> get_active_changed_temp_list();
    
    void put_active_changed_temp_list(std::vector<Component *> &component_container);
    
private:
    // Script
    std::vector<Script *> on_start_scripts_;
    std::vector<Script *> on_update_scripts_;
    std::vector<Script *> destroy_components_;
    
    // Render
    std::vector<Renderer *> renderers_;
    
    // Delay dispose active/inActive Pool
    std::vector<std::vector<Component *>> components_container_pool_;
    
    // Animatior
    //    std::vector<Animator *> _onUpdateAnimators;
    std::vector<SceneAnimator *> on_update_scene_animators_;
};

template<> inline ComponentsManager *Singleton<ComponentsManager>::ms_singleton_{nullptr};

}        // namespace vox
