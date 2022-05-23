//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.core/linalg/blas_wrapper.h"
#include "vox.core/linalg/linalg_utils.h"
#include "vox.core/linalg/mat_mul.h"

namespace vox {
namespace core {
void MatmulCPU(void* A_data, void* B_data, void* C_data, int64_t m, int64_t k, int64_t n, Dtype dtype) {
    DISPATCH_LINALG_DTYPE_TO_TEMPLATE(dtype, [&]() {
        scalar_t alpha = 1, beta = 0;
        gemm_cpu<scalar_t>(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha,
                           static_cast<const scalar_t*>(A_data), m, static_cast<const scalar_t*>(B_data), k, beta,
                           static_cast<scalar_t*>(C_data), m);
    });
}

}  // namespace core
}  // namespace vox
