//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "tensor.h"

namespace vox {
namespace core {

// See documentation for `core::Tensor::Triu`.
void Triu(const Tensor& A, Tensor& output, const int diagonal = 0);

// See documentation for `core::Tensor::Tril`.
void Tril(const Tensor& A, Tensor& output, const int diagonal = 0);

// See documentation for `core::Tensor::Triul`.
void Triul(const Tensor& A, Tensor& upper, Tensor& lower, const int diagonal = 0);

}  // namespace core
}  // namespace vox
