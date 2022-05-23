//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/particle/particle_manager.h"

#include "vox.render/logging.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
ParticleManager *ParticleManager::GetSingletonPtr() { return ms_singleton; }

ParticleManager &ParticleManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}
//-----------------------------------------------------------------------
ParticleManager::ParticleManager(Device &device, RenderContext &render_context) {
    emitter_pipeline_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    emitter_pass_ = &emitter_pipeline_->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_emission.comp"));

    simulation_pipeline_ = std::make_unique<PostProcessingPipeline>(render_context, ShaderSource());
    simulation_pass_ = &simulation_pipeline_->AddPass<PostProcessingComputePass>(
            ShaderManager::GetSingleton().LoadShader("base/particle/particle_simulation.comp"));
}

const std::vector<ParticleRenderer *> &ParticleManager::Particles() const { return particles_; }

void ParticleManager::AddParticle(ParticleRenderer *particle) {
    auto iter = std::find(particles_.begin(), particles_.end(), particle);
    if (iter == particles_.end()) {
        particles_.push_back(particle);
    } else {
        LOGE("Particle already attached.")
    }
}

void ParticleManager::RemoveParticle(ParticleRenderer *particle) {
    auto iter = std::find(particles_.begin(), particles_.end(), particle);
    if (iter != particles_.end()) {
        particles_.erase(iter);
    }
}

float ParticleManager::TimeStepFactor() const { return time_step_factor_; }

void ParticleManager::SetTimeStepFactor(float factor) { time_step_factor_ = factor; }

void ParticleManager::Draw(CommandBuffer &command_buffer, RenderTarget &render_target) {
    for (auto &particle : particles_) {
        /* Max number of particles able to be spawned. */
        uint32_t const kNumDeadParticles = ParticleRenderer::k_max_particle_count_ - particle->NumAliveParticles();
        /* Number of particles to be emitted. */
        uint32_t const kEmitCount = std::min(ParticleRenderer::k_batch_emit_count_, kNumDeadParticles);  //
        Emission(kEmitCount, particle, command_buffer, render_target);
        Simulation(particle, command_buffer, render_target);
    }
}

void ParticleManager::Emission(const uint32_t count,
                               ParticleRenderer *particle,
                               CommandBuffer &command_buffer,
                               RenderTarget &render_target) {
    /* Emit only if a minimum count is reached. */
    if (!count) {
        return;
    }
    if (count < ParticleRenderer::k_batch_emit_count_) {
        // return;
    }
    particle->SetEmitCount(count);

    emitter_pass_->AttachShaderData(&particle->shader_data_);
    auto n_groups = ThreadsGroupCount(count);
    emitter_pass_->SetDispatchSize({n_groups, 1, 1});
    emitter_pipeline_->Draw(command_buffer, render_target);
    emitter_pass_->DetachShaderData(&particle->shader_data_);
}

void ParticleManager::Simulation(ParticleRenderer *particle,
                                 CommandBuffer &command_buffer,
                                 RenderTarget &render_target) {
    if (particle->NumAliveParticles() == 0u) {
        return;
    }

    simulation_pass_->AttachShaderData(&particle->shader_data_);
    auto n_groups = ThreadsGroupCount(particle->NumAliveParticles());
    simulation_pass_->SetDispatchSize({n_groups, 1, 1});
    simulation_pipeline_->Draw(command_buffer, render_target);
    simulation_pass_->DetachShaderData(&particle->shader_data_);
}

}  // namespace vox
