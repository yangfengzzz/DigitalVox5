//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_manager.h"
#include "shader/shader_manager.h"
#include "logging.h"

namespace vox {
ParticleManager *ParticleManager::get_singleton_ptr() {
    return ms_singleton_;
}

ParticleManager &ParticleManager::get_singleton() {
    assert(ms_singleton_);
    return (*ms_singleton_);
}
//-----------------------------------------------------------------------
ParticleManager::ParticleManager(Device &device, RenderContext &render_context) {
    emitter_pipeline_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    emitter_pass_ = &emitter_pipeline_
    ->add_pass<PostProcessingComputePass>(ShaderManager::get_singleton().load_shader("base/light/cluster_bounds.comp"));
    
    simulation_pipeline_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    simulation_pass_ = &simulation_pipeline_
    ->add_pass<PostProcessingComputePass>(ShaderManager::get_singleton().load_shader("base/light/cluster_bounds.comp"));
}

const std::vector<ParticleRenderer *> &ParticleManager::particles() const {
    return particles_;
}

void ParticleManager::add_particle(ParticleRenderer *particle) {
    auto iter = std::find(particles_.begin(), particles_.end(), particle);
    if (iter == particles_.end()) {
        particles_.push_back(particle);
    } else {
        LOGE("Particle already attached.")
    }
}

void ParticleManager::remove_particle(ParticleRenderer *particle) {
    auto iter = std::find(particles_.begin(), particles_.end(), particle);
    if (iter != particles_.end()) {
        particles_.erase(iter);
    }
}

float ParticleManager::time_step_factor() const {
    return time_step_factor_;
}

void ParticleManager::set_time_step_factor(float factor) {
    time_step_factor_ = factor;
}

void ParticleManager::draw(CommandBuffer &command_buffer, RenderTarget &render_target) {
    for (auto &particle : particles_) {
        /* Max number of particles able to be spawned. */
        uint32_t const kNumDeadParticles = ParticleRenderer::k_max_particle_count_ - particle->num_alive_particles();
        /* Number of particles to be emitted. */
        uint32_t const kEmitCount = std::min(ParticleRenderer::k_batch_emit_count_, kNumDeadParticles); //
        emission(kEmitCount, particle, command_buffer, render_target);
        simulation(particle, command_buffer, render_target);
    }
}

void ParticleManager::emission(const uint32_t count, ParticleRenderer *particle,
                               CommandBuffer &command_buffer, RenderTarget &render_target) {
    /* Emit only if a minimum count is reached. */
    if (!count) {
        return;
    }
    if (count < ParticleRenderer::k_batch_emit_count_) {
        //return;
    }
    particle->set_emit_count(count);
    
    emitter_pass_->attach_shader_data(&particle->shader_data_);
    auto n_groups = threads_group_count(count);
    emitter_pass_->set_dispatch_size({n_groups, 1, 1});
    emitter_pipeline_->draw(command_buffer, render_target);
    emitter_pass_->detach_shader_data(&particle->shader_data_);
}

void ParticleManager::simulation(ParticleRenderer *particle,
                                 CommandBuffer &command_buffer, RenderTarget &render_target) {
    if (particle->num_alive_particles() == 0u) {
        return;
    }
    
    simulation_pass_->attach_shader_data(&particle->shader_data_);
    auto n_groups = threads_group_count(particle->num_alive_particles());
    simulation_pass_->set_dispatch_size({n_groups, 1, 1});
    simulation_pipeline_->draw(command_buffer, render_target);
    simulation_pass_->detach_shader_data(&particle->shader_data_);
}

}
