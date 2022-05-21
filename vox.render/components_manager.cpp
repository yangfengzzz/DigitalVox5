//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "components_manager.h"
#include "script.h"
#include "renderer.h"
#include "camera.h"
//#include "animator.h"
#include "scene_animator.h"
#include "logging.h"

namespace vox {
ComponentsManager *ComponentsManager::GetSingletonPtr() {
    return ms_singleton_;
}

ComponentsManager &ComponentsManager::GetSingleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}

void ComponentsManager::add_on_start_script(Script *script) {
    auto iter = std::find(on_start_scripts_.begin(), on_start_scripts_.end(), script);
    if (iter == on_start_scripts_.end()) {
        on_start_scripts_.push_back(script);
    } else {
        LOGE("Script already attached.")
    }
}

void ComponentsManager::remove_on_start_script(Script *script) {
    auto iter = std::find(on_start_scripts_.begin(), on_start_scripts_.end(), script);
    if (iter != on_start_scripts_.end()) {
        on_start_scripts_.erase(iter);
    }
}

void ComponentsManager::add_on_update_script(Script *script) {
    auto iter = std::find(on_update_scripts_.begin(), on_update_scripts_.end(), script);
    if (iter == on_update_scripts_.end()) {
        on_update_scripts_.push_back(script);
    } else {
        LOGE("Script already attached.")
    }
}

void ComponentsManager::remove_on_update_script(Script *script) {
    auto iter = std::find(on_update_scripts_.begin(), on_update_scripts_.end(), script);
    if (iter != on_update_scripts_.end()) {
        on_update_scripts_.erase(iter);
    }
}

void ComponentsManager::add_destroy_component(Script *component) {
    destroy_components_.push_back(component);
}

void ComponentsManager::call_component_destroy() {
    if (!destroy_components_.empty()) {
        for (auto &destroy_component : destroy_components_) {
            destroy_component->on_destroy();
        }
        destroy_components_.clear();
    }
}

void ComponentsManager::call_script_on_start() {
    if (!on_start_scripts_.empty()) {
        // The 'onStartScripts.length' maybe add if you add some Script with addComponent() in some Script's on_start()
        for (auto &script : on_start_scripts_) {
            script->set_is_started(true);
            script->on_start();
        }
        on_start_scripts_.clear();
    }
}

void ComponentsManager::call_script_on_update(float delta_time) {
    for (auto &element : on_update_scripts_) {
        if (element->is_started()) {
            element->on_update(delta_time);
        }
    }
}

void ComponentsManager::call_script_on_late_update(float delta_time) {
    for (auto &element : on_update_scripts_) {
        if (element->is_started()) {
            element->on_late_update(delta_time);
        }
    }
}

void ComponentsManager::call_script_input_event(const InputEvent &input_event) {
    for (auto &element : on_update_scripts_) {
        if (element->is_started()) {
            element->input_event(input_event);
        }
    }
}

void ComponentsManager::call_script_resize(uint32_t win_width, uint32_t win_height,
                                           uint32_t fb_width, uint32_t fb_height) {
    for (auto &element : on_update_scripts_) {
        if (element->is_started()) {
            element->resize(win_width, win_height, fb_width, fb_height);
        }
    }
}

//MARK: - Renderer
void ComponentsManager::add_renderer(Renderer *renderer) {
    auto iter = std::find(renderers_.begin(), renderers_.end(), renderer);
    if (iter == renderers_.end()) {
        renderers_.push_back(renderer);
    } else {
        LOGE("Renderer already attached.")
    }
}

void ComponentsManager::remove_renderer(Renderer *renderer) {
    auto iter = std::find(renderers_.begin(), renderers_.end(), renderer);
    if (iter != renderers_.end()) {
        renderers_.erase(iter);
    }
}

void ComponentsManager::call_renderer_on_update(float delta_time) {
    for (auto &renderer : renderers_) {
        renderer->update(delta_time);
    }
}

