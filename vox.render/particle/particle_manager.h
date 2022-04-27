//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "singleton.h"
#include "particle_renderer.h"
#include "rendering/postprocessing_pipeline.h"
#include "rendering/postprocessing_computepass.h"

namespace vox {
class ParticleManager : public Singleton<ParticleManager> {
public:
    static constexpr uint32_t particles_kernel_group_width_ = 256;
    
    static constexpr uint32_t threads_group_count(uint32_t nthreads) {
        return (nthreads + particles_kernel_group_width_ - 1u) / particles_kernel_group_width_;
    }
    
    static constexpr uint32_t floor_particle_count(uint32_t nparticles) {
        return particles_kernel_group_width_ * (nparticles / particles_kernel_group_width_);
    }
    
    static ParticleManager &get_singleton();
    
    static ParticleManager *get_singleton_ptr();
    
    ParticleManager(Device &device, RenderContext &render_context);
    
    [[nodiscard]] const std::vector<ParticleRenderer *> &particles() const;
    
    void add_particle(ParticleRenderer *particle);
    
    void remove_particle(ParticleRenderer *particle);
    
    void draw(CommandBuffer &command_buffer, RenderTarget &render_target);
    
public:
    [[nodiscard]] float time_step_factor() const;
    
    void set_time_step_factor(float factor);
    
private:
    void emission(uint32_t count, ParticleRenderer *particle,
                  CommandBuffer &command_buffer, RenderTarget &render_target);
    
    void simulation(ParticleRenderer *particle,
                    CommandBuffer &command_buffer, RenderTarget &render_target);
    
private:
    std::vector<ParticleRenderer *> particles_{};
    float time_step_factor_ = 1.0f;
    
    PostProcessingComputePass *emitter_pass_{nullptr};
    std::unique_ptr<PostProcessingPipeline> emitter_pipeline_{nullptr};
    PostProcessingComputePass *simulation_pass_{nullptr};
    std::unique_ptr<PostProcessingPipeline> simulation_pipeline_{nullptr};
};

template<> inline ParticleManager *Singleton<ParticleManager>::ms_singleton_{nullptr};

}
