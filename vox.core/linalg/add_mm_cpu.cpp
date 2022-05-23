//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/logging.h"
#include "vox.core/linalg/add_mm.h"
#include "vox.core/linalg/blas_wrapper.h"
#include "vox.core/linalg/linalg_utils.h"

namespace vox {
namespace core {

void AddMMCPU(void* A_data,
              void* B_data,
              void* C_data,
              int64_t m,
              int64_t k,
              int64_t n,
              double alpha,
              double beta,
              bool gemmTrA,
              bool gemmTrB,
              int lda,
              int ldb,
              int ldc,
              Dtype dtype) {
    DISPATCH_DTYPE_TO_TEMPLATE(dtype, [&]() {
        gemm_cpu(CblasColMajor, gemmTrA ? CblasTrans : CblasNoTrans, gemmTrB ? CblasTrans : CblasNoTrans, m, n, k,
                 static_cast<scalar_t>(alpha), static_cast<const scalar_t*>(A_data), lda,
                 static_cast<const scalar_t*>(B_data), ldb, static_cast<scalar_t>(beta), static_cast<scalar_t*>(C_data),
                 ldc);
    });
}

}  // namespace core
}  // namespace vox
