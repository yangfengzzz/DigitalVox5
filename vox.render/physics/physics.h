//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_PHYSICS_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_PHYSICS_H_

#include <PxPhysicsAPI.h>

#include <memory>

#include "vector3.h"

namespace vox::physics {
// MARK: - Forward Declare
class PhysicsMaterial;

class ColliderShape;

using ColliderShapePtr = std::shared_ptr<ColliderShape>;

class Collider;

class CharacterController;

class BoxCharacterController;

class CapsuleCharacterController;

using namespace physx;

// MARK: - PxPhysics
class Physics {
public:
    Physics();

    PxPhysics *operator()();

private:
    PxPhysics *physics_;
    PxDefaultAllocator g_allocator_;
    PxDefaultErrorCallback g_error_callback_;
};

}  // namespace vox::physics

#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_PHYSICS_H_ */
