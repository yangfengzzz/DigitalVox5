//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_renderer.h"
#include "entity.h"
#include "scene.h"
#include "mesh/mesh_renderer.h"
#include "mesh/mesh_manager.h"
#include "particle_manager.h"
#include "shader/internal_variant_name.h"

namespace vox {
namespace {
uint32_t closest_power_of_two(uint32_t const n) {
    uint32_t r = 1u;
    for (uint32_t i = 0u; r < n; r <<= 1u)
        ++i;
    return r;
}

}  // namespace

std::string ParticleRenderer::name() {
    return "ParticleRenderer";
}

ParticleRenderer::ParticleRenderer(Entity *entity) :
Renderer(entity),
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
    alloc_buffer();
    
    mesh_ = MeshManager::get_singleton().load_buffer_mesh();
    mesh_->add_sub_mesh(0, 4);
    
    std::vector<VkVertexInputAttributeDescription> vertex_input_attributes(3);
    vertex_input_attributes[0] = initializers::vertex_input_attribute_description(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
    vertex_input_attributes[1] = initializers::vertex_input_attribute_description(0, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(Vector4F));
    vertex_input_attributes[2] = initializers::vertex_input_attribute_description(0, 2, VK_FORMAT_R32G32B32A32_SFLOAT, 2 * sizeof(Vector4F));
    std::vector<VkVertexInputBindingDescription> vertex_input_bindings(1);
    vertex_input_bindings[0] = initializers::vertex_input_binding_description(0, sizeof(TParticle), VK_VERTEX_INPUT_RATE_INSTANCE);
    mesh_->set_vertex_input_state(vertex_input_bindings, vertex_input_attributes);
    
    material_ = std::make_shared<ParticleMaterial>(entity->scene()->device());
    set_material(material_);
}

void ParticleRenderer::alloc_buffer() {
    auto &device = entity()->scene()->device();
    
    /* Assert than the number of particles will be a factor of threadGroupWidth */
    uint32_t num_particles = ParticleManager::floor_particle_count(k_max_particle_count_); //
    shader_data_.add_define(PARTICLE_COUNT + std::to_string(num_particles));
    fprintf(stderr, "[ %u particles, %u per batch ]\n", num_particles, k_batch_emit_count_);
    
    /* Random value buffer */
    random_vec_.resize(4u * 256);
    shader_data_.set_data(random_buffer_prop_, random_vec_);
    
    /* Atomic */
    atomic_buffer_[0] =
    std::make_unique<core::Buffer>(device, sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    atomic_buffer_[1] =
    std::make_unique<core::Buffer>(device, sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    shader_data_.set_buffer_functor(read_atomic_buffer_prop_, [this]() -> core::Buffer * {
        return atomic_buffer_[read_].get();
    });
    shader_data_.set_buffer_functor(write_atomic_buffer_prop_, [this]() -> core::Buffer * {
        return atomic_buffer_[write_].get();
    });
    
    /* Append Consume */
    append_consume_buffer_[0] = std::make_unique<core::Buffer>(device,
                                                               sizeof(TParticle) * num_particles,
                                                               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                               VMA_MEMORY_USAGE_GPU_ONLY);
    append_consume_buffer_[1] = std::make_unique<core::Buffer>(device,
                                                               sizeof(TParticle) * num_particles,
                                                               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                               VMA_MEMORY_USAGE_GPU_ONLY);
    shader_data_.set_buffer_functor(read_consume_buffer_prop_, [this]() -> core::Buffer * {
        return append_consume_buffer_[read_].get();
    });
    shader_data_.set_buffer_functor(write_consume_buffer_prop_, [this]() -> core::Buffer * {
        return append_consume_buffer_[write_].get();
    });
    
    /* Sort buffers */
    // The parallel nature of the sorting algorithm needs power of two sized buffer.
    unsigned int const kSortBufferMaxCount = closest_power_of_two(k_max_particle_count_); //
    dp_buffer_ = std::make_unique<core::Buffer>(device, sizeof(float) * kSortBufferMaxCount,
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    sort_indices_buffer_ = std::make_unique<core::Buffer>(device, sizeof(uint32_t) * kSortBufferMaxCount * 2,
                                                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    shader_data_.set_buffer_functor(dp_buffer_prop_, [this]() -> auto {
        return dp_buffer_.get();
    });
    shader_data_.set_buffer_functor(sort_indices_buffer_prop_, [this]() -> auto {
        return sort_indices_buffer_.get();
    });
}

void ParticleRenderer::generate_random_values() {
    std::uniform_real_distribution<float> distrib(min_value_, max_value_);
    for (float &i : random_vec_) {
        i = distrib(mt_);
    }
    shader_data_.set_data(random_buffer_prop_, random_vec_);
}

void ParticleRenderer::render(std::vector<RenderElement> &opaque_queue,
                              std::vector<RenderElement> &alpha_test_queue,
                              std::vector<RenderElement> &transparent_queue) {
    if (num_alive_particles_ > 0) {
        transparent_queue.emplace_back(this, mesh_, mesh_->sub_mesh(), material_);
    }
}

void ParticleRenderer::update_bounds(BoundingBox3F &world_bounds) {
    world_bounds.lowerCorner.x = -std::numeric_limits<float>::max();
    world_bounds.lowerCorner.y = -std::numeric_limits<float>::max();
    world_bounds.lowerCorner.z = -std::numeric_limits<float>::max();
    world_bounds.upperCorner.x = std::numeric_limits<float>::max();
    world_bounds.upperCorner.y = std::numeric_limits<float>::max();
    world_bounds.upperCorner.z = std::numeric_limits<float>::max();
}

void ParticleRenderer::update(float delta_time) {
    set_time_step(delta_time * ParticleManager::get_singleton().time_step_factor());
    write_ = 1 - write_;
    read_ = 1 - read_;
    
    // todo
    mesh_->set_instance_count(num_alive_particles_);
    mesh_->set_vertex_buffer_binding(0, append_consume_buffer_[read_].get());
    generate_random_values();
}

uint32_t ParticleRenderer::num_alive_particles() const {
    return num_alive_particles_;
}

ParticleMaterial &ParticleRenderer::material() {
    return *material_;
}

float ParticleRenderer::time_step() const {
    return simulation_data_.time_step;
}

void ParticleRenderer::set_time_step(float step) {
    simulation_data_.time_step = step;
    shader_data_.set_data(simulation_data_prop_, simulation_data_);
}

ParticleRenderer::SimulationVolume ParticleRenderer::bounding_volume_type() const {
    return simulation_data_.bounding_volume_type;
}

void ParticleRenderer::set_bounding_volume_type(SimulationVolume vol) {
    simulation_data_.bounding_volume_type = vol;
    shader_data_.set_data(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::bbox_size() const {
    return simulation_data_.bbox_size;
}

void ParticleRenderer::set_bbox_size(float size) {
    simulation_data_.bbox_size = size;
    shader_data_.set_data(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::scattering_factor() const {
    return simulation_data_.scattering_factor;
}

void ParticleRenderer::set_scattering_factor(float factor) {
    simulation_data_.scattering_factor = factor;
    shader_data_.set_data(simulation_data_prop_, simulation_data_);
    shader_data_.add_define(NEED_PARTICLE_SCATTERING);
}

std::shared_ptr<Image> ParticleRenderer::vector_field_texture() const {
    return vector_field_texture_;
}

void ParticleRenderer::set_vector_field_texture(const std::shared_ptr<Image> &field) {
    vector_field_texture_ = field;
    shader_data_.add_define(NEED_PARTICLE_VECTOR_FIELD);
    shader_data_.set_sampled_texture(vector_field_texture_prop_, field->get_vk_image_view(VK_IMAGE_VIEW_TYPE_3D), nullptr);
}

float ParticleRenderer::vector_field_factor() const {
    return simulation_data_.vector_field_factor;
}

void ParticleRenderer::set_vector_field_factor(float factor) {
    simulation_data_.vector_field_factor = factor;
    shader_data_.set_data(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::curl_noise_factor() const {
    return simulation_data_.curl_noise_factor;
}

void ParticleRenderer::set_curl_noise_factor(float factor) {
    simulation_data_.curl_noise_factor = factor;
    shader_data_.add_define(NEED_PARTICLE_CURL_NOISE);
    shader_data_.set_data(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::curl_noise_scale() const {
    return simulation_data_.curl_noise_scale;
}

void ParticleRenderer::set_curl_noise_scale(float scale) {
    simulation_data_.curl_noise_scale = scale;
    shader_data_.add_define(NEED_PARTICLE_CURL_NOISE);
    shader_data_.set_data(simulation_data_prop_, simulation_data_);
}

float ParticleRenderer::velocity_factor() const {
    return simulation_data_.velocity_factor;
}

void ParticleRenderer::set_velocity_factor(float factor) {
    simulation_data_.velocity_factor = factor;
    shader_data_.add_define(NEED_PARTICLE_VELOCITY_CONTROL);
    shader_data_.set_data(simulation_data_prop_, simulation_data_);
}

//MARK: - Emitter
uint32_t ParticleRenderer::emit_count() const {
    return emitter_data_.emit_count;
}

void ParticleRenderer::set_emit_count(uint32_t count) {
    num_alive_particles_ += count;
    emitter_data_.emit_count = count;
    shader_data_.set_data(emitter_data_prop_, emitter_data_);
}

ParticleRenderer::EmitterType ParticleRenderer::emitter_type() const {
    return emitter_data_.emitter_type;
}

void ParticleRenderer::set_emitter_type(EmitterType type) {
    emitter_data_.emitter_type = type;
    shader_data_.set_data(emitter_data_prop_, emitter_data_);
}

Vector3F ParticleRenderer::emitter_position() const {
    return emitter_data_.emitter_position;
}

void ParticleRenderer::set_emitter_position(const Vector3F &position) {
    emitter_data_.emitter_position = position;
    shader_data_.set_data(emitter_data_prop_, emitter_data_);
}

Vector3F ParticleRenderer::emitter_direction() const {
    return emitter_data_.emitter_direction;
}

void ParticleRenderer::set_emitter_direction(const Vector3F &direction) {
    emitter_data_.emitter_direction = direction;
    shader_data_.set_data(emitter_data_prop_, emitter_data_);
}

float ParticleRenderer::emitter_radius() const {
    return emitter_data_.emitter_radius;
}

void ParticleRenderer::set_emitter_radius(float radius) {
    emitter_data_.emitter_radius = radius;
    shader_data_.set_data(emitter_data_prop_, emitter_data_);
}

float ParticleRenderer::particle_min_age() const {
    return emitter_data_.particle_min_age;
}

void ParticleRenderer::set_particle_min_age(float age) {
    emitter_data_.particle_min_age = age;
    shader_data_.set_data(emitter_data_prop_, emitter_data_);
}

float ParticleRenderer::particle_max_age() const {
    return emitter_data_.particle_max_age;
}

void ParticleRenderer::set_particle_max_age(float age) {
    emitter_data_.particle_max_age = age;
    shader_data_.set_data(emitter_data_prop_, emitter_data_);
}

void ParticleRenderer::on_enable() {
    Renderer::on_enable();
    ParticleManager::get_singleton().add_particle(this);
}

void ParticleRenderer::on_disable() {
    Renderer::on_disable();
    ParticleManager::get_singleton().remove_particle(this);
}

//MARK: - Reflection
void ParticleRenderer::on_serialize(nlohmann::json &data) {
    
}

void ParticleRenderer::on_deserialize(const nlohmann::json &data) {
    
}

void ParticleRenderer::on_inspector(ui::WidgetContainer &p_root) {
    
}

}