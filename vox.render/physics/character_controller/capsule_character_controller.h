//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "character_controller.h"

namespace vox::physics {
/**
 * A capsule character controller.
 */
class CapsuleCharacterController : public CharacterController {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit CapsuleCharacterController(Entity *entity);

    void SetDesc(const PxCapsuleControllerDesc &desc);

    [[nodiscard]] float Radius() const;

    bool SetRadius(float radius);

    [[nodiscard]] float Height() const;

    bool SetHeight(float height);

    [[nodiscard]] PxCapsuleClimbingMode::Enum ClimbingMode() const;

    bool SetClimbingMode(PxCapsuleClimbingMode::Enum mode);

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
};

}  // namespace vox::physics