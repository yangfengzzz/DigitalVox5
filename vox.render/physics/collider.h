//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_COLLIDER_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_COLLIDER_H_

#include "physics.h"
#include "component.h"
#include "update_flag.h"
#include <vector>

namespace vox::physics {
class Collider : public Component {
public:
    explicit Collider(Entity *entity);
    
    ~Collider() override;
    
    void add_shape(const ColliderShapePtr &shape);
    
    void remove_shape(const ColliderShapePtr &shape);
    
    void clear_shapes();
    
    PxRigidActor *handle();
    
public:
    void on_update();
    
    virtual void on_late_update() {
    }
    
    void on_enable() override;
    
    void on_disable() override;
    
public:
#ifdef _DEBUG
    Entity* debugEntity{nullptr};;
#endif
    
protected:
    friend class PhysicsManager;
    
    ssize_t index_ = -1;
    std::unique_ptr<UpdateFlag> update_flag_;
    physx::PxRigidActor *native_actor_{};
    std::vector<ColliderShapePtr> shapes_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_COLLIDER_H_ */
