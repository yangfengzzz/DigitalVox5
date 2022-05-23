//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.core/tensor.h"

namespace vox {
namespace core {

/// Solve AX = B with LU decomposition. A is a square matrix.
void Solve(const Tensor& A, const Tensor& B, Tensor& X);

void SolveCPU(void* A_data, void* B_data, void* ipiv_data, int64_t n, int64_t k, Dtype dtype, const Device& device);

#ifdef BUILD_CUDA_MODULE
void SolveCUDA(void* A_data, void* B_data, void* ipiv_data, int64_t n, int64_t k, Dtype dtype, const Device& device);
#endif

}  // namespace core
}  // namespace vox
