//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "dispatch.h"
#include "kernel/arange.h"
#include "parallel_for.h"
#include "tensor.h"

namespace vox {
namespace core {
namespace kernel {

void ArangeCPU(const Tensor& start, const Tensor& stop, const Tensor& step, Tensor& dst) {
    Dtype dtype = start.GetDtype();
    DISPATCH_DTYPE_TO_TEMPLATE(dtype, [&]() {
        scalar_t sstart = start.Item<scalar_t>();
        scalar_t sstep = step.Item<scalar_t>();
        scalar_t* dst_ptr = dst.GetDataPtr<scalar_t>();
        int64_t n = dst.GetLength();
        ParallelFor(start.GetDevice(), n, [&](int64_t workload_idx) {
            dst_ptr[workload_idx] = sstart + static_cast<scalar_t>(sstep * workload_idx);
        });
    });
}

}  // namespace kernel
}  // namespace core
}  // namespace vox
