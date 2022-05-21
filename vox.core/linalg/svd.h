//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "tensor.h"

namespace vox {
namespace core {

/// Computes SVD decomposition A = U S VT, where A is an m x n, U is an m x m, S
/// is a min(m, n), VT is an n x n tensor.
void SVD(const Tensor& A, Tensor& U, Tensor& S, Tensor& VT);

#ifdef BUILD_CUDA_MODULE
void SVDCUDA(const void* A_data,
             void* U_data,
             void* S_data,
             void* VT_data,
             void* superb_data,
             int64_t m,
             int64_t n,
             Dtype dtype,
             const Device& device);
#endif

void SVDCPU(const void* A_data,
            void* U_data,
            void* S_data,
            void* VT_data,
            void* superb_data,
            int64_t m,
            int64_t n,
            Dtype dtype,
            const Device& device);

}  // namespace core
}  // namespace vox
