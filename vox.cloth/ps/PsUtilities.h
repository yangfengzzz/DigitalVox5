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

#ifndef PSFOUNDATION_PSUTILITIES_H
#define PSFOUNDATION_PSUTILITIES_H

#include "vox.cloth/foundation/PxVec3.h"
#include "vox.cloth/NvCloth/Callbacks.h"
#include "vox.cloth/NvCloth/ps/Ps.h"
#include "vox.cloth/NvCloth/ps/PsBasicTemplates.h"
#include "vox.cloth/NvCloth/ps/PsIntrinsics.h"

/** \brief NVidia namespace */
namespace nv::cloth::ps {

PX_INLINE char littleEndian() {
    int i = 1;
    return *(reinterpret_cast<char*>(&i));
}

// PT: checked casts
PX_CUDA_CALLABLE PX_FORCE_INLINE physx::PxU32 to32(physx::PxU64 value) {
    NV_CLOTH_ASSERT(value <= 0xffffffff);
    return physx::PxU32(value);
}
PX_CUDA_CALLABLE PX_FORCE_INLINE physx::PxU16 to16(physx::PxU32 value) {
    NV_CLOTH_ASSERT(value <= 0xffff);
    return physx::PxU16(value);
}
PX_CUDA_CALLABLE PX_FORCE_INLINE physx::PxU8 to8(physx::PxU16 value) {
    NV_CLOTH_ASSERT(value <= 0xff);
    return physx::PxU8(value);
}
PX_CUDA_CALLABLE PX_FORCE_INLINE physx::PxU8 to8(physx::PxU32 value) {
    NV_CLOTH_ASSERT(value <= 0xff);
    return physx::PxU8(value);
}
PX_CUDA_CALLABLE PX_FORCE_INLINE physx::PxU8 to8(physx::PxI32 value) {
    NV_CLOTH_ASSERT(value <= 0xff);
    NV_CLOTH_ASSERT(value >= 0);
    return physx::PxU8(value);
}
PX_CUDA_CALLABLE PX_FORCE_INLINE physx::PxI8 toI8(physx::PxU32 value) {
    NV_CLOTH_ASSERT(value <= 0x7f);
    return physx::PxI8(value);
}

/*!
Get number of elements in array
*/
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];
#define PX_ARRAY_SIZE(_array) (sizeof(nv::cloth::ps::ArraySizeHelper(_array)))

/*!
Sort two elements using operator<

On return x will be the smaller of the two
*/
template <class T>
PX_CUDA_CALLABLE PX_FORCE_INLINE void order(T& x, T& y) {
    if (y < x) swap(x, y);
}

// most architectures can do predication on real comparisons, and on VMX, it matters

PX_CUDA_CALLABLE PX_FORCE_INLINE void order(physx::PxReal& x, physx::PxReal& y) {
    physx::PxReal newX = physx::PxMin(x, y);
    physx::PxReal newY = physx::PxMax(x, y);
    x = newX;
    y = newY;
}

/*!
Sort two elements using operator< and also keep order
of any extra data
*/
template <class T, class E1>
PX_CUDA_CALLABLE PX_FORCE_INLINE void order(T& x, T& y, E1& xe1, E1& ye1) {
    if (y < x) {
        swap(x, y);
        swap(xe1, ye1);
    }
}

#if PX_GCC_FAMILY && !PX_EMSCRIPTEN && !PX_LINUX
__attribute__((noreturn))
#endif
PX_INLINE void
debugBreak() {
#if PX_WINDOWS || PX_XBOXONE
    __debugbreak();
#elif PX_ANDROID
    raise(SIGTRAP);  // works better than __builtin_trap. Proper call stack and can be continued.
#elif PX_LINUX
#if (PX_X64 || PX_X64)
    asm("int $3");
#else
    raise(SIGTRAP);
#endif
#elif PX_GCC_FAMILY
    __builtin_trap();
#else
    NV_CLOTH_ASSERT(false);
#endif
}

bool checkValid(const float&);
bool checkValid(const physx::PxVec3&);
bool checkValid(const physx::PxQuat&);
bool checkValid(const physx::PxMat33&);
bool checkValid(const physx::PxTransform&);
bool checkValid(const char*);

// equivalent to std::max_element
template <typename T>
inline const T* maxElement(const T* first, const T* last) {
    const T* m = first;
    for (const T* it = first + 1; it < last; ++it)
        if (*m < *it) m = it;

    return m;
}

}  // namespace nv::cloth::ps

#endif
