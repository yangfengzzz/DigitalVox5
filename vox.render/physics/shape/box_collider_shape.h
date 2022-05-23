//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/physics/shape/collider_shape.h"

namespace vox::physics {
class BoxColliderShape : public ColliderShape {
public:
    BoxColliderShape();

    Vector3F Size();

    void SetSize(const Vector3F &value);

    void SetWorldScale(const Vector3F &scale) override;

#ifdef DEBUG
    void SetEntity(Entity *value) override;

    void SyncBoxGeometry();
#endif

private:
    Vector3F half_ = Vector3F(0.5, 0.5, 0.5);
};

}  // namespace vox::physics