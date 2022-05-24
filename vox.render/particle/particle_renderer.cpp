//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/particle/particle_renderer.h"

#include "vox.render/entity.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/particle/particle_manager.h"
#include "vox.render/scene.h"
#include "vox.render/shader/internal_variant_name.h"

namespace vox {
namespace {
uint32_t ClosestPowerOfTwo(uint32_t const n) {
    uint32_t r = 1u;
    for (uint32_t i = 0u; r < n; r <<= 1u) ++i;
    return r;
}

}  // namespace

std::string ParticleRenderer::name() { return "ParticleRenderer"; }

ParticleRenderer::ParticleRenderer(Entity *entity)
    : Renderer(entity),
      random_buffer_prop_("randomBuffer"),
      simulation_data_prop_("simulationData"),
      emitter_data_prop_("emitterData"),
      vector_field_texture_prop_("vectorFieldSampler"),

      read_atomic_buffer_prop_("readAtomicBuffer"),
      write_atomic_buffer_prop_("writeAtomicBuffer"),
      read_consume_buffer_prop_("readConsumeBuffer"),
      write_consume_buffer_prop_("writeConsumeBuffer"),
      dp_buffer_prop_("dpBuffer"),
      sort_indices_buffer_prop_("sortIndicesBuffer") {
    AllocBuffer();

    mesh_ = MeshManager::GetSingleton().LoadBufferMesh();
    mesh_->AddSubMesh(0, 4);

    std::vector<VkVertexInputAttributeDescription> vertex_input_attributes(3);
    vertex_input_attributes[0] = initializers::VertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
    vertex_input_attributes[1] =
            initializers::VertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(Vector4F));
    vertex_input_attributes[2] =
            initializers::VertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32A32_SFLOAT, 2 * sizeof(Vector4F));
    std::vector<VkVertexInputBindingDescription> vertex_input_bindings(1);
    vertex_input_bindings[0] =
            initializers::VertexInputBindingDescription(0, sizeof(TParticle), VK_VERTEX_INPUT_RATE_INSTANCE);
    mesh_->SetVertexInputState(vertex_input_bindings, vertex_input_attributes);

    material_ = std::make_shared<ParticleMaterial>(entity->Scene()->Device());
    SetMaterial(material_);
}

