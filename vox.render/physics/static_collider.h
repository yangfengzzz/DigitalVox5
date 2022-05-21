//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "collider.h"

namespace vox::physics {
class StaticCollider : public Collider {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit StaticCollider(Entity *entity);

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