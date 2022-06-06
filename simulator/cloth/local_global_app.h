//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "cloth_application.h"

namespace vox::cloth {
class LocalGlobalApp : public ClothApplication {
public:
    void LoadScene() override;

    void Update(float delta_time) override;

private:
    void InitializeCloth(Entity* entity, int index, const physx::PxVec3& offset);

    nv::cloth::Fabric* fabric_[2];
    nv::cloth::Solver* solver_[2];
    ClothActor cloth_actor_[2];

    int attachment_vertices_[2];
    physx::PxVec4 attachment_vertex_original_positions_[2];

    float time_;
};

}  // namespace vox::cloth