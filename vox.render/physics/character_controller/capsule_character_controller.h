//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_CHARACTER_CONTROLLER_CAPSULE_CHARACTER_CONTROLLER_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_CHARACTER_CONTROLLER_CAPSULE_CHARACTER_CONTROLLER_H_

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

    void set_desc(const PxCapsuleControllerDesc &desc);

    [[nodiscard]] float radius() const;

    bool set_radius(float radius);

    [[nodiscard]] float height() const;

    bool set_height(float height);

    [[nodiscard]] PxCapsuleClimbingMode::Enum climbing_mode() const;

    bool set_climbing_mode(PxCapsuleClimbingMode::Enum mode);

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
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_CHARACTER_CONTROLLER_CAPSULE_CHARACTER_CONTROLLER_H_ */
