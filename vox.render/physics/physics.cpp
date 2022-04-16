//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "physics.h"

namespace vox::physics {
Physics::Physics() {
    PxFoundation *g_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_allocator_, g_error_callback_);
    physics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *g_foundation, PxTolerancesScale(), false, nullptr);
}

PxPhysics *Physics::operator()() {
    return physics_;
}

}
