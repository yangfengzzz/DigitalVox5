//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <assert.h>

#include <atomic>
#include <memory>
#include <vector>

#include "vox.core/hashmap/hash_backend_buffer.h"

namespace vox {
namespace core {

class CPUHashBackendBufferAccessor {
public:
    /// Must initialize from a non-const buffer to grab the heap top.
    CPUHashBackendBufferAccessor(HashBackendBuffer &hashmap_buffer)
        : capacity_(hashmap_buffer.GetCapacity()),
          key_dsize_(hashmap_buffer.GetKeyDsize()),
          value_dsizes_(hashmap_buffer.GetValueDsizes()),
          heap_(hashmap_buffer.GetIndexHeap().GetDataPtr<buf_index_t>()),
          key_buffer_ptr_(hashmap_buffer.GetKeyBuffer().GetDataPtr<uint8_t>()) {
        std::vector<Tensor> value_buffers = hashmap_buffer.GetValueBuffers();
        for (size_t i = 0; i < value_buffers.size(); ++i) {
            void *value_buffer_ptr = value_buffers[i].GetDataPtr();
            std::memset(value_buffer_ptr, 0, capacity_ * value_dsizes_[i]);
            value_buffer_ptrs_.push_back(static_cast<uint8_t *>(value_buffer_ptr));
        }
        heap_top_ = &(hashmap_buffer.GetHeapTop().cpu);
    }

    buf_index_t DeviceAllocate() { return heap_[(*heap_top_).fetch_add(1)]; }
    void DeviceFree(buf_index_t buf_index) { heap_[(*heap_top_).fetch_sub(1) - 1] = buf_index; }

    void *GetKeyPtr(buf_index_t buf_index) { return key_buffer_ptr_ + buf_index * key_dsize_; }
    void *GetValuePtr(buf_index_t buf_index, int value_idx = 0) {
        return value_buffer_ptrs_[value_idx] + buf_index * value_dsizes_[value_idx];
    }

public:
    int64_t capacity_;
    int64_t key_dsize_;
    std::vector<int64_t> value_dsizes_;

    buf_index_t *heap_;          /* [N] */
    std::atomic<int> *heap_top_; /* [1] */

    uint8_t *key_buffer_ptr_;                  /* [N] * sizeof(Key) */
    std::vector<uint8_t *> value_buffer_ptrs_; /* [N] * sizeof(Value) */
};

}  // namespace core
}  // namespace vox
