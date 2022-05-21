//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#include "scene_animator.h"
#include "entity.h"
#include "scene.h"
#include "components_manager.h"

namespace vox {
std::string SceneAnimator::name() {
    return "SceneAnimator";
}

SceneAnimator::SceneAnimator(Entity *entity) :
Component(entity) {
}

void SceneAnimator::update(float delta_time) {
    if (active_animation_ != -1) {
        animation_clips_[active_animation_]->update(delta_time);
    }
}

void SceneAnimator::add_animation_clip(std::unique_ptr<SceneAnimationClip> &&clip) {
    animation_clips_.emplace_back(std::move(clip));
}

void SceneAnimator::play(const std::string &name) {
    auto iter = std::find_if(animation_clips_.begin(), animation_clips_.end(), [&](const auto &u) {
        return u->name() == name;
    });
    if (iter != animation_clips_.end()) {
        active_animation_ = iter - animation_clips_.begin();
    } else {
        active_animation_ = -1;
    }
}

void SceneAnimator::on_enable() {
    ComponentsManager::GetSingleton().add_on_update_scene_animators(this);
}

void SceneAnimator::on_disable() {
    ComponentsManager::GetSingleton().remove_on_update_scene_animators(this);
}

//MARK: - Reflection
void SceneAnimator::on_serialize(nlohmann::json &data) {
    
}

void SceneAnimator::on_deserialize(const nlohmann::json &data) {
    
}

void SceneAnimator::on_inspector(ui::WidgetContainer &p_root) {
    
}

}
