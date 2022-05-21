//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "tensor.h"

namespace vox {
namespace core {

/// Computes A^{-1} with LU factorization, where A is a N x N square matrix.
void Inverse(const Tensor& A, Tensor& output);

void InverseCPU(void* A_data, void* ipiv_data, void* output_data, int64_t n, Dtype dtype, const Device& device);

#ifdef BUILD_CUDA_MODULE
void InverseCUDA(void* A_data, void* ipiv_data, void* output_data, int64_t n, Dtype dtype, const Device& device);
#endif

}  // namespace core
}  // namespace vox
