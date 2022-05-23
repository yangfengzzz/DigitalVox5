//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/parallel.h"
#include "vox.core/hashmap/hash_backend_buffer.h"

namespace vox {
namespace core {
void CPUResetHeap(Tensor& heap) {
    uint32_t* heap_ptr = heap.GetDataPtr<uint32_t>();
    int64_t capacity = heap.GetLength();

#pragma omp parallel for num_threads(utility::EstimateMaxThreads())
    for (int64_t i = 0; i < capacity; ++i) {
        heap_ptr[i] = i;
    }
};
}  // namespace core
}  // namespace vox
