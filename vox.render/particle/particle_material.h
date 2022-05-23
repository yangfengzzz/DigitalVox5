//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector3.h"
#include "vox.render/material/base_material.h"

namespace vox {
class ParticleMaterial : public BaseMaterial {
public:
    enum class ColorMode : uint32_t { DEFAULT, GRADIENT, K_NUM_COLOR_MODE };

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

    [[nodiscard]] float MinParticleSize() const;

    void SetMinParticleSize(float size);

    [[nodiscard]] float MaxParticleSize() const;

    void SetMaxParticleSize(float size);

    [[nodiscard]] ColorMode GetColorMode() const;

    void SetColorMode(ColorMode mode);

    [[nodiscard]] Vector3F BirthGradient() const;

    void SetBirthGradient(const Vector3F &gradient);

    [[nodiscard]] Vector3F DeathGradient() const;

    void SetDeathGradient(const Vector3F &gradient);

    [[nodiscard]] float FadeCoefficient() const;

    void SetFadeCoefficient(float coeff);

    [[nodiscard]] bool DebugDraw() const;

    void SetDebugDraw(bool flag);

    explicit ParticleMaterial(Device &device);

private:
    ParticleData particle_data_;
    const std::string particle_data_prop_;
};

}  // namespace vox
