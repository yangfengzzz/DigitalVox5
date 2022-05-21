//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "joint.h"

namespace vox::physics {
/*
 * A fixed joint permits no relative movement between two bodies. ie the bodies are glued together.
 */
class FixedJoint : public Joint {
public:
    FixedJoint(Collider *collider_0, Collider *collider_1);

    void SetProjectionLinearTolerance(float tolerance);

    [[nodiscard]] float ProjectionLinearTolerance() const;

    void SetProjectionAngularTolerance(float tolerance);

    [[nodiscard]] float ProjectionAngularTolerance() const;
};

}  // namespace vox::physics