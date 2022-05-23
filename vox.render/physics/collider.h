//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.render/component.h"
#include "vox.render/physics/physics.h"
#include "vox.render/update_flag.h"

namespace vox::physics {
class Collider : public Component {
public:
    explicit Collider(Entity *entity);

    ~Collider() override;

    void AddShape(const ColliderShapePtr &shape);

    void RemoveShape(const ColliderShapePtr &shape);

    void ClearShapes();

    PxRigidActor *Handle();

public:
    void OnUpdate();

    virtual void OnLateUpdate() {}

    void OnEnable() override;

    void OnDisable() override;

public:
#ifdef DEBUG
    Entity *debug_entity_{nullptr};
#endif

protected:
    friend class PhysicsManager;

    ssize_t index_ = -1;
    std::unique_ptr<UpdateFlag> update_flag_;
    physx::PxRigidActor *native_actor_{};
    std::vector<ColliderShapePtr> shapes_;
};

}  // namespace vox::physics