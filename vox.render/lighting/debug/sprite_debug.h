//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/base_material.h"
#include "vox.render/script.h"

namespace vox {
class SpriteDebugMaterial : public BaseMaterial {
public:
    SpriteDebugMaterial(Device &device);

    void SetIsSpotLight(bool value);
};

class SpriteDebug : public Script {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit SpriteDebug(Entity *entity);

    void OnUpdate(float delta_time) override;

private:
    Entity *spot_entity_{nullptr};
    std::shared_ptr<Mesh> spot_light_mesh_{nullptr};

    Entity *point_entity_{nullptr};
    std::shared_ptr<Mesh> point_light_mesh_{nullptr};
};

}  // namespace vox
