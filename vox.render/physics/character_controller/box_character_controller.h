//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/physics/character_controller/character_controller.h"

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

    void SetDesc(const PxBoxControllerDesc &desc);

    [[nodiscard]] float HalfHeight() const;

    [[nodiscard]] float HalfSideExtent() const;

    [[nodiscard]] float HalfForwardExtent() const;

    bool SetHalfHeight(float half_height);

    bool SetHalfSideExtent(float half_side_extent);

    bool SetHalfForwardExtent(float half_forward_extent);

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