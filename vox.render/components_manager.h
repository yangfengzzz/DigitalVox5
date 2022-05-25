//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_map>
#include <vector>

#include "vox.base/singleton.h"
#include "vox.math/bounding_frustum.h"
#include "vox.math/matrix4x4.h"
#include "vox.render/platform/input_events.h"
#include "vox.render/rendering/render_element.h"
#include "vox.render/scene_forward.h"

namespace vox {
/**
 * The manager of the components.
 */
class ComponentsManager : public Singleton<ComponentsManager> {
public:
    static ComponentsManager &GetSingleton();

    static ComponentsManager *GetSingletonPtr();

    void AddOnStartScript(Script *script);

    void RemoveOnStartScript(Script *script);

    void AddOnUpdateScript(Script *script);

    void RemoveOnUpdateScript(Script *script);

    void AddDestroyComponent(Script *component);

    void CallScriptOnStart();

    void CallScriptOnUpdate(float delta_time);

    void CallScriptOnLateUpdate(float delta_time);

    void CallScriptInputEvent(const InputEvent &input_event);

    void CallScriptResize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height);

    void CallComponentDestroy();

public:
    void AddRenderer(Renderer *renderer);

    void RemoveRenderer(Renderer *renderer);

    void CallRendererOnUpdate(float delta_time);

    void CallRender(Camera *camera,
                    std::vector<RenderElement> &opaque_queue,
                    std::vector<RenderElement> &alpha_test_queue,
                    std::vector<RenderElement> &transparent_queue);

    void CallRender(const BoundingFrustum &frustum,
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
    void AddOnUpdateSceneAnimators(SceneAnimator *animator);

    void RemoveOnUpdateSceneAnimators(SceneAnimator *animator);

    void CallSceneAnimatorUpdate(float delta_time);

public:
    static void CallCameraOnBeginRender(Camera *camera);

    static void CallCameraOnEndRender(Camera *camera);

    std::vector<Component *> GetActiveChangedTempList();

    void PutActiveChangedTempList(std::vector<Component *> &component_container);

private:
    // Script
    std::vector<Script *> on_start_scripts_;
    std::vector<Script *> on_update_scripts_;
    std::vector<Script *> destroy_components_;

    // Render
    std::vector<Renderer *> renderers_;

    // Delay dispose active/inActive Pool
    std::vector<std::vector<Component *>> components_container_pool_;

    // Animator
    //    std::vector<Animator *> _onUpdateAnimators;
    std::vector<SceneAnimator *> on_update_scene_animators_;
};

template <>
inline ComponentsManager *Singleton<ComponentsManager>::ms_singleton{nullptr};

}  // namespace vox
