//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "component.h"
#include "scene_animation_clip.h"
#include <vector>
#include <string>

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
    void on_serialize(nlohmann::json &data) override;
    
    /**
     * Called when the deserialization is asked
     */
    void on_deserialize(const nlohmann::json &data) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void on_inspector(ui::WidgetContainer &p_root) override;
    
private:
    void on_enable() override;
    
    void on_disable() override;
    
private:
    ssize_t active_animation_ = -1;
    std::vector<std::unique_ptr<SceneAnimationClip>> animation_clips_;
};

}
