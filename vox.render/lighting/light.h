//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/matrix4x4.h"
#include "vox.render/component.h"

namespace vox {
/**
 * Light base class.
 */
class Light : public Component {
public:
    /**
     * Each type of light source is at most 10, beyond which it will not take effect.
     * */
    static constexpr uint32_t max_light_ = 10;

    explicit Light(Entity *entity);

    /**
     * View matrix.
     */
    Matrix4x4F ViewMatrix();

    /**
     * Inverse view matrix.
     */
    Matrix4x4F InverseViewMatrix();

public:
    [[nodiscard]] bool EnableShadow() const;

    void SetEnableShadow(bool enabled);

    /**
     * Shadow bias.
     */
    [[nodiscard]] float ShadowBias() const;

    void SetShadowBias(float value);

    /**
     * Shadow intensity, the larger the value, the clearer and darker the shadow.
     */
    [[nodiscard]] float ShadowIntensity() const;

    void SetShadowIntensity(float value);

    /**
     * Pixel range used for shadow PCF interpolation.
     */
    [[nodiscard]] float ShadowRadius() const;

    void SetShadowRadius(float value);

    virtual Matrix4x4F ShadowProjectionMatrix() = 0;

private:
    bool enable_shadow_ = false;
    float shadow_bias_ = 0.005;
    float shadow_intensity_ = 0.2;
    float shadow_radius_ = 1;
};

}  // namespace vox
