//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.core/tensor.h"

namespace vox {
namespace core {

/// Computes matrix multiplication C = AB.
void Matmul(const Tensor& A, const Tensor& B, Tensor& C);

#ifdef BUILD_CUDA_MODULE
void MatmulCUDA(void* A_data, void* B_data, void* C_data, int64_t m, int64_t k, int64_t n, Dtype dtype);
#endif
void MatmulCPU(void* A_data, void* B_data, void* C_data, int64_t m, int64_t k, int64_t n, Dtype dtype);
}  // namespace core
}  // namespace vox
