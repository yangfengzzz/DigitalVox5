//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/singleton.h"
#include "vox.render/particle/particle_renderer.h"
#include "vox.render/rendering/postprocessing_computepass.h"
#include "vox.render/rendering/postprocessing_pipeline.h"

namespace vox {
class ParticleManager : public Singleton<ParticleManager> {
public:
    static constexpr uint32_t particles_kernel_group_width_ = 256;

    static constexpr uint32_t ThreadsGroupCount(uint32_t nthreads) {
        return (nthreads + particles_kernel_group_width_ - 1u) / particles_kernel_group_width_;
    }

    static constexpr uint32_t FloorParticleCount(uint32_t nparticles) {
        return particles_kernel_group_width_ * (nparticles / particles_kernel_group_width_);
    }

    static ParticleManager &GetSingleton();

    static ParticleManager *GetSingletonPtr();

    ParticleManager(Device &device, RenderContext &render_context);

    [[nodiscard]] const std::vector<ParticleRenderer *> &Particles() const;

    void AddParticle(ParticleRenderer *particle);

    void RemoveParticle(ParticleRenderer *particle);

    void Draw(CommandBuffer &command_buffer, RenderTarget &render_target);

public:
    [[nodiscard]] float TimeStepFactor() const;

    void SetTimeStepFactor(float factor);

private:
    void Emission(uint32_t count,
                  ParticleRenderer *particle,
                  CommandBuffer &command_buffer,
                  RenderTarget &render_target);

    void Simulation(ParticleRenderer *particle, CommandBuffer &command_buffer, RenderTarget &render_target);

private:
    std::vector<ParticleRenderer *> particles_{};
    float time_step_factor_ = 1.0f;

    PostProcessingComputePass *emitter_pass_{nullptr};
    std::unique_ptr<PostProcessingPipeline> emitter_pipeline_{nullptr};
    PostProcessingComputePass *simulation_pass_{nullptr};
    std::unique_ptr<PostProcessingPipeline> simulation_pipeline_{nullptr};
};

template <>
inline ParticleManager *Singleton<ParticleManager>::ms_singleton{nullptr};

}  // namespace vox
