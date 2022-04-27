//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_material.h"

namespace vox {
ParticleMaterial::ParticleMaterial(Device &device) :
BaseMaterial(device, "particle_instancing"),
particle_data_prop_("u_particleData") {
    shader_data_.set_data(particle_data_prop_, particle_data_);
    
    set_is_transparent(true);
    set_blend_mode(BlendMode::ADDITIVE);
}

float ParticleMaterial::min_particle_size() const {
    return particle_data_.min_particle_size;
}

void ParticleMaterial::set_min_particle_size(float size) {
    particle_data_.min_particle_size = size;
    shader_data_.set_data(particle_data_prop_, particle_data_);
}

float ParticleMaterial::max_particle_size() const {
    return particle_data_.max_particle_size;
}

void ParticleMaterial::set_max_particle_size(float size) {
    particle_data_.max_particle_size = size;
    shader_data_.set_data(particle_data_prop_, particle_data_);
}

ParticleMaterial::ColorMode ParticleMaterial::color_mode() const {
    return particle_data_.color_mode;
}

void ParticleMaterial::set_color_mode(ColorMode mode) {
    particle_data_.color_mode = mode;
    shader_data_.set_data(particle_data_prop_, particle_data_);
}

Vector3F ParticleMaterial::birth_gradient() const {
    return particle_data_.birth_gradient;
}

void ParticleMaterial::set_birth_gradient(const Vector3F &gradient) {
    particle_data_.birth_gradient = gradient;
    shader_data_.set_data(particle_data_prop_, particle_data_);
}

Vector3F ParticleMaterial::death_gradient() const {
    return particle_data_.death_gradient;
}

void ParticleMaterial::set_death_gradient(const Vector3F &gradient) {
    particle_data_.death_gradient = gradient;
    shader_data_.set_data(particle_data_prop_, particle_data_);
}

float ParticleMaterial::fade_coefficient() const {
    return particle_data_.fade_coefficient;
}

void ParticleMaterial::set_fade_coefficient(float coeff) {
    particle_data_.fade_coefficient = coeff;
    shader_data_.set_data(particle_data_prop_, particle_data_);
}

bool ParticleMaterial::debug_draw() const {
    return particle_data_.debug_draw;
}

void ParticleMaterial::set_debug_draw(bool flag) {
    particle_data_.debug_draw = flag ? 1 : 0;
    shader_data_.set_data(particle_data_prop_, particle_data_);
}

}
