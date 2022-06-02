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

#ifndef PSFOUNDATION_PSFPU_H
#define PSFOUNDATION_PSFPU_H

#include "vox.cloth/NvCloth/ps/Ps.h"
#include "vox.cloth/NvCloth/ps/PsIntrinsics.h"

#define PX_IR(x) ((uint32_t&)(x))
#define PX_SIR(x) ((int32_t&)(x))
#define PX_FR(x) ((float&)(x))

// signed integer representation of a floating-point value.

// Floating-point representation of a integer value.

#define PX_SIGN_BITMASK 0x80000000

#define PX_FPU_GUARD shdfnd::FPUGuard scopedFpGuard;
#define PX_SIMD_GUARD shdfnd::SIMDGuard scopedFpGuard;

#define PX_SUPPORT_GUARDS (PX_WINDOWS_FAMILY || PX_XBOXONE || (PX_LINUX && (PX_X86 || PX_X64)) || PX_PS4 || PX_OSX)

/** \brief NVidia namespace */
namespace nv::cloth::ps {

// sets the default SDK state for scalar and SIMD units
class NV_CLOTH_IMPORT FPUGuard {
public:
    FPUGuard();   // set fpu control word for PhysX
    ~FPUGuard();  // restore fpu control word
private:
    uint32_t mControlWords[8];
};

// sets default SDK state for simd unit only, lighter weight than FPUGuard
class SIMDGuard {
public:
    PX_INLINE SIMDGuard();   // set simd control word for PhysX
    PX_INLINE ~SIMDGuard();  // restore simd control word
private:
#if PX_SUPPORT_GUARDS
    uint32_t mControlWord;
#endif
};

/**
\brief Enables floating point exceptions for the scalar and SIMD unit
*/
NV_CLOTH_IMPORT void enableFPExceptions();

/**
\brief Disables floating point exceptions for the scalar and SIMD unit
*/
NV_CLOTH_IMPORT void disableFPExceptions();

}  // namespace nv::cloth::ps

#if PX_WINDOWS_FAMILY || PX_XBOXONE
#include "vox.cloth/windows/PsWindowsFPU.h"
#elif (PX_LINUX && PX_SSE2) || PX_PS4 || PX_OSX
#include "vox.cloth/ps/unix/PsUnixFPU.h"
#else
PX_INLINE nv::cloth::ps::SIMDGuard::SIMDGuard() {}
PX_INLINE nv::cloth::ps::SIMDGuard::~SIMDGuard() {}
#endif

#endif  // #ifndef PSFOUNDATION_PSFPU_H
