//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/scene_animation_clip.h"

#include <iostream>
#include <utility>

namespace vox {
SceneAnimationClip::SceneAnimationClip(std::string name) : name_(std::move(name)) {}

const std::string &SceneAnimationClip::name() const { return name_; }

void SceneAnimationClip::update(float delta_time) {
    current_time_ += delta_time;
    if (current_time_ > end_) {
        current_time_ -= end_;
    }

    for (auto &channel : channels_) {
        AnimationSampler &sampler = samplers_[channel.sampler_index];
        for (size_t i = 0; i < sampler.inputs.size() - 1; i++) {
            if (sampler.interpolation != AnimationSampler::LINEAR) {
                std::cout << "This sample only supports linear interpolations\n";
                continue;
            }

            // Get the input keyframe values for the current time stamp
            if ((current_time_ >= sampler.inputs[i]) && (current_time_ <= sampler.inputs[i + 1])) {
                float a = (current_time_ - sampler.inputs[i]) / (sampler.inputs[i + 1] - sampler.inputs[i]);
                if (channel.path == AnimationChannel::TRANSLATION) {
                    auto interp = lerp(sampler.outputs_vec4[i], sampler.outputs_vec4[i + 1], a);
                    channel.node->transform->SetPosition(Point3F(interp.x, interp.y, interp.z));
                }
                if (channel.path == AnimationChannel::ROTATION) {
                    QuaternionF q1;
                    q1.x = sampler.outputs_vec4[i].x;
                    q1.y = sampler.outputs_vec4[i].y;
                    q1.z = sampler.outputs_vec4[i].z;
                    q1.w = sampler.outputs_vec4[i].w;

                    QuaternionF q2;
                    q2.x = sampler.outputs_vec4[i + 1].x;
                    q2.y = sampler.outputs_vec4[i + 1].y;
                    q2.z = sampler.outputs_vec4[i + 1].z;
                    q2.w = sampler.outputs_vec4[i + 1].w;

                    channel.node->transform->SetRotationQuaternion(slerp(q1, q2, a).normalized());
                }
                if (channel.path == AnimationChannel::SCALE) {
                    auto interp = lerp(sampler.outputs_vec4[i], sampler.outputs_vec4[i + 1], a);
                    channel.node->transform->SetScale(Vector3F(interp.x, interp.y, interp.z));
                }
            }
        }
    }
}

float SceneAnimationClip::start() const { return start_; }

void SceneAnimationClip::set_start(float time) { start_ = time; }

float SceneAnimationClip::end() const { return end_; }

void SceneAnimationClip::set_end(float time) { end_ = time; }

void SceneAnimationClip::add_sampler(const AnimationSampler &sampler) { samplers_.push_back(sampler); }

void SceneAnimationClip::add_channel(const AnimationChannel &channel) { channels_.push_back(channel); }

}  // namespace vox
