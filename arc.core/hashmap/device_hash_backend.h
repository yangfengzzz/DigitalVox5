// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include "cuda_utils.h"
#include "hashmap/hash_backend_buffer.h"
#include "memory_manager.h"
#include "tensor.h"

namespace vox {
namespace core {

enum class HashBackendType;

class DeviceHashBackend {
public:
    DeviceHashBackend(int64_t init_capacity,
                      int64_t key_dsize,
                      const std::vector<int64_t>& value_dsizes,
                      const Device& device)
        : capacity_(init_capacity), key_dsize_(key_dsize), value_dsizes_(value_dsizes), device_(device) {}
    virtual ~DeviceHashBackend() {}

    /// Reserve expects a lot of extra memory space at runtime,
    /// since it consists of
    /// 1) dumping all key value pairs to a buffer
    /// 2) creating a new hash table
    /// 3) parallel inserting dumped key value pairs
    /// 4) deallocating old hash table
    virtual void Reserve(int64_t capacity) = 0;

    /// Parallel insert contiguous arrays of keys and values.
    virtual void Insert(const void* input_keys,
                        const std::vector<const void*>& input_values,
                        buf_index_t* output_buf_indices,
                        bool* output_masks,
                        int64_t count) = 0;

    /// Parallel find a contiguous array of keys.
    virtual void Find(const void* input_keys, buf_index_t* output_buf_indices, bool* output_masks, int64_t count) = 0;

    /// Parallel erase a contiguous array of keys.
    virtual void Erase(const void* input_keys, bool* output_masks, int64_t count) = 0;

    /// Parallel collect all iterators in the hash table
    virtual int64_t GetActiveIndices(buf_index_t* output_buf_indices) = 0;

    /// Clear stored map without reallocating memory.
    virtual void Clear() = 0;

    /// Get the size (number of valid entries) of the hash map.
    virtual int64_t Size() const = 0;

    /// Get the number of buckets of the hash map.
    virtual int64_t GetBucketCount() const = 0;

    /// Get the current load factor, defined as size / bucket count.
    virtual float LoadFactor() const = 0;

    /// Get the maximum capacity of the hash map.
    int64_t GetCapacity() const { return capacity_; }

    /// Get the current device.
    Device GetDevice() const { return device_; }

    /// Get the number of entries per bucket.
    virtual std::vector<int64_t> BucketSizes() const = 0;

    /// Get the key buffer that stores actual keys.
    Tensor GetKeyBuffer() { return buffer_->GetKeyBuffer(); }

    /// Get the value buffers that store actual array of values.
    std::vector<Tensor> GetValueBuffers() { return buffer_->GetValueBuffers(); }

    /// Get the i-th value buffer that store an actual value array.
    Tensor GetValueBuffer(size_t i = 0) { return buffer_->GetValueBuffer(i); }

    virtual void Allocate(int64_t capacity) = 0;
    virtual void Free() = 0;

public:
    int64_t capacity_;

    int64_t key_dsize_;
    std::vector<int64_t> value_dsizes_;

    Device device_;

    std::shared_ptr<HashBackendBuffer> buffer_;
};

/// Factory functions:
/// - Default constructor switch is in DeviceHashBackend.cpp
/// - Default CPU constructor is in CPU/CreateCPUHashBackend.cpp
/// - Default CUDA constructor is in CUDA/CreateCUDAHashBackend.cu
std::shared_ptr<DeviceHashBackend> CreateDeviceHashBackend(int64_t init_capacity,
                                                           const Dtype& key_dtype,
                                                           const SizeVector& key_element_shape,
                                                           const std::vector<Dtype>& value_dtypes,
                                                           const std::vector<SizeVector>& value_element_shapes,
                                                           const Device& device,
                                                           const HashBackendType& backend);

std::shared_ptr<DeviceHashBackend> CreateCPUHashBackend(int64_t init_capacity,
                                                        const Dtype& key_dtype,
                                                        const SizeVector& key_element_shape,
                                                        const std::vector<Dtype>& value_dtypes,
                                                        const std::vector<SizeVector>& value_element_shapes,
                                                        const Device& device,
                                                        const HashBackendType& backend);

std::shared_ptr<DeviceHashBackend> CreateCUDAHashBackend(int64_t init_capacity,
                                                         const Dtype& key_dtype,
                                                         const SizeVector& key_element_shape,
                                                         const std::vector<Dtype>& value_dtypes,
                                                         const std::vector<SizeVector>& value_element_shapes,
                                                         const Device& device,
                                                         const HashBackendType& backend);

}  // namespace core
}  // namespace vox
