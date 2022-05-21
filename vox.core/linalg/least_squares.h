//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "tensor.h"

namespace vox {
namespace core {

/// Solve AX = B with QR decomposition. A is a full-rank m x n matrix (m >= n).
void LeastSquares(const Tensor& A, const Tensor& B, Tensor& X);

#ifdef BUILD_CUDA_MODULE
void LeastSquaresCUDA(void* A_data, void* B_data, int64_t m, int64_t n, int64_t k, Dtype dtype, const Device& device);
#endif

void LeastSquaresCPU(void* A_data, void* B_data, int64_t m, int64_t n, int64_t k, Dtype dtype, const Device& device);

}  // namespace core
}  // namespace vox
