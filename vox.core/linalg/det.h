//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "tensor.h"

namespace vox {
namespace core {

// See documentation for `core::Tensor::Det`.
double Det(const Tensor& A);

}  // namespace core
}  // namespace vox
