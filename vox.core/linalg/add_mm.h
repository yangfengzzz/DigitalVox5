//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "tensor.h"

namespace vox {
namespace core {

/// Computes matrix multiplication C = alpha * A @ B + beta * C.
/// If matrix A is a (n x m) tensor, and B is a (m x p) tensor, C should have a
/// shape (n x p).
/// alpha and beta are scaling factors on matrix-matrix multiplication and the
/// added matrix input respectively.
void AddMM(const Tensor& A, const Tensor& B, Tensor& C, double alpha, double beta);

#ifdef BUILD_CUDA_MODULE
void AddMMCUDA(void* A_data,
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
               Dtype dtype);
#endif

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
              Dtype dtype);

}  // namespace core
}  // namespace vox
