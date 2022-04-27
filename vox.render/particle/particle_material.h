//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "material/base_material.h"
#include "vector3.h"

namespace vox {
class ParticleMaterial : public BaseMaterial {
public:
    enum class ColorMode : uint32_t {
        DEFAULT,
        GRADIENT,
        K_NUM_COLOR_MODE
    };
    
    struct ParticleData {
        Vector3F birth_gradient;
        float min_particle_size;
        Vector3F death_gradient;
        float max_particle_size;
        ColorMode color_mode = ColorMode::DEFAULT;
        float fade_coefficient;
        float debug_draw;
        float pad;
    };
    
    [[nodiscard]] float min_particle_size() const;
    
    void set_min_particle_size(float size);
    
    [[nodiscard]] float max_particle_size() const;
    
    void set_max_particle_size(float size);
    
    [[nodiscard]] ColorMode color_mode() const;
    
    void set_color_mode(ColorMode mode);
    
    [[nodiscard]] Vector3F birth_gradient() const;
    
    void set_birth_gradient(const Vector3F &gradient);
    
    [[nodiscard]] Vector3F death_gradient() const;
    
    void set_death_gradient(const Vector3F &gradient);
    
    [[nodiscard]] float fade_coefficient() const;
    
    void set_fade_coefficient(float coeff);
    
    [[nodiscard]] bool debug_draw() const;
    
    void set_debug_draw(bool flag);
    
    explicit ParticleMaterial(Device &device);
    
private:
    ParticleData particle_data_;
    const std::string particle_data_prop_;
};

}