void ParticleRenderer::AllocBuffer() {
    auto &device = GetEntity()->Scene()->Device();

    /* Assert than the number of particles will be a factor of threadGroupWidth */
    uint32_t num_particles = ParticleManager::FloorParticleCount(k_max_particle_count_);  //
    shader_data_.AddDefine(PARTICLE_COUNT + std::to_string(num_particles));
    fprintf(stderr, "[ %u particles, %u per batch ]\n", num_particles, k_batch_emit_count_);

    /* Random value buffer */
    random_vec_.resize(4u * 256);
    shader_data_.SetData(random_buffer_prop_, random_vec_);

    /* Atomic */
    atomic_buffer_[0] = std::make_unique<core::Buffer>(device, sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                       VMA_MEMORY_USAGE_CPU_TO_GPU);
    atomic_buffer_[1] = std::make_unique<core::Buffer>(device, sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                       VMA_MEMORY_USAGE_CPU_TO_GPU);
    shader_data_.SetBufferFunctor(read_atomic_buffer_prop_,
                                  [this]() -> core::Buffer * { return atomic_buffer_[read_].get(); });
    shader_data_.SetBufferFunctor(write_atomic_buffer_prop_,
                                  [this]() -> core::Buffer * { return atomic_buffer_[write_].get(); });

    /* Append Consume */
    append_consume_buffer_[0] = std::make_unique<core::Buffer>(
            device, sizeof(TParticle) * num_particles,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    append_consume_buffer_[1] = std::make_unique<core::Buffer>(
            device, sizeof(TParticle) * num_particles,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    shader_data_.SetBufferFunctor(read_consume_buffer_prop_,
                                  [this]() -> core::Buffer * { return append_consume_buffer_[read_].get(); });
    shader_data_.SetBufferFunctor(write_consume_buffer_prop_,
                                  [this]() -> core::Buffer * { return append_consume_buffer_[write_].get(); });

    /* Sort buffers */
    // The parallel nature of the sorting algorithm needs power of two sized buffer.
    unsigned int const kSortBufferMaxCount = ClosestPowerOfTwo(k_max_particle_count_);  //
    dp_buffer_ = std::make_unique<core::Buffer>(device, sizeof(float) * kSortBufferMaxCount,
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    sort_indices_buffer_ =
            std::make_unique<core::Buffer>(device, sizeof(uint32_t) * kSortBufferMaxCount * 2,
                                           VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    shader_data_.SetBufferFunctor(
            dp_buffer_prop_, [this]() -> auto{ return dp_buffer_.get(); });
    shader_data_.SetBufferFunctor(
            sort_indices_buffer_prop_, [this]() -> auto{ return sort_indices_buffer_.get(); });
}

void ParticleRenderer::GenerateRandomValues() {
    std::uniform_real_distribution<float> distrib(min_value_, max_value_);
    for (float &i : random_vec_) {
        i = distrib(mt_);
    }
    shader_data_.SetData(random_buffer_prop_, random_vec_);
}

void ParticleRenderer::Render(std::vector<RenderElement> &opaque_queue,
                              std::vector<RenderElement> &alpha_test_queue,
                              std::vector<RenderElement> &transparent_queue) {
    if (num_alive_particles_ > 0) {
        transparent_queue.emplace_back(this, mesh_, mesh_->FirstSubMesh(), material_);
    }
}

void ParticleRenderer::UpdateBounds(BoundingBox3F &world_bounds) {
    world_bounds.lower_corner.x = -std::numeric_limits<float>::max();
    world_bounds.lower_corner.y = -std::numeric_limits<float>::max();
    world_bounds.lower_corner.z = -std::numeric_limits<float>::max();
    world_bounds.upper_corner.x = std::numeric_limits<float>::max();
    world_bounds.upper_corner.y = std::numeric_limits<float>::max();
    world_bounds.upper_corner.z = std::numeric_limits<float>::max();
}

void ParticleRenderer::Update(float delta_time) {
    SetTimeStep(delta_time * ParticleManager::GetSingleton().TimeStepFactor());
    write_ = 1 - write_;
    read_ = 1 - read_;

    // todo
    mesh_->SetInstanceCount(num_alive_particles_);
    mesh_->SetVertexBufferBinding(0, append_consume_buffer_[read_].get());
    GenerateRandomValues();
}

uint32_t ParticleRenderer::NumAliveParticles() const { return num_alive_particles_; }

ParticleMaterial &ParticleRenderer::Material() { return *material_; }

float ParticleRenderer::TimeStep() const { return simulation_data_.time_step; }

void ParticleRenderer::SetTimeStep(float step) {
    simulation_data_.time_step = step;
    shader_data_.SetData(simulation_data_prop_, simulation_data_);
}

ParticleRenderer::SimulationVolume ParticleRenderer::BoundingVolumeType() const {
    return simulation_data_.bounding_volume_type;
}

void ParticleRenderer::SetBoundingVolumeType(SimulationVolume vol) {
    simulation_data_.bounding_volume_type = vol;
    shader_data_.SetData(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::BboxSize() const { return simulation_data_.bbox_size; }

void ParticleRenderer::SetBboxSize(float size) {
    simulation_data_.bbox_size = size;
    shader_data_.SetData(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::ScatteringFactor() const { return simulation_data_.scattering_factor; }

void ParticleRenderer::SetScatteringFactor(float factor) {
    simulation_data_.scattering_factor = factor;
    shader_data_.SetData(simulation_data_prop_, simulation_data_);
    shader_data_.AddDefine(NEED_PARTICLE_SCATTERING);
}

std::shared_ptr<Texture> ParticleRenderer::VectorFieldTexture() const { return vector_field_texture_; }

void ParticleRenderer::SetVectorFieldTexture(const std::shared_ptr<Texture> &field) {
    vector_field_texture_ = field;
    shader_data_.AddDefine(NEED_PARTICLE_VECTOR_FIELD);
    shader_data_.SetSampledTexture(vector_field_texture_prop_, field->GetVkImageView(VK_IMAGE_VIEW_TYPE_3D), nullptr);
}

float ParticleRenderer::VectorFieldFactor() const { return simulation_data_.vector_field_factor; }

void ParticleRenderer::SetVectorFieldFactor(float factor) {
    simulation_data_.vector_field_factor = factor;
    shader_data_.SetData(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::CurlNoiseFactor() const { return simulation_data_.curl_noise_factor; }

void ParticleRenderer::SetCurlNoiseFactor(float factor) {
    simulation_data_.curl_noise_factor = factor;
    shader_data_.AddDefine(NEED_PARTICLE_CURL_NOISE);
    shader_data_.SetData(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::CurlNoiseScale() const { return simulation_data_.curl_noise_scale; }

void ParticleRenderer::SetCurlNoiseScale(float scale) {
    simulation_data_.curl_noise_scale = scale;
    shader_data_.AddDefine(NEED_PARTICLE_CURL_NOISE);
    shader_data_.SetData(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::VelocityFactor() const { return simulation_data_.velocity_factor; }

void ParticleRenderer::SetVelocityFactor(float factor) {
    simulation_data_.velocity_factor = factor;
    shader_data_.AddDefine(NEED_PARTICLE_VELOCITY_CONTROL);
    shader_data_.SetData(simulation_data_prop_, simulation_data_);
}

// MARK: - Emitter
uint32_t ParticleRenderer::EmitCount() const { return emitter_data_.emit_count; }

void ParticleRenderer::SetEmitCount(uint32_t count) {
    num_alive_particles_ += count;
    emitter_data_.emit_count = count;
    shader_data_.SetData(emitter_data_prop_, emitter_data_);
}

ParticleRenderer::EmitterType ParticleRenderer::GetEmitterType() const { return emitter_data_.emitter_type; }

void ParticleRenderer::SetEmitterType(EmitterType type) {
    emitter_data_.emitter_type = type;
    shader_data_.SetData(emitter_data_prop_, emitter_data_);
}

Vector3F ParticleRenderer::EmitterPosition() const { return emitter_data_.emitter_position; }

void ParticleRenderer::SetEmitterPosition(const Vector3F &position) {
    emitter_data_.emitter_position = position;
    shader_data_.SetData(emitter_data_prop_, emitter_data_);
}

Vector3F ParticleRenderer::EmitterDirection() const { return emitter_data_.emitter_direction; }

void ParticleRenderer::SetEmitterDirection(const Vector3F &direction) {
    emitter_data_.emitter_direction = direction;
    shader_data_.SetData(emitter_data_prop_, emitter_data_);
}

float ParticleRenderer::EmitterRadius() const { return emitter_data_.emitter_radius; }

void ParticleRenderer::SetEmitterRadius(float radius) {
    emitter_data_.emitter_radius = radius;
    shader_data_.SetData(emitter_data_prop_, emitter_data_);
}

float ParticleRenderer::ParticleMinAge() const { return emitter_data_.particle_min_age; }

void ParticleRenderer::SetParticleMinAge(float age) {
    emitter_data_.particle_min_age = age;
    shader_data_.SetData(emitter_data_prop_, emitter_data_);
}

float ParticleRenderer::ParticleMaxAge() const { return emitter_data_.particle_max_age; }

void ParticleRenderer::SetParticleMaxAge(float age) {
    emitter_data_.particle_max_age = age;
    shader_data_.SetData(emitter_data_prop_, emitter_data_);
}

void ParticleRenderer::OnEnable() {
    Renderer::OnEnable();
    ParticleManager::GetSingleton().AddParticle(this);
}

void ParticleRenderer::OnDisable() {
    Renderer::OnDisable();
    ParticleManager::GetSingleton().RemoveParticle(this);
}

// MARK: - Reflection
void ParticleRenderer::OnSerialize(nlohmann::json &data) {}

void ParticleRenderer::OnDeserialize(const nlohmann::json &data) {}

void ParticleRenderer::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
