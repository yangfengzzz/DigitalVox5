// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2020 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.

#pragma once

#include <utility>

#include "vox.cloth/foundation/PxTransform.h"
#include "vox.cloth/foundation/PxVec3.h"
#include "vox.cloth/foundation/PxVec4.h"
#include "vox.cloth/Simd.h"
#include "vox.cloth/StackAllocator.h"

namespace nv::cloth {

class SwCloth;
struct SwClothData;

typedef StackAllocator<16> SwKernelAllocator;

typedef bool (*InterCollisionFilter)(void* cloth0, void* cloth1);

struct SwInterCollisionData {
    SwInterCollisionData() = default;
    SwInterCollisionData(physx::PxVec4* particles,
                         physx::PxVec4* prevParticles,
                         uint32_t numParticles,
                         uint32_t* indices,
                         physx::PxTransform globalPose,
                         const physx::PxVec3& boundsCenter,
                         const physx::PxVec3& boundsHalfExtents,
                         float impulseScale,
                         void* userData)
        : mParticles(particles),
          mPrevParticles(prevParticles),
          mNumParticles(numParticles),
          mIndices(indices),
          mGlobalPose(std::move(globalPose)),
          mBoundsCenter(boundsCenter),
          mBoundsHalfExtent(boundsHalfExtents),
          mImpulseScale(impulseScale),
          mUserData(userData) {}

    physx::PxVec4* mParticles{};
    physx::PxVec4* mPrevParticles{};
    uint32_t mNumParticles{};
    uint32_t* mIndices{};
    physx::PxTransform mGlobalPose;
    physx::PxVec3 mBoundsCenter;
    physx::PxVec3 mBoundsHalfExtent;
    float mImpulseScale{};
    void* mUserData{};
};

template <typename T4f>
class SwInterCollision {
public:
    SwInterCollision(const SwInterCollisionData* cloths,
                     uint32_t n,
                     float colDist,
                     float stiffness,
                     uint32_t iterations,
                     InterCollisionFilter filter,
                     cloth::SwKernelAllocator& alloc);

    ~SwInterCollision();

    void operator()();

    static size_t estimateTemporaryMemory(SwInterCollisionData* cloths, uint32_t n);

private:
    SwInterCollision& operator=(const SwInterCollision&);  // not implemented

    static size_t getBufferSize(uint32_t);

    void collideParticles(const uint32_t* keys,
                          uint32_t firstColumnSize,
                          const uint32_t* sortedIndices,
                          uint32_t numParticles,
                          uint32_t collisionDistance);

    T4f& getParticle(uint32_t index);

    // better wrap these in a struct
    void collideParticle(uint32_t index);

    T4f mParticle;
    T4f mImpulse;

    T4f mCollisionDistance;
    T4f mCollisionSquareDistance;
    T4f mStiffness;

    uint16_t mClothIndex{};
    uint32_t mClothMask{};
    uint32_t mParticleIndex{};

    uint32_t mNumIterations;

    const SwInterCollisionData* mInstances;
    uint32_t mNumInstances;

    uint16_t* mClothIndices;
    uint32_t* mParticleIndices;
    uint32_t mNumParticles;
    uint32_t* mOverlapMasks{};

    uint32_t mTotalParticles;

    InterCollisionFilter mFilter;

    SwKernelAllocator& mAllocator;

public:
    mutable uint32_t mNumTests{};
    mutable uint32_t mNumCollisions{};
};

// explicit template instantiation declaration
#if NV_SIMD_SIMD
extern template class SwInterCollision<Simd4f>;
#endif
#if NV_SIMD_SCALAR
extern template class SwInterCollision<Scalar4f>;
#endif

}  // namespace nv::cloth
