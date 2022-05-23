//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/logging.h"
#include "vox.core/tensor.h"

namespace vox {
namespace core {
namespace kernel {

enum class UnaryEWOpCode {
    Sqrt,
    Sin,
    Cos,
    Neg,
    Exp,
    Abs,
    IsNan,
    IsInf,
    IsFinite,
    Floor,
    Ceil,
    Round,
    Trunc,
    LogicalNot
};

void UnaryEW(const Tensor& src, Tensor& dst, UnaryEWOpCode op_code);

void UnaryEWCPU(const Tensor& src, Tensor& dst, UnaryEWOpCode op_code);

#ifdef BUILD_CUDA_MODULE
void UnaryEWCUDA(const Tensor& src, Tensor& dst, UnaryEWOpCode op_code);
#endif

// Copy is separated from other unary ops since it support cross-device copy and
// dtype casting.
void Copy(const Tensor& src, Tensor& dst);

void CopyCPU(const Tensor& src, Tensor& dst);

#ifdef BUILD_CUDA_MODULE
void CopyCUDA(const Tensor& src, Tensor& dst);
#endif

}  // namespace kernel
}  // namespace core
}  // namespace vox
