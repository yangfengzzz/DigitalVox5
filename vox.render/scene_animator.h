//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <vector>

#include "vox.render/component.h"
#include "vox.render/scene_animation_clip.h"

namespace vox {
class SceneAnimator : public Component {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit SceneAnimator(Entity *entity);

    void update(float delta_time);

    void add_animation_clip(std::unique_ptr<SceneAnimationClip> &&clip);

    void play(const std::string &name);

public:
    /**
     * Called when the serialization is asked
     */
    void OnSerialize(nlohmann::json &data) override;

    /**
     * Called when the deserialization is asked
     */
    void OnDeserialize(const nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void OnInspector(ui::WidgetContainer &p_root) override;

private:
    void OnEnable() override;

    void OnDisable() override;

private:
    ssize_t active_animation_ = -1;
    std::vector<std::unique_ptr<SceneAnimationClip>> animation_clips_;
};

}  // namespace vox
