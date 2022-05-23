//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.core/linalg/tri.h"
#include "vox.core/tensor.h"

namespace vox {
namespace core {

void TriuCPU(const Tensor& A, Tensor& output, const int diagonal = 0);

void TrilCPU(const Tensor& A, Tensor& output, const int diagonal = 0);

void TriulCPU(const Tensor& A, Tensor& upper, Tensor& lower, const int diagonal = 0);

#ifdef BUILD_CUDA_MODULE
void TriuCUDA(const Tensor& A, Tensor& output, const int diagonal = 0);

void TrilCUDA(const Tensor& A, Tensor& output, const int diagonal = 0);

void TriulCUDA(const Tensor& A, Tensor& upper, Tensor& lower, const int diagonal = 0);
#endif
}  // namespace core
}  // namespace vox
