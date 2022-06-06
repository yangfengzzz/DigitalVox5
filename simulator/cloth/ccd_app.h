//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "cloth_application.h"

namespace vox::cloth {
class CCDApp : public ClothApplication {
public:
    void LoadScene() override;

    void Update(float delta_time) override;

private:
    void InitializeCloth(Entity *entity, const physx::PxVec3 &offset);

    nv::cloth::Fabric *fabric_{nullptr};
    nv::cloth::Solver *solver_{nullptr};
    ClothActor cloth_actor_;

    physx::PxVec3 offset_;
};

}  // namespace vox::cloth