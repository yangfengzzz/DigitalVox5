//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "simulator/cloth/cloth_application.h"

namespace vox::cloth {
class FrictionApp : public ClothApplication {
public:
    void LoadScene() override;

private:
    void InitializeCloth(Entity* entity, int index, const physx::PxVec3& offset, float friction_coef);

    nv::cloth::Fabric* fabric_[5];
    nv::cloth::Solver* solver_{nullptr};
    ClothActor cloth_actor_[5];
};

}  // namespace vox::cloth
