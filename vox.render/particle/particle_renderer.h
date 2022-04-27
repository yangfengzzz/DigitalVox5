//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "renderer.h"
#include "particle_material.h"
#include "mesh/buffer_mesh.h"
#include <random>

namespace vox {
class ParticleRenderer : public Renderer {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    // [USER DEFINED]
    static constexpr float k_default_simulation_volume_size_ = 32.0f;
    static constexpr uint32_t k_max_particle_count_ = (1u << 15u);
    static constexpr uint32_t k_batch_emit_count_ = std::max(256u, (k_max_particle_count_ >> 4u));
    
    enum class EmitterType : uint32_t {
        POINT,
        DISK,
        SPHERE,
        BALL,
        K_NUM_EMITTER_TYPE
    };
    
    enum class SimulationVolume : uint32_t {
        SPHERE,
        BOX,
        NONE,
        K_NUM_SIMULATION_VOLUME
    };
    
    struct TParticle {
        Vector4<float> position;
        Vector4<float> velocity;
        float start_age;
        float age;
        float pad;
        uint32_t id;
    };
    
    struct ParticleEmitterData {
        Vector3F emitter_position;
        uint32_t emit_count;
        Vector3F emitter_direction;
        EmitterType emitter_type;
        float emitter_radius;
        float particle_min_age;
        float particle_max_age;
        float pad;
    };
    
    struct ParticleSimulationData {
        float time_step;
        SimulationVolume bounding_volume_type;
        float bbox_size;
        float scattering_factor;
        float vector_field_factor;
        float curl_noise_factor;
        float curl_noise_scale;
        float velocity_factor;
    };
    
    explicit ParticleRenderer(Entity *entity);
    
    ParticleMaterial &material();
    
    [[nodiscard]] uint32_t num_alive_particles() const;
    
public:
    void render(std::vector<RenderElement> &opaque_queue,
                std::vector<RenderElement> &alpha_test_queue,
                std::vector<RenderElement> &transparent_queue) override;
    
    void update_bounds(BoundingBox3F &world_bounds) override;
    
    void update(float delta_time) override;
    
public:
    [[nodiscard]] float time_step() const;
    
    void set_time_step(float step);
    
    [[nodiscard]] SimulationVolume bounding_volume_type() const;
    
    void set_bounding_volume_type(SimulationVolume vol);
    
    [[nodiscard]] float bbox_size() const;
    
    void set_bbox_size(float size);
    
    [[nodiscard]] float scattering_factor() const;
    
    void set_scattering_factor(float factor);
    
    [[nodiscard]] float vector_field_factor() const;
    
    void set_vector_field_factor(float factor);
    
    [[nodiscard]] std::shared_ptr<Image> vector_field_texture() const;
    
    void set_vector_field_texture(const std::shared_ptr<Image> &field);
    
    [[nodiscard]] float curl_noise_factor() const;
    
    void set_curl_noise_factor(float factor);
    
    [[nodiscard]] float curl_noise_scale() const;
    
    void set_curl_noise_scale(float scale);
    
    [[nodiscard]] float velocity_factor() const;
    
    void set_velocity_factor(float factor);
    
public:
    [[nodiscard]] uint32_t emit_count() const;
    
    void set_emit_count(uint32_t count);
    
    [[nodiscard]] EmitterType emitter_type() const;
    
    void set_emitter_type(EmitterType type);
    
    [[nodiscard]] Vector3F emitter_position() const;
    
    void set_emitter_position(const Vector3F &position);
    
    [[nodiscard]] Vector3F emitter_direction() const;
    
    void set_emitter_direction(const Vector3F &direction);
    
    [[nodiscard]] float emitter_radius() const;
    
    void set_emitter_radius(float radius);
    
    [[nodiscard]] float particle_min_age() const;
    
    void set_particle_min_age(float age);
    
    [[nodiscard]] float particle_max_age() const;
    
    void set_particle_max_age(float age);
    
private:
    void on_enable() override;
    
    void on_disable() override;
    
    void alloc_buffer();
    
    void generate_random_values();
    
public:
    /**
     * Called when the serialization is asked
     */
    void on_serialize(nlohmann::json &data) override;
    
    /**
     * Called when the deserialization is asked
     */
    void on_deserialize(const nlohmann::json &data) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void on_inspector(ui::WidgetContainer &p_root) override;
    
private:
    uint32_t num_alive_particles_ = 0;
    
    std::shared_ptr<BufferMesh> mesh_{nullptr};
    std::shared_ptr<ParticleMaterial> material_{nullptr};
    
    std::random_device random_device_;
    std::mt19937 mt_;
    float min_value_ = 0.0;
    float max_value_ = 1.0;
    std::vector<float> random_vec_{};
    const std::string random_buffer_prop_;
    
    ParticleSimulationData simulation_data_{};
    const std::string simulation_data_prop_;
    
    ParticleEmitterData emitter_data_;
    const std::string emitter_data_prop_;
    
    std::shared_ptr<Image> vector_field_texture_{nullptr};
    const std::string vector_field_texture_prop_;
    
private:
    uint32_t read_ = 0;
    uint32_t write_ = 1;
    std::unique_ptr<core::Buffer> atomic_buffer_[2] = {nullptr, nullptr};
    const std::string read_atomic_buffer_prop_;
    const std::string write_atomic_buffer_prop_;
    
    std::unique_ptr<core::Buffer> append_consume_buffer_[2] = {nullptr, nullptr};
    const std::string read_consume_buffer_prop_;
    const std::string write_consume_buffer_prop_;
    
    std::unique_ptr<core::Buffer> dp_buffer_{nullptr};
    const std::string dp_buffer_prop_;
    std::unique_ptr<core::Buffer> sort_indices_buffer_{nullptr};
    const std::string sort_indices_buffer_prop_;
};
}
