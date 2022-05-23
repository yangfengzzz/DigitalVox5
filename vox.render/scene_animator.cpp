//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/scene_animator.h"

#include "vox.render/components_manager.h"
#include "vox.render/entity.h"
#include "vox.render/scene.h"

namespace vox {
std::string SceneAnimator::name() { return "SceneAnimator"; }

SceneAnimator::SceneAnimator(Entity *entity) : Component(entity) {}

void SceneAnimator::update(float delta_time) {
    if (active_animation_ != -1) {
        animation_clips_[active_animation_]->update(delta_time);
    }
}

void SceneAnimator::add_animation_clip(std::unique_ptr<SceneAnimationClip> &&clip) {
    animation_clips_.emplace_back(std::move(clip));
}

void SceneAnimator::play(const std::string &name) {
    auto iter = std::find_if(animation_clips_.begin(), animation_clips_.end(),
                             [&](const auto &u) { return u->name() == name; });
    if (iter != animation_clips_.end()) {
        active_animation_ = iter - animation_clips_.begin();
    } else {
        active_animation_ = -1;
    }
}

void SceneAnimator::OnEnable() { ComponentsManager::GetSingleton().AddOnUpdateSceneAnimators(this); }

void SceneAnimator::OnDisable() { ComponentsManager::GetSingleton().RemoveOnUpdateSceneAnimators(this); }

// MARK: - Reflection
void SceneAnimator::OnSerialize(nlohmann::json &data) {}

void SceneAnimator::OnDeserialize(const nlohmann::json &data) {}

void SceneAnimator::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
