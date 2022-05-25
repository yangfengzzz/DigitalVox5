//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/components_manager.h"

#include "vox.render/camera.h"
#include "vox.render/renderer.h"
#include "vox.render/script.h"
//#include "vox.render/animator.h"
#include "vox.base/logging.h"
#include "vox.render/scene_animator.h"

namespace vox {
ComponentsManager *ComponentsManager::GetSingletonPtr() { return ms_singleton; }

ComponentsManager &ComponentsManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

void ComponentsManager::AddOnStartScript(Script *script) {
    auto iter = std::find(on_start_scripts_.begin(), on_start_scripts_.end(), script);
    if (iter == on_start_scripts_.end()) {
        on_start_scripts_.push_back(script);
    } else {
        LOGE("Script already attached.")
    }
}

void ComponentsManager::RemoveOnStartScript(Script *script) {
    auto iter = std::find(on_start_scripts_.begin(), on_start_scripts_.end(), script);
    if (iter != on_start_scripts_.end()) {
        on_start_scripts_.erase(iter);
    }
}

void ComponentsManager::AddOnUpdateScript(Script *script) {
    auto iter = std::find(on_update_scripts_.begin(), on_update_scripts_.end(), script);
    if (iter == on_update_scripts_.end()) {
        on_update_scripts_.push_back(script);
    } else {
        LOGE("Script already attached.")
    }
}

void ComponentsManager::RemoveOnUpdateScript(Script *script) {
    auto iter = std::find(on_update_scripts_.begin(), on_update_scripts_.end(), script);
    if (iter != on_update_scripts_.end()) {
        on_update_scripts_.erase(iter);
    }
}

void ComponentsManager::AddDestroyComponent(Script *component) { destroy_components_.push_back(component); }

void ComponentsManager::CallComponentDestroy() {
    if (!destroy_components_.empty()) {
        for (auto &destroy_component : destroy_components_) {
            destroy_component->OnDestroy();
        }
        destroy_components_.clear();
    }
}

void ComponentsManager::CallScriptOnStart() {
    if (!on_start_scripts_.empty()) {
        // The 'onStartScripts.length' maybe add if you add some Script with addComponent() in some Script's on_start()
        for (auto &script : on_start_scripts_) {
            script->SetIsStarted(true);
            script->OnStart();
        }
        on_start_scripts_.clear();
    }
}

void ComponentsManager::CallScriptOnUpdate(float delta_time) {
    for (auto &element : on_update_scripts_) {
        if (element->IsStarted()) {
            element->OnUpdate(delta_time);
        }
    }
}

void ComponentsManager::CallScriptOnLateUpdate(float delta_time) {
    for (auto &element : on_update_scripts_) {
        if (element->IsStarted()) {
            element->OnLateUpdate(delta_time);
        }
    }
}

void ComponentsManager::CallScriptInputEvent(const InputEvent &input_event) {
    for (auto &element : on_update_scripts_) {
        if (element->IsStarted()) {
            element->InputEvent(input_event);
        }
    }
}

void ComponentsManager::CallScriptResize(uint32_t win_width,
                                         uint32_t win_height,
                                         uint32_t fb_width,
                                         uint32_t fb_height) {
    for (auto &element : on_update_scripts_) {
        if (element->IsStarted()) {
            element->Resize(win_width, win_height, fb_width, fb_height);
        }
    }
}

// MARK: - Renderer
void ComponentsManager::AddRenderer(Renderer *renderer) {
    auto iter = std::find(renderers_.begin(), renderers_.end(), renderer);
    if (iter == renderers_.end()) {
        renderers_.push_back(renderer);
    } else {
        LOGE("Renderer already attached.")
    }
}

void ComponentsManager::RemoveRenderer(Renderer *renderer) {
    auto iter = std::find(renderers_.begin(), renderers_.end(), renderer);
    if (iter != renderers_.end()) {
        renderers_.erase(iter);
    }
}

void ComponentsManager::CallRendererOnUpdate(float delta_time) {
    for (auto &renderer : renderers_) {
        renderer->Update(delta_time);
    }
}

void ComponentsManager::CallRender(Camera *camera,
                                   std::vector<RenderElement> &opaque_queue,
                                   std::vector<RenderElement> &alpha_test_queue,
                                   std::vector<RenderElement> &transparent_queue) {
    for (auto &element : renderers_) {
        // filter by camera culling mask.
        if (!(camera->culling_mask_ & element->entity_->layer)) {
            continue;
        }

        // filter by camera frustum.
        if (camera->enable_frustum_culling_) {
            element->is_culled_ = !camera->Frustum().intersectsBox(element->Bounds());
            if (element->is_culled_) {
                continue;
            }
        }

        const auto &transform = camera->GetEntity()->transform;
        const auto kPosition = transform->WorldPosition();
        auto center = element->Bounds().midPoint();
        if (camera->IsOrthographic()) {
            const auto kForward = transform->WorldForward();
            const auto kOffset = center - kPosition;
            element->SetDistanceForSort(kOffset.dot(kForward));
        } else {
            element->SetDistanceForSort(center.distanceSquaredTo(kPosition));
        }

        element->Render(opaque_queue, alpha_test_queue, transparent_queue);
    }
}

void ComponentsManager::CallRender(const BoundingFrustum &frustum,
                                   std::vector<RenderElement> &opaque_queue,
                                   std::vector<RenderElement> &alpha_test_queue,
                                   std::vector<RenderElement> &transparent_queue) {
    for (auto &renderer : renderers_) {
        // filter by renderer castShadow and frustum cull
        if (frustum.intersectsBox(renderer->Bounds())) {
            renderer->Render(opaque_queue, alpha_test_queue, transparent_queue);
        }
    }
}

// MARK: - Camera
void ComponentsManager::CallCameraOnBeginRender(Camera *camera) {
    const auto &cam_comps = camera->GetEntity()->Scripts();
    for (auto cam_comp : cam_comps) {
        cam_comp->OnBeginRender(camera);
    }
}

void ComponentsManager::CallCameraOnEndRender(Camera *camera) {
    const auto &cam_comps = camera->GetEntity()->Scripts();
    for (auto cam_comp : cam_comps) {
        cam_comp->OnEndRender(camera);
    }
}

std::vector<Component *> ComponentsManager::GetActiveChangedTempList() {
    return !components_container_pool_.empty() ? *(components_container_pool_.end() - 1) : std::vector<Component *>{};
}

void ComponentsManager::PutActiveChangedTempList(std::vector<Component *> &component_container) {
    component_container.clear();
    components_container_pool_.push_back(component_container);
}

// MARK: - Animation
// void ComponentsManager::addOnUpdateAnimators(Animator *animator) {
//     auto iter = std::find(_onUpdateAnimators.begin(), _onUpdateAnimators.end(), animator);
//     if (iter == _onUpdateAnimators.end()) {
//         _onUpdateAnimators.push_back(animator);
//     } else {
//         LOGE("Animator already attached.")
//     }
// }
//
// void ComponentsManager::removeOnUpdateAnimators(Animator *animator) {
//     auto iter = std::find(_onUpdateAnimators.begin(), _onUpdateAnimators.end(), animator);
//     if (iter != _onUpdateAnimators.end()) {
//         _onUpdateAnimators.erase(iter);
//     }
// }
//
// void ComponentsManager::callAnimatorUpdate(float deltaTime) {
//     const auto &elements = _onUpdateAnimators;
//     for (size_t i = 0; i < _onUpdateAnimators.size(); i++) {
//         elements[i]->update(deltaTime);
//     }
// }

void ComponentsManager::AddOnUpdateSceneAnimators(SceneAnimator *animator) {
    auto iter = std::find(on_update_scene_animators_.begin(), on_update_scene_animators_.end(), animator);
    if (iter == on_update_scene_animators_.end()) {
        on_update_scene_animators_.push_back(animator);
    } else {
        LOGE("SceneAnimator already attached.")
    }
}

void ComponentsManager::RemoveOnUpdateSceneAnimators(SceneAnimator *animator) {
    auto iter = std::find(on_update_scene_animators_.begin(), on_update_scene_animators_.end(), animator);
    if (iter != on_update_scene_animators_.end()) {
        on_update_scene_animators_.erase(iter);
    }
}

void ComponentsManager::CallSceneAnimatorUpdate(float delta_time) {
    const auto &elements = on_update_scene_animators_;
    for (size_t i = 0; i < on_update_scene_animators_.size(); i++) {
        elements[i]->update(delta_time);
    }
}

}  // namespace vox
