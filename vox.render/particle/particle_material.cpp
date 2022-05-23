//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/particle/particle_material.h"

#include "vox.render/shader/shader_manager.h"

namespace vox {
ParticleMaterial::ParticleMaterial(Device &device)
    : BaseMaterial(device, "particle_instancing"), particle_data_prop_("particleData") {
    shader_data_.SetData(particle_data_prop_, particle_data_);

    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/particle/particle_render_instancing.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/particle/particle_render_instancing.frag");

    SetIsTransparent(true);
    SetBlendMode(BlendMode::ADDITIVE);
    input_assembly_state_.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
}

float ParticleMaterial::MinParticleSize() const { return particle_data_.min_particle_size; }

void ParticleMaterial::SetMinParticleSize(float size) {
    particle_data_.min_particle_size = size;
    shader_data_.SetData(particle_data_prop_, particle_data_);
}

float ParticleMaterial::MaxParticleSize() const { return particle_data_.max_particle_size; }

void ParticleMaterial::SetMaxParticleSize(float size) {
    particle_data_.max_particle_size = size;
    shader_data_.SetData(particle_data_prop_, particle_data_);
}

ParticleMaterial::ColorMode ParticleMaterial::GetColorMode() const { return particle_data_.color_mode; }

void ParticleMaterial::SetColorMode(ColorMode mode) {
    particle_data_.color_mode = mode;
    shader_data_.SetData(particle_data_prop_, particle_data_);
}

Vector3F ParticleMaterial::BirthGradient() const { return particle_data_.birth_gradient; }

void ParticleMaterial::SetBirthGradient(const Vector3F &gradient) {
    particle_data_.birth_gradient = gradient;
    shader_data_.SetData(particle_data_prop_, particle_data_);
}

Vector3F ParticleMaterial::DeathGradient() const { return particle_data_.death_gradient; }

void ParticleMaterial::SetDeathGradient(const Vector3F &gradient) {
    particle_data_.death_gradient = gradient;
    shader_data_.SetData(particle_data_prop_, particle_data_);
}

float ParticleMaterial::FadeCoefficient() const { return particle_data_.fade_coefficient; }

void ParticleMaterial::SetFadeCoefficient(float coeff) {
    particle_data_.fade_coefficient = coeff;
    shader_data_.SetData(particle_data_prop_, particle_data_);
}

bool ParticleMaterial::DebugDraw() const { return particle_data_.debug_draw; }

void ParticleMaterial::SetDebugDraw(bool flag) {
    particle_data_.debug_draw = flag ? 1 : 0;
    shader_data_.SetData(particle_data_prop_, particle_data_);
}

}  // namespace vox
