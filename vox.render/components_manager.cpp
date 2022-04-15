//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "components_manager.h"
#include "script.h"
//#include "renderer.h"
#include "entity.h"
#include "camera.h"
//#include "animator.h"
//#include "scene_animator.h"
#include "logging.h"

namespace vox {
ComponentsManager *ComponentsManager::get_singleton_ptr() {
    return ms_singleton_;
}

ComponentsManager &ComponentsManager::get_singleton() {
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
//void ComponentsManager::addRenderer(Renderer *renderer) {
//    auto iter = std::find(_renderers.begin(), _renderers.end(), renderer);
//    if (iter == _renderers.end()) {
//        _renderers.push_back(renderer);
//    } else {
//        LOG(ERROR) << "Renderer already attached." << std::endl;;
//    }
//}
//
//void ComponentsManager::removeRenderer(Renderer *renderer) {
//    auto iter = std::find(_renderers.begin(), _renderers.end(), renderer);
//    if (iter != _renderers.end()) {
//        _renderers.erase(iter);
//    }
//}
//
//void ComponentsManager::callRendererOnUpdate(float deltaTime) {
//    for (size_t i = 0; i < _renderers.size(); i++) {
//        _renderers[i]->update(deltaTime);
//    }
//}
//
//void ComponentsManager::callRender(Camera *camera,
//                                   std::vector<RenderElement> &opaqueQueue,
//                                   std::vector<RenderElement> &alphaTestQueue,
//                                   std::vector<RenderElement> &transparentQueue) {
//    for (size_t i = 0; i < _renderers.size(); i++) {
//        const auto &element = _renderers[i];
//
//        // filter by camera culling mask.
//        if (!(camera->cullingMask & element->_entity->layer)) {
//            continue;
//        }
//
//        // filter by camera frustum.
//        if (camera->enableFrustumCulling) {
//            element->isCulled = !camera->frustum().intersectsBox(element->bounds());
//            if (element->isCulled) {
//                continue;
//            }
//        }
//
//        const auto &transform = camera->entity()->transform;
//        const auto position = transform->worldPosition();
//        auto center = element->bounds().midPoint();
//        if (camera->is_orthographic()) {
//            const auto forward = transform->worldForward();
//            const auto offset = center - position;
//            element->setDistanceForSort(offset.dot(forward));
//        } else {
//            element->setDistanceForSort(center.distanceSquaredTo(position));
//        }
//
//        element->_render(opaqueQueue, alphaTestQueue, transparentQueue);
//    }
//}
//
//void ComponentsManager::callRender(const BoundingFrustum &frustrum,
//                                   std::vector<RenderElement> &opaqueQueue,
//                                   std::vector<RenderElement> &alphaTestQueue,
//                                   std::vector<RenderElement> &transparentQueue) {
//    for (size_t i = 0; i < _renderers.size(); i++) {
//        const auto &renderer = _renderers[i];
//        // filter by renderer castShadow and frustrum cull
//        if (frustrum.intersectsBox(renderer->bounds())) {
//            renderer->_render(opaqueQueue, alphaTestQueue, transparentQueue);
//        }
//    }
//}

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
//        LOG(ERROR) << "Animator already attached." << std::endl;;
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
//
//void ComponentsManager::addOnUpdateSceneAnimators(SceneAnimator *animator) {
//    auto iter = std::find(_onUpdateSceneAnimators.begin(), _onUpdateSceneAnimators.end(), animator);
//    if (iter == _onUpdateSceneAnimators.end()) {
//        _onUpdateSceneAnimators.push_back(animator);
//    } else {
//        LOG(ERROR) << "SceneAnimator already attached." << std::endl;;
//    }
//}
//
//void ComponentsManager::removeOnUpdateSceneAnimators(SceneAnimator *animator) {
//    auto iter = std::find(_onUpdateSceneAnimators.begin(), _onUpdateSceneAnimators.end(), animator);
//    if (iter != _onUpdateSceneAnimators.end()) {
//        _onUpdateSceneAnimators.erase(iter);
//    }
//}
//
//void ComponentsManager::callSceneAnimatorUpdate(float deltaTime) {
//    const auto &elements = _onUpdateSceneAnimators;
//    for (size_t i = 0; i < _onUpdateSceneAnimators.size(); i++) {
//        elements[i]->update(deltaTime);
//    }
//}

}        // namespace vox
