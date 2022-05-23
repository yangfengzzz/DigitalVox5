//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.core/device.h"
#include "vox.core/memory_manager.h"

namespace vox {
namespace core {

/// Class satisfying the Allocator requirements defined by the C++ standard.
/// This bridge makes the MemoryManager interface accessible to all classes
/// and containers that use the standard Allocator interface.
///
/// This is particularly useful to allocate (potentially cached) GPU memory
/// or different types of memory depending on the provided device.
template <typename T>
class StdAllocator {
public:
    /// T.
    using value_type = T;

    /// Default constructor.
    StdAllocator() = default;

    /// Constructor from device.
    explicit StdAllocator(const Device& device) : device_(device) {}

    /// Default copy constructor.
    StdAllocator(const StdAllocator&) = default;

    /// Default copy assignment operator.
    StdAllocator& operator=(const StdAllocator&) = default;

    /// Default move constructor.
    StdAllocator(StdAllocator&&) = default;

    /// Default move assignment operator.
    StdAllocator& operator=(StdAllocator&&) = default;

    /// Rebind copy constructor.
    template <typename U>
    StdAllocator(const StdAllocator<U>& other) : device_(other.device_) {}

    /// Allocates memory of size \p n.
    T* allocate(std::size_t n) { return static_cast<T*>(MemoryManager::Malloc(n * sizeof(T), device_)); }

    /// Deallocates memory from pointer \p p of size \p n .
    void deallocate(T* p, std::size_t n) { MemoryManager::Free(p, device_); }

    /// Returns true if the instances are equal, false otherwise.
    bool operator==(const StdAllocator& other) const { return device_ == other.device_; }

    /// Returns true if the instances are not equal, false otherwise.
    bool operator!=(const StdAllocator& other) const { return !operator==(other); }

    /// Returns the device on which memory is allocated.
    Device GetDevice() const { return device_; }

private:
    // Allow access in rebind constructor.
    template <typename T2>
    friend class StdAllocator;

    Device device_;
};

}  // namespace core
}  // namespace vox
