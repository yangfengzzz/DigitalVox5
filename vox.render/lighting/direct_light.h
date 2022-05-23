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
 * Directional light.
 */
class DirectLight : public Light {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    struct DirectLightData {
        Vector3F color;
        float color_pad;  // for align
        Vector3F direction;
        float direction_pad;  // for align
    };

    /** Light color. */
    Color color_ = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity_ = 1.0;

    explicit DirectLight(Entity *entity);

public:
    Matrix4x4F ShadowProjectionMatrix() override;

    Vector3F Direction();

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

    void UpdateShaderData(DirectLightData &shader_data);
};

}  // namespace vox
