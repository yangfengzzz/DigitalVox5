//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.core/dispatch.h"
#include "vox.core/indexer.h"
#include "vox.core/linalg/tri_impl.h"
#include "vox.core/parallel_for.h"
#include "vox.core/tensor.h"

namespace vox {
namespace core {

void TriuCPU(const Tensor &A, Tensor &output, const int diagonal) {
    DISPATCH_DTYPE_TO_TEMPLATE(A.GetDtype(), [&]() {
        const scalar_t *A_ptr = static_cast<const scalar_t *>(A.GetDataPtr());
        scalar_t *output_ptr = static_cast<scalar_t *>(output.GetDataPtr());
        int cols = A.GetShape()[1];
        int n = A.GetShape()[0] * cols;

        ParallelFor(A.GetDevice(), n, [&] OPEN3D_DEVICE(int64_t workload_idx) {
            const int64_t idx = workload_idx / cols;
            const int64_t idy = workload_idx % cols;
            if (idy - idx >= diagonal) {
                output_ptr[workload_idx] = A_ptr[idx * cols + idy];
            }
        });
    });
}

void TrilCPU(const Tensor &A, Tensor &output, const int diagonal) {
    DISPATCH_DTYPE_TO_TEMPLATE(A.GetDtype(), [&]() {
        const scalar_t *A_ptr = static_cast<const scalar_t *>(A.GetDataPtr());
        scalar_t *output_ptr = static_cast<scalar_t *>(output.GetDataPtr());
        int cols = A.GetShape()[1];
        int n = A.GetShape()[0] * cols;

        ParallelFor(A.GetDevice(), n, [&] OPEN3D_DEVICE(int64_t workload_idx) {
            const int64_t idx = workload_idx / cols;
            const int64_t idy = workload_idx % cols;
            if (idy - idx <= diagonal) {
                output_ptr[workload_idx] = A_ptr[idx * cols + idy];
            }
        });
    });
}

void TriulCPU(const Tensor &A, Tensor &upper, Tensor &lower, const int diagonal) {
    DISPATCH_DTYPE_TO_TEMPLATE(A.GetDtype(), [&]() {
        const scalar_t *A_ptr = static_cast<const scalar_t *>(A.GetDataPtr());
        scalar_t *upper_ptr = static_cast<scalar_t *>(upper.GetDataPtr());
        scalar_t *lower_ptr = static_cast<scalar_t *>(lower.GetDataPtr());
        int cols = A.GetShape()[1];
        int n = A.GetShape()[0] * cols;

        ParallelFor(A.GetDevice(), n, [&] OPEN3D_DEVICE(int64_t workload_idx) {
            const int64_t idx = workload_idx / cols;
            const int64_t idy = workload_idx % cols;
            if (idy - idx < diagonal) {
                lower_ptr[workload_idx] = A_ptr[idx * cols + idy];
            } else if (idy - idx > diagonal) {
                upper_ptr[workload_idx] = A_ptr[idx * cols + idy];
            } else {
                lower_ptr[workload_idx] = 1;
                upper_ptr[workload_idx] = A_ptr[idx * cols + idy];
            }
        });
    });
}

}  // namespace core
}  // namespace vox
