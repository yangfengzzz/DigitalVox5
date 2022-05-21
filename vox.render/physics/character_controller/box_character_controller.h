//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_CHARACTER_CONTROLLER_BOX_CHARACTER_CONTROLLER_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_CHARACTER_CONTROLLER_BOX_CHARACTER_CONTROLLER_H_

#include "character_controller.h"

namespace vox::physics {
/**
 * A box character controller.
 */
class BoxCharacterController : public CharacterController {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit BoxCharacterController(Entity *entity);

    void set_desc(const PxBoxControllerDesc &desc);

    [[nodiscard]] float half_height() const;

    [[nodiscard]] float half_side_extent() const;

    [[nodiscard]] float half_forward_extent() const;

    bool set_half_height(float half_height);

    bool set_half_side_extent(float half_side_extent);

    bool set_half_forward_extent(float half_forward_extent);

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

#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_CHARACTER_CONTROLLER_BOX_CHARACTER_CONTROLLER_H_ */
