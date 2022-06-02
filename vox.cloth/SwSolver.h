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

#include "vox.cloth/NvCloth/Solver.h"
#include "vox.cloth/SwInterCollision.h"

namespace nv::cloth {

class SwCloth;
class SwFactory;

/// CPU/SSE based cloth solver
class SwSolver : public Solver {
    struct SimulatedCloth {
        SimulatedCloth(SwCloth& cloth, SwSolver* parent);
        void Destroy() const;
        void Simulate();

        SwCloth* mCloth;
        uint32_t mScratchMemorySize;
        void* mScratchMemory;
        float mInvNumIterations;

        SwSolver* mParent;
    };
    friend struct SimulatedCloth;

public:
    SwSolver();
    ~SwSolver() override;

    void addCloth(Cloth*) override;
    void addCloths(Range<Cloth*> cloths) override;
    void removeCloth(Cloth*) override;
    int getNumCloths() const override;
    Cloth* const* getClothList() const override;

    // functions executing the simulation work.
    bool beginSimulation(float dt) override;
    void simulateChunk(int idx) override;
    void endSimulation() override;
    int getSimulationChunkCount() const override;

    void setInterCollisionDistance(float distance) override { mInterCollisionDistance = distance; }
    float getInterCollisionDistance() const override { return mInterCollisionDistance; }

    void setInterCollisionStiffness(float stiffness) override { mInterCollisionStiffness = stiffness; }
    float getInterCollisionStiffness() const override { return mInterCollisionStiffness; }

    void setInterCollisionNbIterations(uint32_t nbIterations) override { mInterCollisionIterations = nbIterations; }
    uint32_t getInterCollisionNbIterations() const override { return mInterCollisionIterations; }

    void setInterCollisionFilter(InterCollisionFilter filter) override { mInterCollisionFilter = filter; }

    bool hasError() const override { return false; }

private:
    // add cloth helper functions
    void addClothAppend(Cloth* cloth);

    // simulate helper functions
    void beginFrame() const;
    void endFrame() const;

    void interCollision();

private:
    Vector<SimulatedCloth>::Type mSimulatedCloths;
    typedef Vector<SwCloth*>::Type ClothVector;
    ClothVector mCloths;

    float mInterCollisionDistance;
    float mInterCollisionStiffness;
    uint32_t mInterCollisionIterations;
    InterCollisionFilter mInterCollisionFilter;

    void* mInterCollisionScratchMem;
    uint32_t mInterCollisionScratchMemSize;
    Vector<SwInterCollisionData>::Type mInterCollisionInstances;

    float mCurrentDt{};  // The delta time for the current simulated frame

    mutable void* mSimulateProfileEventData;
};
}  // namespace nv::cloth