void ComponentsManager::call_render(Camera *camera,
                                    std::vector<RenderElement> &opaque_queue,
                                    std::vector<RenderElement> &alpha_test_queue,
                                    std::vector<RenderElement> &transparent_queue) {
    for (auto &element : renderers_) {
        // filter by camera culling mask.
        if (!(camera->culling_mask_ & element->entity_->layer_)) {
            continue;
        }
        
        // filter by camera frustum.
        if (camera->enable_frustum_culling_) {
            element->is_culled_ = !camera->frustum().intersectsBox(element->bounds());
            if (element->is_culled_) {
                continue;
            }
        }
        
        const auto &transform = camera->entity()->transform_;
        const auto kPosition = transform->world_position();
        auto center = element->bounds().midPoint();
        if (camera->is_orthographic()) {
            const auto kForward = transform->world_forward();
            const auto kOffset = center - kPosition;
            element->set_distance_for_sort(kOffset.dot(kForward));
        } else {
            element->set_distance_for_sort(center.distanceSquaredTo(kPosition));
        }
        
        element->render(opaque_queue, alpha_test_queue, transparent_queue);
    }
}

void ComponentsManager::call_render(const BoundingFrustum &frustum,
                                    std::vector<RenderElement> &opaque_queue,
                                    std::vector<RenderElement> &alpha_test_queue,
                                    std::vector<RenderElement> &transparent_queue) {
    for (auto &renderer : renderers_) {
        // filter by renderer castShadow and frustum cull
        if (frustum.intersectsBox(renderer->bounds())) {
            renderer->render(opaque_queue, alpha_test_queue, transparent_queue);
        }
    }
}

//MARK: - Camera
void ComponentsManager::call_camera_on_begin_render(Camera *camera) {
    const auto &cam_comps = camera->entity()->scripts();
    for (auto cam_comp : cam_comps) {
        cam_comp->on_begin_render(camera);
    }
}

void ComponentsManager::call_camera_on_end_render(Camera *camera) {
    const auto &cam_comps = camera->entity()->scripts();
    for (auto cam_comp : cam_comps) {
        cam_comp->on_end_render(camera);
    }
}

std::vector<Component *> ComponentsManager::get_active_changed_temp_list() {
    return !components_container_pool_.empty() ? *(components_container_pool_.end() - 1) : std::vector<Component *>{};
}

void ComponentsManager::put_active_changed_temp_list(std::vector<Component *> &component_container) {
    component_container.clear();
    components_container_pool_.push_back(component_container);
}

//MARK: - Animation
//void ComponentsManager::addOnUpdateAnimators(Animator *animator) {
//    auto iter = std::find(_onUpdateAnimators.begin(), _onUpdateAnimators.end(), animator);
//    if (iter == _onUpdateAnimators.end()) {
//        _onUpdateAnimators.push_back(animator);
//    } else {
//        LOGE("Animator already attached.")
//    }
//}
//
//void ComponentsManager::removeOnUpdateAnimators(Animator *animator) {
//    auto iter = std::find(_onUpdateAnimators.begin(), _onUpdateAnimators.end(), animator);
//    if (iter != _onUpdateAnimators.end()) {
//        _onUpdateAnimators.erase(iter);
//    }
//}
//
//void ComponentsManager::callAnimatorUpdate(float deltaTime) {
//    const auto &elements = _onUpdateAnimators;
//    for (size_t i = 0; i < _onUpdateAnimators.size(); i++) {
//        elements[i]->update(deltaTime);
//    }
//}

void ComponentsManager::add_on_update_scene_animators(SceneAnimator *animator) {
    auto iter = std::find(on_update_scene_animators_.begin(), on_update_scene_animators_.end(), animator);
    if (iter == on_update_scene_animators_.end()) {
        on_update_scene_animators_.push_back(animator);
    } else {
        LOGE("SceneAnimator already attached.")
    }
}

void ComponentsManager::remove_on_update_scene_animators(SceneAnimator *animator) {
    auto iter = std::find(on_update_scene_animators_.begin(), on_update_scene_animators_.end(), animator);
    if (iter != on_update_scene_animators_.end()) {
        on_update_scene_animators_.erase(iter);
    }
}

void ComponentsManager::call_scene_animator_update(float delta_time) {
    const auto &elements = on_update_scene_animators_;
    for (size_t i = 0; i < on_update_scene_animators_.size(); i++) {
        elements[i]->update(delta_time);
    }
}

}        // namespace vox
