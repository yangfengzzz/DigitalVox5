//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "vox.render/entity.h"

namespace vox {
class SceneAnimationClip {
public:
    struct AnimationChannel {
        enum PathType { TRANSLATION, ROTATION, SCALE };
        PathType path;
        Entity *node;
        uint32_t sampler_index;
    };

    struct AnimationSampler {
        enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
        InterpolationType interpolation;
        std::vector<float> inputs;
        std::vector<Vector4F> outputs_vec4;
    };

public:
    explicit SceneAnimationClip(std::string name);

    void update(float delta_time);

    [[nodiscard]] const std::string &name() const;

    [[nodiscard]] float start() const;

    void set_start(float time);

    [[nodiscard]] float end() const;

    void set_end(float time);

    void add_sampler(const AnimationSampler &sampler);

    void add_channel(const AnimationChannel &channel);

private:
    std::string name_;
    std::vector<AnimationSampler> samplers_;
    std::vector<AnimationChannel> channels_;
    float start_ = std::numeric_limits<float>::max();
    float end_ = std::numeric_limits<float>::min();

    float current_time_ = 0.0f;
};

}  // namespace vox
