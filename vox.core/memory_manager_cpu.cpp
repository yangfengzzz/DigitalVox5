//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cstdlib>

#include "vox.core/memory_manager.h"

namespace vox::core {

void* CPUMemoryManager::Malloc(size_t byte_size, const Device& device) {
    void* ptr;
    ptr = std::malloc(byte_size);
    if (byte_size != 0 && !ptr) {
        LOGE("CPU malloc failed")
    }
    return ptr;
}

void CPUMemoryManager::Free(void* ptr, const Device& device) {
    if (ptr) {
        std::free(ptr);
    }
}

void CPUMemoryManager::Memcpy(
        void* dst_ptr, const Device& dst_device, const void* src_ptr, const Device& src_device, size_t num_bytes) {
    std::memcpy(dst_ptr, src_ptr, num_bytes);
}

}  // namespace vox::core
