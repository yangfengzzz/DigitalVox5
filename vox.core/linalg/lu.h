//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "tensor.h"

namespace vox {
namespace core {

// See documentation for `core::Tensor::LUIpiv`.
void LUIpiv(const Tensor& A, Tensor& ipiv, Tensor& output);

// See documentation for `core::Tensor::LU`.
void LU(const Tensor& A, Tensor& permutation, Tensor& lower, Tensor& upper, const bool permute_l = false);

}  // namespace core
}  // namespace vox
