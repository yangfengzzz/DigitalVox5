//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_BOX_COLLIDER_SHAPE_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_BOX_COLLIDER_SHAPE_H_

#include "collider_shape.h"

namespace vox::physics {
class BoxColliderShape : public ColliderShape {
public:
    BoxColliderShape();
    
    Vector3F size();
    
    void set_size(const Vector3F &value);
    
    void set_world_scale(const Vector3F &scale) override;
    
#ifdef DEBUG
    void set_entity(Entity *value) override;
    
    void sync_box_geometry();
#endif
    
private:
    Vector3F half_ = Vector3F(0.5, 0.5, 0.5);
};

}
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_BOX_COLLIDER_SHAPE_H_ */
