//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdint>

#ifdef MSC_VER
#include <intrin.h>
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedExchangeAdd64)
#endif

namespace vox {
namespace core {

/// Adds \p val to the value stored at \p address and returns the previous
/// stored value as an atomic operation. This function does not impose any
/// ordering on concurrent memory accesses.
/// \warning This function will treat all values as signed integers on Windows!
inline uint32_t AtomicFetchAddRelaxed(uint32_t* address, uint32_t val) {
#ifdef __GNUC__
    return __atomic_fetch_add(address, val, __ATOMIC_RELAXED);
#elif _MSC_VER
    static_assert(sizeof(long) == sizeof(uint32_t), "Expected long to be a 32 bit type");
    return static_cast<uint32_t>(_InterlockedExchangeAdd(reinterpret_cast<long*>(address), static_cast<long>(val)));
#else
    static_assert(false, "AtomicFetchAddRelaxed not implemented for platform");
#endif
}

/// Adds \p val to the value stored at \p address and returns the previous
/// stored value as an atomic operation. This function does not impose any
/// ordering on concurrent memory accesses.
/// \warning This function will treat all values as signed integers on Windows!
inline uint64_t AtomicFetchAddRelaxed(uint64_t* address, uint64_t val) {
#ifdef __GNUC__
    return __atomic_fetch_add(address, val, __ATOMIC_RELAXED);
#elif _MSC_VER
    return static_cast<uint64_t>(
            _InterlockedExchangeAdd64(reinterpret_cast<int64_t*>(address), static_cast<int64_t>(val)));
#else
    static_assert(false, "AtomicFetchAddRelaxed not implemented for platform");
#endif
}

}  // namespace core
}  // namespace vox
