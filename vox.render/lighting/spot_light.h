//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.render/lighting/light.h"

namespace vox {
/**
 * Spot light.
 */
class SpotLight : public Light {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    struct SpotLightData {
        Vector3F color;
        float color_pad;  // for align
        Vector3F position;
        float position_pad;  // for align
        Vector3F direction;
        float distance;
        float angle_cos;
        float penumbra_cos;
        float pad;   // for align
        float pad2;  // for align
    };

    /** Light color. */
    Color color_ = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity_ = 1.0;
    /** Defines a distance cutoff at which the light's intensity must be considered zero. */
    float distance_ = 5;
    /** Angle, in radians, from centre of spotlight where falloff begins. */
    float angle_ = M_PI / 6;
    /** Angle, in radians, from falloff begins to ends. */
    float penumbra_ = M_PI / 12;

    explicit SpotLight(Entity *entity);

public:
    Matrix4x4F ShadowProjectionMatrix() override;

public:
    /**
     * Called when the serialization is asked
     */
    void OnSerialize(nlohmann::json &data) override;

    /**
     * Called when the deserialization is asked
     */
    void OnDeserialize(const nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void OnInspector(ui::WidgetContainer &p_root) override;

private:
    friend class LightManager;

    /**
     * Mount to the current Scene.
     */
    void OnEnable() override;

    /**
     * Unmount from the current Scene.
     */
    void OnDisable() override;

    void UpdateShaderData(SpotLightData &shader_data);
};

}  // namespace vox
